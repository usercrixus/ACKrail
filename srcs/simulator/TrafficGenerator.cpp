#include "TrafficGenerator.hpp"

#include <chrono>

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

    std::uniform_int_distribution<qsizetype> stationDistribution(0, stations.size() - 1);
    const int fromStationId = engine.getPad().getParkingStationId();
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &toStation = stations[stationDistribution(randomGenerator)];
        if (fromStationId != toStation.getId())
        {
            if (trafficManager.contractRoute(engine, fromStationId, toStation.getId(), currentSimulationTimeSeconds))
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

    qsizetype stationIndex = 0;
    for (Engine *engine : garage.getIdleEngines())
    {
        garage.setIdleEngineParkingStation(*engine, stations[stationIndex].getId());
        stationIndex = (stationIndex + 1) % stations.size();
    }
}
