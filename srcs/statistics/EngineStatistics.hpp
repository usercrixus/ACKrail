#pragma once

#include "../garage/engine/EnginePad.hpp"
#include "helper/EngineReport.hpp"
#include "helper/NumericSummary.hpp"
#include <cstddef>
#include <unordered_map>

class EngineStatistics
{
public:
    void recordTrip(int engineId, EnginePad::TravelType travelType, double distanceKilometers, double travelTimeSeconds, double waitSeconds);

    const EngineReport *findEngineReport(int engineId) const;

    NumericSummary getTotalDistanceSummary() const;
    NumericSummary getPassengerDistanceSummary() const;
    NumericSummary getRebalancingDistanceSummary() const;
    NumericSummary getTravelTimeSummary() const;
    NumericSummary getWaitTimeSummary() const;

    double getTotalDistanceKilometers() const;
    double getPassengerDistanceKilometers() const;
    double getRebalancingDistanceKilometers() const;
    std::size_t getTripCount() const;
    std::size_t getPassengerTripCount() const;
    std::size_t getRebalancingTripCount() const;
    void clear();

private:
    std::unordered_map<int, EngineReport> reportsByEngineId;
    NumericSummary totalDistanceSummary;
    NumericSummary passengerDistanceSummary;
    NumericSummary rebalancingDistanceSummary;
    NumericSummary travelTimeSummary;
    NumericSummary waitTimeSummary;
    std::size_t passengerTripCount = 0;
    std::size_t rebalancingTripCount = 0;
};
