#include "TrafficOperations.hpp"

TrafficOperations::TrafficOperations(Topology &topology, Garage &garage)
    : garage(garage),
      routeManager(topology),
      eventManager(garage)
{
}

bool TrafficOperations::contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractOptimizedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
        return false;
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    return true;
}

bool TrafficOperations::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractPrecomputedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
        return false;
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    return true;
}

double TrafficOperations::getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const
{
    return routeManager.getStaticRouteDistanceKilometers(fromStationId, toStationId);
}

void TrafficOperations::processCurrentEvents(double currentSimulationTimeSeconds)
{
    eventManager.processCurrentEvents(currentSimulationTimeSeconds);
}

std::optional<double> TrafficOperations::getNextEventTimeSeconds() const
{
    return eventManager.getNextEventTimeSeconds();
}

const std::vector<TrafficRouteManager::RouteDispatch> &TrafficOperations::getRouteDispatches() const
{
    return routeManager.getRouteDispatches();
}

const std::vector<CompletedTrip> &TrafficOperations::getCompletedTrips() const
{
    return eventManager.getCompletedTrips();
}
