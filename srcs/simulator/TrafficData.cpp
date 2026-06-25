#include "TrafficData.hpp"

#include <algorithm>

void TrafficData::record(const PassengerGenerationResult &result)
{
    totalQueueSize += result.generatedPassengerCount;
}

void TrafficData::record(const PassengerDispatchResult &result)
{
    totalQueueSize -= std::min(totalQueueSize, result.routeDispatches.size());
    appendRouteDispatches(result.routeDispatches);
}

void TrafficData::record(const EventProcessingResult &result)
{
    completedTrips.insert(
        completedTrips.end(),
        result.completedTrips.begin(),
        result.completedTrips.end());
}

void TrafficData::record(const RebalanceResult &result)
{
    appendRouteDispatches(result.routeDispatches);
}

void TrafficData::record(const RouteDispatch &routeDispatch)
{
    routeDispatches.push_back(routeDispatch);
}

std::size_t TrafficData::getTotalQueueSize() const
{
    return totalQueueSize;
}

const std::vector<RouteDispatch> &TrafficData::getRouteDispatches() const
{
    return routeDispatches;
}

const std::vector<CompletedTrip> &TrafficData::getCompletedTrips() const
{
    return completedTrips;
}

void TrafficData::appendRouteDispatches(const std::vector<RouteDispatch> &newRouteDispatches)
{
    routeDispatches.insert(
        routeDispatches.end(),
        newRouteDispatches.begin(),
        newRouteDispatches.end());
}
