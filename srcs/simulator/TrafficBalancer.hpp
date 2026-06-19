#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficManager.hpp"
#include <random>
#include <unordered_map>

class TrafficBalancer
{
public:
    TrafficBalancer(const Topology &topology, Garage &garage, TrafficManager &trafficManager);

    void tryRebalance(double currentSimulationTimeSeconds, double elapsedSeconds);
    double getSecondsUntilRebalance() const;

private:
    void initializeStationWeightsFromTopology();
    void rebalance(double currentSimulationTimeSeconds);
    std::size_t getIdleEngineCountAtStation(int stationId) const;
    double getTargetIdleEngineCountAtStation(int stationId) const;

    const Topology &topology;
    Garage &garage;
    TrafficManager &trafficManager;
    std::mt19937 randomGenerator;
    std::unordered_map<int, double> stationWeights;
    double totalStationWeight = 0.0;
    double secondsUntilRebalance = 5.0;
    std::size_t maxRebalanceDispatchesPerPass = 50;
    std::size_t stationBalanceMargin = 2;
};
