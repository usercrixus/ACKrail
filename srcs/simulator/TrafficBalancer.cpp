#include "TrafficBalancer.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <vector>

TrafficBalancer::TrafficBalancer(const Topology &topology, Garage &garage, TrafficManager &trafficManager)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
    initializeStationWeightsFromTopology();
}

void TrafficBalancer::tryRebalance(double currentSimulationTimeSeconds, double elapsedSeconds)
{
    if (elapsedSeconds <= 0.0)
        return;

    secondsUntilRebalance -= elapsedSeconds;
    while (secondsUntilRebalance <= 0.0)
    {
        rebalance(currentSimulationTimeSeconds);
        secondsUntilRebalance += 5.0;
    }
}

double TrafficBalancer::getSecondsUntilRebalance() const
{
    return secondsUntilRebalance;
}

void TrafficBalancer::initializeStationWeightsFromTopology()
{
    stationWeights.clear();
    totalStationWeight = 0.0;

    const QVector<Node> &stations = topology.getNodes();
    const std::vector<double> &arrivalWeights = topology.getArrivalWeights();
    const std::vector<double> &departureWeights = topology.getDepartureWeights();

    for (qsizetype stationIndex = 0; stationIndex < stations.size(); ++stationIndex)
    {
        const std::size_t weightIndex = static_cast<std::size_t>(stationIndex);
        const double arrivalWeight = weightIndex < arrivalWeights.size() ? arrivalWeights[weightIndex] : 1.0;
        const double departureWeight = weightIndex < departureWeights.size() ? departureWeights[weightIndex] : 1.0;
        const double stationWeight = std::max(0.0, departureWeight - arrivalWeight);
        stationWeights[stations[stationIndex].getId()] = stationWeight;
        totalStationWeight += stationWeight;
    }

    if (totalStationWeight > 0.0)
        return;

    stationWeights.clear();
    totalStationWeight = 0.0;
    for (qsizetype stationIndex = 0; stationIndex < stations.size(); ++stationIndex)
    {
        const std::size_t weightIndex = static_cast<std::size_t>(stationIndex);
        const double stationWeight = weightIndex < departureWeights.size() ? departureWeights[weightIndex] : 1.0;
        stationWeights[stations[stationIndex].getId()] = stationWeight;
        totalStationWeight += stationWeight;
    }
}

void TrafficBalancer::rebalance(double currentSimulationTimeSeconds)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2 || garage.getIdleEngines().empty() || totalStationWeight <= 0.0)
        return;

    std::vector<int> surplusStationIds;
    std::vector<int> deficitStationIds;
    surplusStationIds.reserve(stations.size());
    deficitStationIds.reserve(stations.size());

    for (const Node &station : stations)
    {
        const std::size_t idleCount = getIdleEngineCountAtStation(station.getId());
        const double targetIdleEngines = getTargetIdleEngineCountAtStation(station.getId());
        if (targetIdleEngines <= static_cast<double>(stationBalanceMargin))
            continue;
        if (static_cast<double>(idleCount) > targetIdleEngines + static_cast<double>(stationBalanceMargin))
            surplusStationIds.push_back(station.getId());
        else if (static_cast<double>(idleCount + stationBalanceMargin) < targetIdleEngines)
            deficitStationIds.push_back(station.getId());
    }

    if (surplusStationIds.empty() || deficitStationIds.empty())
        return;

    std::shuffle(surplusStationIds.begin(), surplusStationIds.end(), randomGenerator);
    std::shuffle(deficitStationIds.begin(), deficitStationIds.end(), randomGenerator);

    std::size_t dispatched = 0;
    std::size_t surplusIndex = 0;
    for (const int deficitStationId : deficitStationIds)
    {
        while (surplusIndex < surplusStationIds.size() && dispatched < maxRebalanceDispatchesPerPass)
        {
            const int surplusStationId = surplusStationIds[surplusIndex];
            const auto stationPool = garage.getIdleEnginesByStation().find(surplusStationId);
            const double surplusTargetIdleEngines = getTargetIdleEngineCountAtStation(surplusStationId);
            if (stationPool == garage.getIdleEnginesByStation().end()
                || static_cast<double>(stationPool->second.size()) <= surplusTargetIdleEngines + static_cast<double>(stationBalanceMargin))
            {
                ++surplusIndex;
                continue;
            }

            Engine *engine = stationPool->second.random(randomGenerator);
            if (trafficManager.contractRoute(*engine, surplusStationId, deficitStationId, currentSimulationTimeSeconds, EnginePad::TravelType::Rebalancing))
            {
                ++dispatched;
                break;
            }
            ++surplusIndex;
        }

        if (surplusIndex >= surplusStationIds.size() || dispatched >= maxRebalanceDispatchesPerPass)
            break;
    }
}

std::size_t TrafficBalancer::getIdleEngineCountAtStation(int stationId) const
{
    const auto stationPool = garage.getIdleEnginesByStation().find(stationId);
    return stationPool == garage.getIdleEnginesByStation().end() ? 0 : stationPool->second.size();
}

double TrafficBalancer::getTargetIdleEngineCountAtStation(int stationId) const
{
    const auto stationWeight = stationWeights.find(stationId);
    if (stationWeight == stationWeights.end() || totalStationWeight <= 0.0)
        return 0.0;
    return static_cast<double>(garage.getEngineCount()) * stationWeight->second / totalStationWeight;
}
