#pragma once

#include "../garage/engine/EnginePad.hpp"
#include "NumericSummary.hpp"
#include <cstddef>
#include <unordered_map>

class EngineStatistics
{
public:
    struct EngineReport
    {
        int engineId = -1;
        std::size_t tripCount = 0;
        double passengerDistanceKilometers = 0.0;
        double rebalancingDistanceKilometers = 0.0;
        double totalDistanceKilometers = 0.0;
        double passengerTimeSeconds = 0.0;
        double rebalancingTimeSeconds = 0.0;
        double totalTravelTimeSeconds = 0.0;
        double totalWaitSeconds = 0.0;
        double averageWaitSeconds = 0.0;
        double maximumWaitSeconds = 0.0;
    };

    void recordTrip(int engineId,
                    EnginePad::TravelType travelType,
                    double distanceKilometers,
                    double travelTimeSeconds,
                    double waitSeconds);

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
