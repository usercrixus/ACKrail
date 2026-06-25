#pragma once

#include "../garage/engine/EnginePad.hpp"
#include "../statistics/helper/CompletedTrip.hpp"

#include <cstddef>
#include <vector>

struct RouteDispatch
{
    int fromStationId;
    EnginePad::TravelType travelType;
};

struct PassengerGenerationResult
{
    std::size_t generatedPassengerCount = 0;
};

struct PassengerDispatchResult
{
    std::vector<RouteDispatch> routeDispatches;
};

struct EventProcessingResult
{
    std::vector<CompletedTrip> completedTrips;
};

struct RebalanceResult
{
    std::vector<RouteDispatch> routeDispatches;
};
