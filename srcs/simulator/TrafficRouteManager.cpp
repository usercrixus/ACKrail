#include "TrafficRouteManager.hpp"

TrafficRouteManager::TrafficRouteManager(Topology &topology)
    : topology(topology),
      dijkstra(topology.getNodes(), topology.getLinks())
{
}

std::optional<TrafficRouteManager::ContractedRoute> TrafficRouteManager::contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds)
{
    if (engine.getPad().isActive())
        return std::nullopt;

    const double entrySeparationSeconds = engine.getEntrySeparationSeconds();
    Route *route = dijkstra.findRoute(fromStationId, toStationId, currentSimulationTimeSeconds, engine.getPad().getMaximumSpeedKilometersPerHour(), entrySeparationSeconds);
    if (route == nullptr)
        return std::nullopt;
    if (!engine.getPad().startContractedTrajectory(route, currentSimulationTimeSeconds))
    {
        delete route;
        return std::nullopt;
    }

    double arrivalTimeSeconds = currentSimulationTimeSeconds;
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
