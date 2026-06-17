#pragma once

#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include <QElapsedTimer>
#include <QObject>

class TrafficSimulator : public QObject
{
public:
    TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, QObject *parent = nullptr);
    void start();
    double getCurrentSimulationTimeSeconds() const;

private:
    void processScheduledWork();
    void scheduleNextWork();

    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    QElapsedTimer simulationClock;
    bool started = false;
    double lastGeneratorUpdateSeconds = 0.0;
};
