#pragma once

#include "NumericSummary.hpp"
#include <cstddef>
#include <unordered_map>

class StationStatistics
{
public:
    struct StationReport
    {
        int stationId = -1;
        double targetIdleEngines = 0.0;
        double currentIdleEngines = 0.0;
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

    struct GlobalReport
    {
        std::size_t stationCount = 0;
        std::size_t stationsWithoutIdleEngine = 0;
        std::size_t departureCount = 0;
        std::size_t arrivalCount = 0;
        double currentIdleEngines = 0.0;
        double targetIdleEngines = 0.0;
        double averageIdleEnginesPerStation = 0.0;
        double averageTargetGap = 0.0;
        double observedStationTimeSeconds = 0.0;
        double timeWithNoIdleEngineSeconds = 0.0;
        double timeBelowTargetSeconds = 0.0;
    };

    void setTargetIdleEngines(int stationId, double targetIdleEngines);
    void recordSnapshot(int stationId, double idleEngines, double simulationTimeSeconds);
    void recordDeparture(int stationId);
    void recordArrival(int stationId);
    const StationReport *findStationReport(int stationId) const;
    std::size_t getTotalMovementCount() const;
    GlobalReport getGlobalReport() const;

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
