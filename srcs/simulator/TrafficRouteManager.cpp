#include "TrafficRouteManager.hpp"

#include <cstddef>

TrafficRouteManager::TrafficRouteManager(Topology &topology)
    : topology(topology),
      dijkstra(topology.getNodes(), topology.getLinks()),
      staticRouteCostMatrix(topology.getNodes(), topology.getLinks())
{
}

std::optional<TrafficRouteManager::ContractedRoute> TrafficRouteManager::contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive())
        return std::nullopt;

    const double entrySeparationSeconds = engine.getEntrySeparationSeconds();
    Route *route = dijkstra.findRoute(fromStationId, toStationId, currentSimulationTimeSeconds, engine.getPad().getMaximumSpeedKilometersPerHour(), entrySeparationSeconds);
    if (route == nullptr)
        return std::nullopt;
    return contractRouteTiming(engine, route, currentSimulationTimeSeconds, travelType);
}

std::optional<TrafficRouteManager::ContractedRoute> TrafficRouteManager::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (engine.getPad().isActive())
        return std::nullopt;

    const double entrySeparationSeconds = engine.getEntrySeparationSeconds();
    Route *route = staticRouteCostMatrix.findRoute(fromStationId, toStationId, currentSimulationTimeSeconds, engine.getPad().getMaximumSpeedKilometersPerHour(), entrySeparationSeconds);
    if (route == nullptr)
        return std::nullopt;
    return contractRouteTiming(engine, route, currentSimulationTimeSeconds, travelType);
}

std::optional<TrafficRouteManager::ContractedRoute> TrafficRouteManager::contractRouteTiming(Engine &engine, Route *route, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    if (!engine.getPad().startContractedTrajectory(route, currentSimulationTimeSeconds, travelType))
    {
        delete route;
        return std::nullopt;
    }
    double arrivalTimeSeconds = currentSimulationTimeSeconds;
    const double entrySeparationSeconds = engine.getEntrySeparationSeconds();
    for (qsizetype index = 0; index < route->getLinks().size(); ++index)
    {
        const Route::ContractStep &step = route->getContract()[index];
        const double entryTimeSeconds = arrivalTimeSeconds + step.waitSeconds;
        Node *departureNode = route->getStations()[index];
        departureNode->getController().reserveEntry(route->getLinks()[index]->getId(), engine.getId(), static_cast<std::size_t>(index), entryTimeSeconds, entrySeparationSeconds);
        arrivalTimeSeconds = entryTimeSeconds + step.traversalSeconds;
    }
    engine.getPad().setTotalTrajectorySeconds(arrivalTimeSeconds - currentSimulationTimeSeconds);
    return ContractedRoute{&engine, engine.getPad().getTrajectory(), currentSimulationTimeSeconds};
}

double TrafficRouteManager::getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const
{
    return staticRouteCostMatrix.getDistanceKilometers(fromStationId, toStationId);
}
