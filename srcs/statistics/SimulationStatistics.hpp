#pragma once

#include "EngineStatistics.hpp"
#include "StationStatistics.hpp"

class SimulationStatistics
{
public:
    EngineStatistics &getEngineStatistics();
    const EngineStatistics &getEngineStatistics() const;
    StationStatistics &getStationStatistics();
    const StationStatistics &getStationStatistics() const;

    void clear();

private:
    EngineStatistics engineStatistics;
    StationStatistics stationStatistics;
};
