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

void SimulationStatistics::clear()
{
    engineStatistics.clear();
    stationStatistics.clear();
}
