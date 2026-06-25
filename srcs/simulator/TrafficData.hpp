#pragma once

#include "TrafficResults.hpp"

#include <cstddef>
#include <vector>

class TrafficData
{
public:
    void record(const PassengerGenerationResult &result);
    void record(const PassengerDispatchResult &result);
    void record(const EventProcessingResult &result);
    void record(const RebalanceResult &result);
    void record(const RouteDispatch &routeDispatch);

    std::size_t getTotalQueueSize() const;
    const std::vector<RouteDispatch> &getRouteDispatches() const;
    const std::vector<CompletedTrip> &getCompletedTrips() const;

private:
    void appendRouteDispatches(const std::vector<RouteDispatch> &newRouteDispatches);

    std::size_t totalQueueSize = 0;
    std::vector<RouteDispatch> routeDispatches;
    std::vector<CompletedTrip> completedTrips;
};
