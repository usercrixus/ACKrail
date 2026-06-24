#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Topology &topology, Garage &garage, SimulationStatistics &statistics)
    : garage(garage),
      statistics(statistics),
      routeManager(topology),
      eventManager(garage, statistics)
{
}

bool TrafficManager::contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
    {
        if (travelType == EnginePad::TravelType::Passenger)
            statistics.recordFailedPassengerDispatch();
        else if (travelType == EnginePad::TravelType::Rebalancing)
            statistics.recordFailedRebalancingDispatch();
        return false;
    }
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    statistics.getStationStatistics().recordDeparture(fromStationId);
    return true;
}

bool TrafficManager::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const std::optional<TrafficRouteManager::ContractedRoute> contractedRoute = routeManager.contractPrecomputedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
    if (!contractedRoute.has_value())
    {
        if (travelType == EnginePad::TravelType::Passenger)
            statistics.recordFailedPassengerDispatch();
        else if (travelType == EnginePad::TravelType::Rebalancing)
            statistics.recordFailedRebalancingDispatch();
        return false;
    }
    contractedRoute->route->getStations().back()->getController().addExpectedArrival();
    garage.activateEngine(&engine);
    eventManager.scheduleRouteEvents(*contractedRoute->engine, *contractedRoute->route, contractedRoute->departureTimeSeconds);
    statistics.getStationStatistics().recordDeparture(fromStationId);
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
