#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Topology &topology, Garage &garage)
    : topology(topology),
      garage(garage),
      dijkstra(topology.getNodes(), topology.getLinks())
{
}

bool TrafficManager::contractRoute(Engine &engine, int fromStationId, int toStationId)
{
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const double entrySeparationSeconds = calculateEntrySeparationSeconds(engine);
    Route *route = dijkstra.findRoute(fromStationId, toStationId, simulationTimeSeconds, engine.getPad().getMaximumSpeedKilometersPerHour(), entrySeparationSeconds);
    if (route == nullptr)
        return false;
    if (!engine.getPad().startContractedTrajectory(route))
    {
        delete route;
        return false;
    }
    double arrivalTimeSeconds = simulationTimeSeconds;
    for (qsizetype index = 0; index < route->getLinks().size(); ++index)
    {
        const Route::ContractStep &step = route->getContract()[index];
        const double entryTimeSeconds = arrivalTimeSeconds + step.waitSeconds;
        Node *departureNode = route->getStations()[index];
        departureNode->getController().reserveEntry(route->getLinks()[index]->getId(), engine, entryTimeSeconds, entrySeparationSeconds);
        arrivalTimeSeconds = entryTimeSeconds + step.traversalSeconds;
    }
    garage.activateEngine(&engine);
    return true;
}

double TrafficManager::calculateEntrySeparationSeconds(const Engine &engine) const
{
    const double protectedDistanceKilometers = (engine.getLengthMeters() + engine.getSecurityDistanceMeters()) / 1000.0;
    return protectedDistanceKilometers / engine.getPad().getMaximumSpeedKilometersPerHour() * 3600.0;
}

void TrafficManager::advance(double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        for (const auto &[id, engine] : garage.getActiveEngines())
            engine->getPad().advance(elapsedSeconds);
        garage.recycleInactiveEngines();
        simulationTimeSeconds += elapsedSeconds;
    }
}
