#include "StationStatistics.hpp"

#include <algorithm>
#include <cmath>

void StationStatistics::setTargetIdleEngines(int stationId, double targetIdleEngines)
{
    MutableStationReport &report = getOrCreateReport(stationId);
    report.report.targetIdleEngines = std::max(0.0, targetIdleEngines);
}

void StationStatistics::setWaitingPassengerCount(int stationId, std::size_t waitingPassengerCount)
{
    MutableStationReport &report = getOrCreateReport(stationId);
    report.report.currentWaitingPassengers = waitingPassengerCount;
}

void StationStatistics::recordSnapshot(int stationId, double idleEngines, double simulationTimeSeconds)
{
    MutableStationReport &report = getOrCreateReport(stationId);
    closeInterval(report, simulationTimeSeconds);

    const double clampedIdleEngines = std::max(0.0, idleEngines);
    report.report.currentIdleEngines = clampedIdleEngines;
    report.idleEngineSummary.add(clampedIdleEngines);
    report.targetGapSummary.add(std::abs(clampedIdleEngines - report.report.targetIdleEngines));
    report.lastIdleEngines = clampedIdleEngines;
    report.lastSnapshotTimeSeconds = simulationTimeSeconds;
    report.hasSnapshot = true;
    report.report = buildReport(report);
}

void StationStatistics::recordDeparture(int stationId)
{
    MutableStationReport &report = getOrCreateReport(stationId);
    ++report.report.departureCount;
    ++totalMovementCount;
    ++totalDepartureCount;
}

void StationStatistics::recordArrival(int stationId)
{
    MutableStationReport &report = getOrCreateReport(stationId);
    ++report.report.arrivalCount;
    ++totalMovementCount;
    ++totalArrivalCount;
}

const StationReport *StationStatistics::findStationReport(int stationId) const
{
    const auto report = reportsByStationId.find(stationId);
    if (report == reportsByStationId.end())
        return nullptr;
    return &report->second.report;
}

std::size_t StationStatistics::getTotalMovementCount() const
{
    return totalMovementCount;
}

StationsGlobalReport StationStatistics::getGlobalReport() const
{
    StationsGlobalReport global;
    global.stationCount = reportsByStationId.size();
    global.departureCount = totalDepartureCount;
    global.arrivalCount = totalArrivalCount;

    for (const auto &entry : reportsByStationId)
    {
        const StationReport &station = entry.second.report;
        global.currentIdleEngines += station.currentIdleEngines;
        global.currentWaitingPassengers += station.currentWaitingPassengers;
        global.targetIdleEngines += station.targetIdleEngines;
        global.averageIdleEnginesPerStation += station.averageIdleEngines;
        global.averageTargetGap += station.averageTargetGap;
        global.observedStationTimeSeconds += station.observedTimeSeconds;
        global.timeWithNoIdleEngineSeconds += station.timeWithNoIdleEngineSeconds;
        global.timeBelowTargetSeconds += station.timeBelowTargetSeconds;
        if (station.currentIdleEngines <= 0.0)
            ++global.stationsWithoutIdleEngine;
    }

    if (global.stationCount > 0)
    {
        const double stationCount = static_cast<double>(global.stationCount);
        global.averageIdleEnginesPerStation /= stationCount;
        global.averageTargetGap /= stationCount;
    }
    return global;
}

NumericSummary StationStatistics::getAverageIdleEngineSummary() const
{
    NumericSummary summary;
    for (const auto &entry : reportsByStationId)
        summary.add(entry.second.report.averageIdleEngines);
    return summary;
}

NumericSummary StationStatistics::getTargetGapSummary() const
{
    NumericSummary summary;
    for (const auto &entry : reportsByStationId)
        summary.add(entry.second.report.averageTargetGap);
    return summary;
}

double StationStatistics::getTotalTimeWithNoIdleEngineSeconds() const
{
    double total = 0.0;
    for (const auto &entry : reportsByStationId)
        total += entry.second.report.timeWithNoIdleEngineSeconds;
    return total;
}

double StationStatistics::getTotalTimeBelowTargetSeconds() const
{
    double total = 0.0;
    for (const auto &entry : reportsByStationId)
        total += entry.second.report.timeBelowTargetSeconds;
    return total;
}

double StationStatistics::getTotalTimeAboveTargetSeconds() const
{
    double total = 0.0;
    for (const auto &entry : reportsByStationId)
        total += entry.second.report.timeAboveTargetSeconds;
    return total;
}

void StationStatistics::clear()
{
    reportsByStationId.clear();
    totalMovementCount = 0;
    totalDepartureCount = 0;
    totalArrivalCount = 0;
}

StationStatistics::MutableStationReport &StationStatistics::getOrCreateReport(int stationId)
{
    MutableStationReport &report = reportsByStationId[stationId];
    report.report.stationId = stationId;
    return report;
}

void StationStatistics::closeInterval(MutableStationReport &report, double simulationTimeSeconds)
{
    if (!report.hasSnapshot || simulationTimeSeconds <= report.lastSnapshotTimeSeconds)
        return;

    const double elapsedSeconds = simulationTimeSeconds - report.lastSnapshotTimeSeconds;
    report.report.observedTimeSeconds += elapsedSeconds;
    if (report.lastIdleEngines <= 0.0)
        report.report.timeWithNoIdleEngineSeconds += elapsedSeconds;
    if (report.lastIdleEngines < report.report.targetIdleEngines)
        report.report.timeBelowTargetSeconds += elapsedSeconds;
    else if (report.lastIdleEngines > report.report.targetIdleEngines)
        report.report.timeAboveTargetSeconds += elapsedSeconds;
}

StationReport StationStatistics::buildReport(const MutableStationReport &report) const
{
    StationReport stationReport = report.report;
    stationReport.averageIdleEngines = report.idleEngineSummary.getAverage();
    stationReport.minimumIdleEngines = report.idleEngineSummary.getMinimum();
    stationReport.maximumIdleEngines = report.idleEngineSummary.getMaximum();
    stationReport.averageTargetGap = report.targetGapSummary.getAverage();
    return stationReport;
}
