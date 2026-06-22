#pragma once

#include "EngineStatistics.hpp"
#include "StationStatistics.hpp"
#include <cstddef>

class SimulationStatistics
{
public:
    EngineStatistics &getEngineStatistics();
    const EngineStatistics &getEngineStatistics() const;
    StationStatistics &getStationStatistics();
    const StationStatistics &getStationStatistics() const;

    void recordFailedPassengerDispatch();
    void recordFailedRebalancingDispatch();
    std::size_t getFailedPassengerDispatchCount() const;
    std::size_t getFailedRebalancingDispatchCount() const;
    void clear();

private:
    EngineStatistics engineStatistics;
    StationStatistics stationStatistics;
    std::size_t failedPassengerDispatchCount = 0;
    std::size_t failedRebalancingDispatchCount = 0;
};
