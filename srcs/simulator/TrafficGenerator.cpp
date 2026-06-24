#include "TrafficGenerator.hpp"

#include <chrono>

TrafficGenerator::TrafficGenerator(const Topology &topology,
                                   Garage &garage,
                                   PassengerDispatcher &passengerDispatcher)
    : topology(topology),
      garage(garage),
      passengerDispatcher(passengerDispatcher),
      randomGenerator(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count())),
      arrivalStationDistribution(topology.getArrivalWeights().begin(), topology.getArrivalWeights().end()),
      departureStationDistribution(topology.getDepartureWeights().begin(), topology.getDepartureWeights().end())
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
    std::uniform_int_distribution<int> dispatchCountDistribution(10, 25);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);
    while (remainingDispatches > 0)
    {
        generatePassengerRequest(currentSimulationTimeSeconds);
        --remainingDispatches;
    }
}

void TrafficGenerator::generatePassengerRequest(double currentSimulationTimeSeconds)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2)
        return;

    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &fromStation = stations[static_cast<qsizetype>(departureStationDistribution(randomGenerator))];
        const Node &toStation = stations[static_cast<qsizetype>(arrivalStationDistribution(randomGenerator))];
        if (fromStation.getId() != toStation.getId())
        {
            passengerDispatcher.enqueue(fromStation.getId(),
                                        toStation.getId(),
                                        currentSimulationTimeSeconds);
            return;
        }
    }
}

void TrafficGenerator::initializeEngineParkingStations()
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.isEmpty())
        return;

    for (Engine *engine : garage.getIdleEngines())
        garage.setIdleEngineParkingStation(*engine, stations[static_cast<qsizetype>(departureStationDistribution(randomGenerator))].getId());
}
