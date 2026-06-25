#pragma once

#include "TrafficManager.hpp"
#include <QElapsedTimer>
#include <QObject>
#include <functional>

class TrafficSimulator : public QObject
{
public:
    TrafficSimulator(TrafficManager &trafficManager, QObject *parent = nullptr);
    void start();
    double getCurrentSimulationTimeSeconds() const;
    void setUpdateObserver(std::function<void(double)> observer);

private:
    void processScheduledWork();
    void scheduleNextWork();

    TrafficManager &trafficManager;
    std::function<void(double)> updateObserver;
    QElapsedTimer simulationClock;
    bool started = false;
    double lastUpdateSeconds = 0.0;
};
