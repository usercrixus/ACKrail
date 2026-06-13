#pragma once

#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include <thread>

class TrafficSimulator
{
public:
    TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator);
    ~TrafficSimulator();
    void start();
    void stop();

private:
    void run(std::stop_token stopToken);
    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    std::jthread workerThread;
};
