#pragma once

#include <cstddef>

class StationsGlobalReport
{
public:
    std::size_t stationCount = 0;
    std::size_t stationsWithoutIdleEngine = 0;
    std::size_t departureCount = 0;
    std::size_t arrivalCount = 0;
    std::size_t currentWaitingPassengers = 0;
    double currentIdleEngines = 0.0;
    double targetIdleEngines = 0.0;
    double averageIdleEnginesPerStation = 0.0;
    double averageTargetGap = 0.0;
    double observedStationTimeSeconds = 0.0;
    double timeWithNoIdleEngineSeconds = 0.0;
    double timeBelowTargetSeconds = 0.0;
};
