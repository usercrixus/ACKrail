#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficOperations.hpp"
#include <random>
#include <vector>

class TrafficPassenger;
class TrafficManager;

class TrafficBalancer
{
public:
    TrafficBalancer(const Topology &topology,
                    Garage &garage,
                    TrafficOperations &trafficOperations,
                    const TrafficPassenger &trafficPassenger);
    TrafficBalancer(const Topology &topology,
                    Garage &garage,
                    TrafficManager &trafficManager,
                    const TrafficPassenger &trafficPassenger);

    void tryRebalance(double currentSimulationTimeSeconds, double elapsedSeconds);
    double getSecondsUntilRebalance() const;
    double getNetworkBalancePercent() const;
    std::size_t getTargetEngineCountAtStation(int stationId) const;

private:
    struct StationPressure
    {
        int stationId;
        double pressure;
    };

    void initializeStationPressuresFromTopology();
    void rebalance(double currentSimulationTimeSeconds);
    std::size_t getIdleEngineCountAtStation(int stationId) const;
    std::size_t getProjectedEngineCountAtStation(int stationId) const;
    std::size_t getBaseReserveCountPerStation() const;
    std::size_t getTargetEngineCountAtStation(const StationPressure &stationPressure) const;

    const Topology &topology;
    Garage &garage;
    TrafficOperations &trafficOperations;
    const TrafficPassenger &trafficPassenger;
    std::mt19937 randomGenerator;
    std::vector<StationPressure> deficitStations;
    std::vector<StationPressure> surplusStations;
    double totalDeficitPressure = 0.0;
    double secondsUntilRebalance = 0.10;
    double baseReserveRatio = 0.20;
};
