#include "TrafficGenerator.hpp"

#include <chrono>
#include <vector>

TrafficGenerator::TrafficGenerator(const Topology &topology, Garage &garage, TrafficManager &trafficManager)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
    initializeEngineParkingStations();
}

void TrafficGenerator::tryGenerate(double currentSimulationTimeSeconds, double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        secondsUntilDispatch -= elapsedSeconds;
        while (secondsUntilDispatch <= 0.0)
        {
            generate(currentSimulationTimeSeconds);
            secondsUntilDispatch += 0.10;
        }
    }
}

double TrafficGenerator::getSecondsUntilDispatch() const
{
    return secondsUntilDispatch;
}

void TrafficGenerator::generate(double currentSimulationTimeSeconds)
{
    std::uniform_int_distribution<int> dispatchCountDistribution(50, 75);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);
    while (remainingDispatches > 0)
    {
        Engine *engine = garage.getRandomIdleEngine(randomGenerator);
        if (engine == nullptr || !dispatchEngine(*engine, currentSimulationTimeSeconds))
            break;
        else
            --remainingDispatches;
    }
}

bool TrafficGenerator::dispatchEngine(Engine &engine, double currentSimulationTimeSeconds)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2 || !engine.getPad().hasParkingStation())
        return false;

    std::vector<double> arrivalWeights;
    arrivalWeights.reserve(static_cast<std::size_t>(stations.size()));
    for (const Node &station : stations)
        arrivalWeights.push_back(station.getArrivalWeight());
    std::discrete_distribution<int> stationDistribution(arrivalWeights.begin(), arrivalWeights.end());

    const int fromStationId = engine.getPad().getParkingStationId();
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &toStation = stations[static_cast<qsizetype>(stationDistribution(randomGenerator))];
        if (fromStationId != toStation.getId())
        {
            if (trafficManager.contractRoute(engine, fromStationId, toStation.getId(), currentSimulationTimeSeconds, EnginePad::TravelType::Passenger))
                return true;
        }
    }
    return false;
}

void TrafficGenerator::initializeEngineParkingStations()
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.isEmpty())
        return;

    std::vector<double> departureWeights;
    departureWeights.reserve(static_cast<std::size_t>(stations.size()));
    for (const Node &station : stations)
        departureWeights.push_back(station.getDepartureWeight());
    std::discrete_distribution<int> stationDistribution(departureWeights.begin(), departureWeights.end());

    for (Engine *engine : garage.getIdleEngines())
        garage.setIdleEngineParkingStation(*engine, stations[static_cast<qsizetype>(stationDistribution(randomGenerator))].getId());
}
