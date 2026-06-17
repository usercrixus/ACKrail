#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Topology &topology, Garage &garage)
    : topology(topology),
      garage(garage),
      dijkstra(topology.getNodes(), topology.getLinks())
{
}

bool TrafficManager::EarlierSimulationEvent::operator()(const SimulationEvent &left, const SimulationEvent &right) const
{
    return left.timeSeconds > right.timeSeconds;
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
    if (!engine.getPad().startContractedTrajectory(route, simulationTimeSeconds))
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
    scheduleRouteEvents(engine, *route, simulationTimeSeconds);
    return true;
}

void TrafficManager::advance(double elapsedSeconds)
{
    const std::lock_guard lock(garage.getMutex());
    if (elapsedSeconds > 0.0)
    {
        simulationTimeSeconds += elapsedSeconds;
        processDueEvents();
        garage.recycleInactiveEngines();
    }
}

double TrafficManager::getSimulationTimeSeconds() const
{
    const std::lock_guard lock(garage.getMutex());
    return simulationTimeSeconds;
}

void TrafficManager::scheduleRouteEvents(const Engine &engine, const Route &route, double departureTimeSeconds)
{
    double arrivalTimeSeconds = departureTimeSeconds;
    const QVector<Route::ContractStep> &contract = route.getContract();
    for (qsizetype step = 0; step < contract.size(); ++step)
    {
        arrivalTimeSeconds += contract[step].waitSeconds + contract[step].traversalSeconds;
        events.push({arrivalTimeSeconds, engine.getId(), step});
    }
}

void TrafficManager::processDueEvents()
{
    while (!events.empty() && events.top().timeSeconds <= simulationTimeSeconds)
    {
        const SimulationEvent event = events.top();
        events.pop();
        processStepCompletion(event);
    }
}

void TrafficManager::processStepCompletion(const SimulationEvent &event)
{
    const auto enginePosition = garage.getActiveEngines().find(event.engineId);
    if (enginePosition == garage.getActiveEngines().end())
        return;

    Engine &engine = *enginePosition->second;
    EnginePad &pad = engine.getPad();
    const Route *route = pad.getTrajectory();
    if (route == nullptr || event.contractStep >= route->getLinks().size())
        return;

    const QVector<Node *> &stations = route->getStations();
    const QVector<Link *> &links = route->getLinks();
    stations[event.contractStep]->getController().cleanExpiredReservation(
        links[event.contractStep]->getId(),
        event.engineId,
        static_cast<std::size_t>(event.contractStep));

    pad.completeContractStep(event.contractStep, event.timeSeconds);

    const qsizetype nextStep = event.contractStep + 1;
    if (nextStep >= route->getContract().size())
    {
        pad.finishContractedTrajectory(event.timeSeconds);
        return;
    }

    const double entryTimeSeconds = event.timeSeconds + route->getContract()[nextStep].waitSeconds;
    const double exitTimeSeconds = entryTimeSeconds + route->getContract()[nextStep].traversalSeconds;
    pad.enterContractStep(nextStep, entryTimeSeconds, exitTimeSeconds);
}
