#include "EngineStatistics.hpp"

#include <algorithm>

void EngineStatistics::recordTrip(int engineId, EnginePad::TravelType travelType, double distanceKilometers, double travelTimeSeconds, double waitSeconds)
{
    EngineReport &report = reportsByEngineId[engineId];
    report.engineId = engineId;
    ++report.tripCount;
    report.totalDistanceKilometers += distanceKilometers;
    report.totalTravelTimeSeconds += travelTimeSeconds;
    report.totalWaitSeconds += waitSeconds;
    report.maximumWaitSeconds = std::max(report.maximumWaitSeconds, waitSeconds);
    totalDistanceSummary.add(distanceKilometers);
    travelTimeSummary.add(travelTimeSeconds);
    waitTimeSummary.add(waitSeconds);
    if (travelType == EnginePad::TravelType::Passenger)
    {
        ++passengerTripCount;
        report.passengerDistanceKilometers += distanceKilometers;
        report.passengerTimeSeconds += travelTimeSeconds;
        passengerDistanceSummary.add(distanceKilometers);
    }
    else if (travelType == EnginePad::TravelType::Rebalancing)
    {
        ++rebalancingTripCount;
        report.rebalancingDistanceKilometers += distanceKilometers;
        report.rebalancingTimeSeconds += travelTimeSeconds;
        rebalancingDistanceSummary.add(distanceKilometers);
    }
    report.averageWaitSeconds = report.totalWaitSeconds / static_cast<double>(report.tripCount);
}

const EngineReport *EngineStatistics::findEngineReport(int engineId) const
{
    const auto report = reportsByEngineId.find(engineId);
    return report == reportsByEngineId.end() ? nullptr : &report->second;
}

NumericSummary EngineStatistics::getTotalDistanceSummary() const
{
    return totalDistanceSummary;
}

NumericSummary EngineStatistics::getPassengerDistanceSummary() const
{
    return passengerDistanceSummary;
}

NumericSummary EngineStatistics::getRebalancingDistanceSummary() const
{
    return rebalancingDistanceSummary;
}

NumericSummary EngineStatistics::getTravelTimeSummary() const
{
    return travelTimeSummary;
}

NumericSummary EngineStatistics::getWaitTimeSummary() const
{
    return waitTimeSummary;
}

double EngineStatistics::getTotalDistanceKilometers() const
{
    return totalDistanceSummary.getTotal();
}

double EngineStatistics::getPassengerDistanceKilometers() const
{
    return passengerDistanceSummary.getTotal();
}

double EngineStatistics::getRebalancingDistanceKilometers() const
{
    return rebalancingDistanceSummary.getTotal();
}

std::size_t EngineStatistics::getTripCount() const
{
    return totalDistanceSummary.getCount();
}

std::size_t EngineStatistics::getPassengerTripCount() const
{
    return passengerTripCount;
}

std::size_t EngineStatistics::getRebalancingTripCount() const
{
    return rebalancingTripCount;
}

void EngineStatistics::clear()
{
    reportsByEngineId.clear();
    totalDistanceSummary.clear();
    passengerDistanceSummary.clear();
    rebalancingDistanceSummary.clear();
    travelTimeSummary.clear();
    waitTimeSummary.clear();
    passengerTripCount = 0;
    rebalancingTripCount = 0;
}
