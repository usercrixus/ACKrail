#pragma once

#include "TrafficBalancer.hpp"
#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include "PassengerDispatcher.hpp"
#include <QElapsedTimer>
#include <QObject>
#include <functional>

class TrafficSimulator : public QObject
{
public:
    TrafficSimulator(TrafficManager &trafficManager,
                     TrafficGenerator &trafficGenerator,
                     PassengerDispatcher &passengerDispatcher,
                     TrafficBalancer &trafficBalancer,
                     QObject *parent = nullptr);
    void start();
    double getCurrentSimulationTimeSeconds() const;
    void setUpdateObserver(std::function<void(double)> observer);

private:
    void processScheduledWork();
    void scheduleNextWork();

    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    PassengerDispatcher &passengerDispatcher;
    TrafficBalancer &trafficBalancer;
    std::function<void(double)> updateObserver;
    QElapsedTimer simulationClock;
    bool started = false;
    double lastGeneratorUpdateSeconds = 0.0;
    double lastBalancerUpdateSeconds = 0.0;
};
