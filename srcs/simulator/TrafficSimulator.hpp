#pragma once

#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"

class TrafficSimulator
{
public:
    TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator);
    void advance(double elapsedSeconds);

private:
    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
};
