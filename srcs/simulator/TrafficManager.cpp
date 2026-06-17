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
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    return true;
}

void TrafficManager::processCurrentEvents(double currentSimulationTimeSeconds)
{
    eventManager.processCurrentEvents(currentSimulationTimeSeconds);
}

std::optional<double> TrafficManager::getNextEventTimeSeconds() const
{
    return eventManager.getNextEventTimeSeconds();
}
