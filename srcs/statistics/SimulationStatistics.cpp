#include "SimulationStatistics.hpp"

EngineStatistics &SimulationStatistics::getEngineStatistics()
{
    return engineStatistics;
}

const EngineStatistics &SimulationStatistics::getEngineStatistics() const
{
    return engineStatistics;
}

StationStatistics &SimulationStatistics::getStationStatistics()
{
    return stationStatistics;
}

const StationStatistics &SimulationStatistics::getStationStatistics() const
{
    return stationStatistics;
}

void SimulationStatistics::recordFailedPassengerDispatch()
{
    ++failedPassengerDispatchCount;
}

void SimulationStatistics::recordFailedRebalancingDispatch()
{
    ++failedRebalancingDispatchCount;
}

std::size_t SimulationStatistics::getFailedPassengerDispatchCount() const
{
    return failedPassengerDispatchCount;
}

std::size_t SimulationStatistics::getFailedRebalancingDispatchCount() const
{
    return failedRebalancingDispatchCount;
}

void SimulationStatistics::clear()
{
    engineStatistics.clear();
    stationStatistics.clear();
    failedPassengerDispatchCount = 0;
    failedRebalancingDispatchCount = 0;
}
