#include "TrafficGenerator.hpp"

#include <chrono>

TrafficGenerator::TrafficGenerator(const Topology &topology, Garage &garage, TrafficManager &trafficManager)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()))
{
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
    std::uniform_int_distribution<int> dispatchCountDistribution(5, 20);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);
    while (remainingDispatches > 0)
    {
        Engine *engine = garage.getIdleEngine();
        if (engine == nullptr || !dispatchEngine(*engine))
            break;
        else
            --remainingDispatches;
    }
}

bool TrafficGenerator::dispatchEngine(Engine &engine)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2)
        return false;

    std::uniform_int_distribution<qsizetype> stationDistribution(0, stations.size() - 1);
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &fromStation = stations[stationDistribution(randomGenerator)];
        const Node &toStation = stations[stationDistribution(randomGenerator)];
        if (fromStation.getId() != toStation.getId())
        {
            if (trafficManager.contractRoute(engine, fromStation.getId(), toStation.getId()))
                return true;
        }
    }
    return false;
}
