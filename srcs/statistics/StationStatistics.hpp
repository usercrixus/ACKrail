#pragma once

#include "helper/NumericSummary.hpp"
#include "helper/StationReport.hpp"
#include "helper/StationsGlobalReport.hpp"
#include <cstddef>
#include <unordered_map>

class StationStatistics
{
public:
    void setTargetIdleEngines(int stationId, double targetIdleEngines);
    void setWaitingPassengerCount(int stationId, std::size_t waitingPassengerCount);
    void recordSnapshot(int stationId, double idleEngines, double simulationTimeSeconds);
    void recordDeparture(int stationId);
    void recordArrival(int stationId);
    const StationReport *findStationReport(int stationId) const;
    std::size_t getTotalMovementCount() const;
    StationsGlobalReport getGlobalReport() const;

    NumericSummary getAverageIdleEngineSummary() const;
    NumericSummary getTargetGapSummary() const;
    double getTotalTimeWithNoIdleEngineSeconds() const;
    double getTotalTimeBelowTargetSeconds() const;
    double getTotalTimeAboveTargetSeconds() const;
    void clear();

private:
    struct MutableStationReport
    {
        StationReport report;
        NumericSummary idleEngineSummary;
        NumericSummary targetGapSummary;
        double lastSnapshotTimeSeconds = 0.0;
        double lastIdleEngines = 0.0;
        bool hasSnapshot = false;
    };

    MutableStationReport &getOrCreateReport(int stationId);
    void closeInterval(MutableStationReport &report, double simulationTimeSeconds);
    StationReport buildReport(const MutableStationReport &report) const;

    std::unordered_map<int, MutableStationReport> reportsByStationId;
    std::size_t totalMovementCount = 0;
    std::size_t totalDepartureCount = 0;
    std::size_t totalArrivalCount = 0;
};
