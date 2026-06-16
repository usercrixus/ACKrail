#include "TrafficGenerator.hpp"

#include <chrono>
#include <mutex>

TrafficGenerator::TrafficGenerator(const Topology &topology, Garage &garage, TrafficManager &trafficManager)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
    initializeEngineParkingStations();
}

void TrafficGenerator::tryGenerate(double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        secondsUntilDispatch -= elapsedSeconds;
        while (secondsUntilDispatch <= 0.0)
        {
            generate();
            secondsUntilDispatch += 1.0;
        }
    }
}

void TrafficGenerator::generate()
{
    std::uniform_int_distribution<int> dispatchCountDistribution(300, 400);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);
    while (remainingDispatches > 0)
    {
        Engine *engine = garage.getRandomIdleEngine(randomGenerator);
        if (engine == nullptr || !dispatchEngine(*engine))
            break;
        else
            --remainingDispatches;
    }
}

bool TrafficGenerator::dispatchEngine(Engine &engine)
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
            if (trafficManager.contractRoute(engine, fromStationId, toStation.getId()))
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

    const std::lock_guard lock(garage.getMutex());
    qsizetype stationIndex = 0;
    for (const auto &[id, engine] : garage.getIdleEngines())
    {
        engine->getPad().setParkingStationId(stations[stationIndex].getId());
        stationIndex = (stationIndex + 1) % stations.size();
    }
}
