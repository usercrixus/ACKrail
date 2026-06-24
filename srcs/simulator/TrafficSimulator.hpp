#pragma once

#include "TrafficBalancer.hpp"
#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include "PassengerDispatcher.hpp"
#include "../garage/Garage.hpp"
#include "../statistics/SimulationStatistics.hpp"
#include "../topology/Topology.hpp"
#include <QElapsedTimer>
#include <QObject>

class TrafficSimulator : public QObject
{
public:
    TrafficSimulator(const Topology &topology,
                     const Garage &garage,
                     TrafficManager &trafficManager,
                     TrafficGenerator &trafficGenerator,
                     PassengerDispatcher &passengerDispatcher,
                     TrafficBalancer &trafficBalancer,
                     SimulationStatistics &statistics,
                     QObject *parent = nullptr);
    void start();
    double getCurrentSimulationTimeSeconds() const;

private:
    void processScheduledWork();
    void recordStationSnapshots(double currentSimulationTimeSeconds);
    void scheduleNextWork();

    const Topology &topology;
    const Garage &garage;
    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    PassengerDispatcher &passengerDispatcher;
    TrafficBalancer &trafficBalancer;
    SimulationStatistics &statistics;
    QElapsedTimer simulationClock;
    bool started = false;
    double lastGeneratorUpdateSeconds = 0.0;
    double lastBalancerUpdateSeconds = 0.0;
    double lastStationStatisticsUpdateSeconds = 0.0;
};
