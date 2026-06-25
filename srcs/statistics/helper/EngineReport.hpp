#pragma once

#include <cstddef>

class EngineReport
{
public:
    int engineId = -1;
    std::size_t tripCount = 0;
    double passengerDistanceKilometers = 0.0;
    double rebalancingDistanceKilometers = 0.0;
    double totalDistanceKilometers = 0.0;
    double passengerTimeSeconds = 0.0;
    double rebalancingTimeSeconds = 0.0;
    double totalTravelTimeSeconds = 0.0;
    double totalWaitSeconds = 0.0;
    double averageWaitSeconds = 0.0;
    double maximumWaitSeconds = 0.0;
};
