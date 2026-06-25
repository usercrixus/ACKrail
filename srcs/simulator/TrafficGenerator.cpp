#include "TrafficGenerator.hpp"

#include <chrono>

TrafficGenerator::TrafficGenerator(const Topology &topology,
                                   Garage &garage,
                                   TrafficPassenger &trafficPassenger)
    : topology(topology),
      garage(garage),
      trafficPassenger(trafficPassenger),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count())),
      arrivalStationDistribution(topology.getArrivalWeights().begin(), topology.getArrivalWeights().end()),
      departureStationDistribution(topology.getDepartureWeights().begin(), topology.getDepartureWeights().end())
{
}

void TrafficGenerator::initialize()
{
    if (initialized)
        return;
    initialized = true;
    initializeEngineParkingStations();
}

PassengerGenerationResult TrafficGenerator::tryGenerate(double currentSimulationTimeSeconds, double elapsedSeconds)
{
    PassengerGenerationResult result;
    if (elapsedSeconds > 0.0)
    {
        secondsUntilDispatch -= elapsedSeconds;
        while (secondsUntilDispatch <= 0.0)
        {
            const PassengerGenerationResult generation =
                generate(currentSimulationTimeSeconds);
            result.generatedPassengerCount += generation.generatedPassengerCount;
            secondsUntilDispatch += 0.10;
        }
    }
    return result;
}

double TrafficGenerator::getSecondsUntilDispatch() const
{
    return secondsUntilDispatch;
}

PassengerGenerationResult TrafficGenerator::generate(double currentSimulationTimeSeconds)
{
    PassengerGenerationResult result;
    std::uniform_int_distribution<int> dispatchCountDistribution(10, 25);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);
    while (remainingDispatches > 0)
    {
        if (generatePassengerRequest(currentSimulationTimeSeconds))
            ++result.generatedPassengerCount;
        --remainingDispatches;
    }
    return result;
}

bool TrafficGenerator::generatePassengerRequest(double currentSimulationTimeSeconds)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2)
        return false;

    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &fromStation = stations[static_cast<qsizetype>(departureStationDistribution(randomGenerator))];
        const Node &toStation = stations[static_cast<qsizetype>(arrivalStationDistribution(randomGenerator))];
        if (fromStation.getId() != toStation.getId())
        {
            trafficPassenger.enqueue(fromStation.getId(), toStation.getId(), currentSimulationTimeSeconds);
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

    for (Engine *engine : garage.getIdleEngines())
    {
        if (!engine->getPad().hasParkingStation())
            garage.setIdleEngineParkingStation(*engine, stations[static_cast<qsizetype>(departureStationDistribution(randomGenerator))].getId());
    }
}
