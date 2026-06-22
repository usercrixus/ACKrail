#include "TrafficBalancer.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>

TrafficBalancer::TrafficBalancer(const Topology &topology, Garage &garage, TrafficManager &trafficManager)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
    initializeStationPressuresFromTopology();
}

void TrafficBalancer::tryRebalance(double currentSimulationTimeSeconds, double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        secondsUntilRebalance -= elapsedSeconds;
        while (secondsUntilRebalance <= 0.0)
        {
            rebalance(currentSimulationTimeSeconds);
            secondsUntilRebalance += 5.0;
        }
    }
}

double TrafficBalancer::getSecondsUntilRebalance() const
{
    return secondsUntilRebalance;
}

double TrafficBalancer::getNetworkBalancePercent() const
{
    double coveredTarget = 0.0;
    double totalTarget = 0.0;

    for (const Node &station : topology.getNodes())
    {
        const double targetEngineCount = static_cast<double>(getTargetEngineCountAtStation(station.getId()));
        const double projectedEngineCount = static_cast<double>(getProjectedEngineCountAtStation(station.getId()));
        coveredTarget += std::min(projectedEngineCount, targetEngineCount);
        totalTarget += targetEngineCount;
    }

    if (totalTarget <= 0.0)
        return 100.0;
    return std::clamp(100.0 * coveredTarget / totalTarget, 0.0, 100.0);
}

void TrafficBalancer::initializeStationPressuresFromTopology()
{
    deficitStations.clear();
    surplusStations.clear();
    totalDeficitPressure = 0.0;

    const QVector<Node> &stations = topology.getNodes();
    const std::vector<double> &arrivalWeights = topology.getArrivalWeights();
    const std::vector<double> &departureWeights = topology.getDepartureWeights();
    for (qsizetype stationIndex = 0; stationIndex < stations.size(); ++stationIndex)
    {
        const std::size_t weightIndex = static_cast<std::size_t>(stationIndex);
        const double arrivalWeight = weightIndex < arrivalWeights.size() ? arrivalWeights[weightIndex] : 1.0;
        const double departureWeight = weightIndex < departureWeights.size() ? departureWeights[weightIndex] : 1.0;
        const double pressure = departureWeight - arrivalWeight;
        if (pressure > 0.0)
        {
            deficitStations.push_back({stations[stationIndex].getId(), pressure});
            totalDeficitPressure += pressure;
        }
        else if (pressure < 0.0)
            surplusStations.push_back({stations[stationIndex].getId(), -pressure});
    }

    const auto byPressureDescending = [](const StationPressure &left, const StationPressure &right)
    {
        return left.pressure > right.pressure;
    };
    std::sort(deficitStations.begin(), deficitStations.end(), byPressureDescending);
    std::sort(surplusStations.begin(), surplusStations.end(), byPressureDescending);
}

void TrafficBalancer::rebalance(double currentSimulationTimeSeconds)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2
        || garage.getIdleEngines().empty()
        || totalDeficitPressure <= 0.0
        || deficitStations.empty()
        || surplusStations.empty())
        return;

    const std::size_t baseReserveCount = getBaseReserveCountPerStation();

    for (const StationPressure &deficitStation : deficitStations)
    {
        const std::size_t targetEngineCount = getTargetEngineCountAtStation(deficitStation);
        std::vector<const StationPressure *> surplusStationsByCost;
        surplusStationsByCost.reserve(surplusStations.size());
        for (const StationPressure &surplusStation : surplusStations)
            surplusStationsByCost.push_back(&surplusStation);
        std::sort(surplusStationsByCost.begin(), surplusStationsByCost.end(), [this, &deficitStation](const StationPressure *left, const StationPressure *right)
                  {
                      return trafficManager.getStaticRouteDistanceKilometers(left->stationId, deficitStation.stationId)
                             < trafficManager.getStaticRouteDistanceKilometers(right->stationId, deficitStation.stationId);
                  });

        for (const StationPressure *surplusStation : surplusStationsByCost)
        {
            while (getProjectedEngineCountAtStation(deficitStation.stationId) < targetEngineCount
                   && getIdleEngineCountAtStation(surplusStation->stationId) > baseReserveCount)
            {
                const auto stationPool = garage.getIdleEnginesByStation().find(surplusStation->stationId);
                if (stationPool == garage.getIdleEnginesByStation().end() || stationPool->second.empty())
                    break;
                Engine *engine = stationPool->second.random(randomGenerator);
                if (!trafficManager.contractPrecomputedRoute(*engine, surplusStation->stationId, deficitStation.stationId, currentSimulationTimeSeconds, EnginePad::TravelType::Rebalancing))
                    break;
                if (garage.getIdleEngines().empty())
                    return;
            }
        }
    }
}

std::size_t TrafficBalancer::getIdleEngineCountAtStation(int stationId) const
{
    const auto stationPool = garage.getIdleEnginesByStation().find(stationId);
    return stationPool == garage.getIdleEnginesByStation().end() ? 0 : stationPool->second.size();
}

std::size_t TrafficBalancer::getProjectedEngineCountAtStation(int stationId) const
{
    for (const Node &station : topology.getNodes())
    {
        if (station.getId() == stationId)
            return getIdleEngineCountAtStation(stationId) + station.getController().getExpectedArrivalCount();
    }
    return getIdleEngineCountAtStation(stationId);
}

std::size_t TrafficBalancer::getBaseReserveCountPerStation() const
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.isEmpty())
        return 0;

    const double reserveCount = static_cast<double>(garage.getEngineCount()) * baseReserveRatio / static_cast<double>(stations.size());
    return static_cast<std::size_t>(std::ceil(reserveCount));
}

std::size_t TrafficBalancer::getTargetEngineCountAtStation(int stationId) const
{
    for (const StationPressure &deficitStation : deficitStations)
    {
        if (deficitStation.stationId == stationId)
            return getTargetEngineCountAtStation(deficitStation);
    }
    return getBaseReserveCountPerStation();
}

std::size_t TrafficBalancer::getTargetEngineCountAtStation(const StationPressure &stationPressure) const
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.isEmpty() || totalDeficitPressure <= 0.0)
        return 0;

    const std::size_t baseReserveCount = getBaseReserveCountPerStation();
    const std::size_t baseReserveFleet = baseReserveCount * static_cast<std::size_t>(stations.size());
    const std::size_t engineCount = garage.getEngineCount();
    const std::size_t pressureFleet = engineCount > baseReserveFleet ? engineCount - baseReserveFleet : 0;
    const double pressureTarget = static_cast<double>(pressureFleet) * stationPressure.pressure / totalDeficitPressure;
    return baseReserveCount + static_cast<std::size_t>(std::ceil(pressureTarget));
}
