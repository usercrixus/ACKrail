#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Topology &topology, Garage &garage)
    : topology(topology),
      garage(garage),
      dijkstra(topology.getNodes(), topology.getLinks())
{
}

bool TrafficManager::contractRoute(Engine &engine, int fromStationId, int toStationId)
{
    const std::lock_guard lock(garage.getMutex());
    if (engine.getPad().isActive() || !garage.isIdleEngine(engine))
        return false;
    const double entrySeparationSeconds = engine.getEntrySeparationSeconds();
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
        departureNode->getController().reserveEntry(
            route->getLinks()[index]->getId(),
            engine.getId(),
            static_cast<std::size_t>(index),
            entryTimeSeconds,
            entrySeparationSeconds);
        arrivalTimeSeconds = entryTimeSeconds + step.traversalSeconds;
    }
    engine.getPad().setTotalTrajectorySeconds(arrivalTimeSeconds - simulationTimeSeconds);
    garage.activateEngine(&engine);
    return true;
}

void TrafficManager::advance(double elapsedSeconds)
{
    const std::lock_guard lock(garage.getMutex());
    if (elapsedSeconds > 0.0)
    {
        for (const auto &[id, engine] : garage.getActiveEngines())
        {
            EnginePad &pad = engine->getPad();
            const qsizetype currentContractStep = pad.getCurrentContractStep();
            const QVector<Node *> stations = pad.getTrajectory()->getStations();
            const QVector<Link *> links = pad.getTrajectory()->getLinks();
            pad.advance(elapsedSeconds);
            const qsizetype advancedContractStep = pad.getCurrentContractStep();
            for (qsizetype step = currentContractStep; step < advancedContractStep; ++step)
                stations[step]->getController().cleanExpiredReservation(links[step]->getId(), id, static_cast<std::size_t>(step));
        }
        garage.recycleInactiveEngines();
        simulationTimeSeconds += elapsedSeconds;
    }
}
