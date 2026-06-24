#pragma once

#include "SimulationStatistics.hpp"
#include <cstddef>

class Garage;
class PassengerDispatcher;
class Topology;
class TrafficBalancer;
class TrafficManager;

class SimulationStatisticsCollector
{
public:
    SimulationStatisticsCollector(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, const PassengerDispatcher &passengerDispatcher, const TrafficBalancer &trafficBalancer);

    void update(double currentSimulationTimeSeconds);
    SimulationStatistics &getStatistics();
    const SimulationStatistics &getStatistics() const;

private:
    void collectEvents();
    void collectStationSnapshots(double currentSimulationTimeSeconds);

    const Topology &topology;
    const Garage &garage;
    const TrafficManager &trafficManager;
    const PassengerDispatcher &passengerDispatcher;
    const TrafficBalancer &trafficBalancer;
    SimulationStatistics statistics;
    std::size_t collectedRouteDispatchCount = 0;
    std::size_t collectedCompletedTripCount = 0;
    double lastStationSnapshotTimeSeconds = 0.0;
    bool hasStationSnapshot = false;
};
