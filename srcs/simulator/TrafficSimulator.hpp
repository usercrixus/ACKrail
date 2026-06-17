#pragma once

#include "TrafficBalancer.hpp"
#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include <QElapsedTimer>
#include <QObject>

class TrafficSimulator : public QObject
{
public:
    TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, TrafficBalancer &trafficBalancer, QObject *parent = nullptr);
    void start();
    double getCurrentSimulationTimeSeconds() const;

private:
    void processScheduledWork();
    void scheduleNextWork();

    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    TrafficBalancer &trafficBalancer;
    QElapsedTimer simulationClock;
    bool started = false;
    double lastGeneratorUpdateSeconds = 0.0;
    double lastBalancerUpdateSeconds = 0.0;
};
