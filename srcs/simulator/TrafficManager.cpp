#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Topology &topology, Garage &garage)
    : garage(garage),
      routeManager(topology),
      eventManager(garage)
{
}

bool TrafficManager::contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
        return false;
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    recordRouteDispatch(fromStationId, travelType);
    return true;
}

bool TrafficManager::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractPrecomputedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
        return false;
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    recordRouteDispatch(fromStationId, travelType);
    return true;
}

double TrafficManager::getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const
{
    return routeManager.getStaticRouteDistanceKilometers(fromStationId, toStationId);
}

void TrafficManager::processCurrentEvents(double currentSimulationTimeSeconds)
{
    eventManager.processCurrentEvents(currentSimulationTimeSeconds);
}

std::optional<double> TrafficManager::getNextEventTimeSeconds() const
{
    return eventManager.getNextEventTimeSeconds();
}

const std::vector<TrafficManager::RouteDispatch> &TrafficManager::getRouteDispatches() const
{
    return routeDispatches;
}

const std::vector<TrafficEventManager::CompletedTrip> &TrafficManager::getCompletedTrips() const
{
    return eventManager.getCompletedTrips();
}

void TrafficManager::recordRouteDispatch(int fromStationId, EnginePad::TravelType travelType)
{
    routeDispatches.push_back({fromStationId, travelType});
}
