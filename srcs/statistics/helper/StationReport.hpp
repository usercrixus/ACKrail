#pragma once

#include <cstddef>

class StationReport
{
public:
    int stationId = -1;
    double targetIdleEngines = 0.0;
    double currentIdleEngines = 0.0;
    std::size_t currentWaitingPassengers = 0;
    double averageIdleEngines = 0.0;
    double minimumIdleEngines = 0.0;
    double maximumIdleEngines = 0.0;
    double timeWithNoIdleEngineSeconds = 0.0;
    double timeBelowTargetSeconds = 0.0;
    double timeAboveTargetSeconds = 0.0;
    double averageTargetGap = 0.0;
    double observedTimeSeconds = 0.0;
    std::size_t departureCount = 0;
    std::size_t arrivalCount = 0;
};
