#include "TrafficEventManager.hpp"

TrafficEventManager::TrafficEventManager(Garage &garage)
    : garage(garage)
{
}

bool TrafficEventManager::EarlierSimulationEvent::operator()(const SimulationEvent &left, const SimulationEvent &right) const
{
    return left.timeSeconds > right.timeSeconds;
}

void TrafficEventManager::scheduleRouteEvents(const Engine &engine, const Route &route, double departureTimeSeconds)
{
    double arrivalTimeSeconds = departureTimeSeconds;
    const QVector<Route::ContractStep> &contract = route.getContract();
    for (qsizetype step = 0; step < contract.size(); ++step)
    {
        arrivalTimeSeconds += contract[step].waitSeconds + contract[step].traversalSeconds;
        events.push({arrivalTimeSeconds, engine.getId(), step});
    }
}

EventProcessingResult TrafficEventManager::processCurrentEvents(double currentSimulationTimeSeconds)
{
    EventProcessingResult result = processDueEvents(currentSimulationTimeSeconds);
    garage.recycleInactiveEngines();
    return result;
}

std::optional<double> TrafficEventManager::getNextEventTimeSeconds() const
{
    if (events.empty())
        return std::nullopt;
    return events.top().timeSeconds;
}

EventProcessingResult TrafficEventManager::processDueEvents(double currentSimulationTimeSeconds)
{
    EventProcessingResult result;
    while (!events.empty() && events.top().timeSeconds <= currentSimulationTimeSeconds)
    {
        const SimulationEvent event = events.top();
        events.pop();
        const std::optional<CompletedTrip> completedTrip = processStepCompletion(event);
        if (completedTrip.has_value())
            result.completedTrips.push_back(*completedTrip);
    }
    return result;
}

std::optional<CompletedTrip> TrafficEventManager::processStepCompletion(const SimulationEvent &event)
{
    Engine *const *enginePosition = garage.getActiveEngines().find(event.engineId);
    if (enginePosition == nullptr)
        return std::nullopt;

    Engine &engine = **enginePosition;
    EnginePad &pad = engine.getPad();
    const Route *route = pad.getTrajectory();
    if (route == nullptr || event.contractStep >= route->getLinks().size())
        return std::nullopt;

    const QVector<Node *> &stations = route->getStations();
    const QVector<Link *> &links = route->getLinks();
    stations[event.contractStep]->getController().cleanExpiredReservation(links[event.contractStep]->getId(), event.engineId, static_cast<std::size_t>(event.contractStep));
    pad.completeContractStep(event.contractStep, event.timeSeconds);

    const qsizetype nextStep = event.contractStep + 1;
    if (nextStep >= route->getContract().size())
    {
        double waitSeconds = 0.0;
        for (const Route::ContractStep &step : route->getContract())
            waitSeconds += step.waitSeconds;
        const CompletedTrip completedTrip{
            engine.getId(),
            stations.back()->getId(),
            pad.getTravelType(),
            route->getTotalDistanceKilometers(),
            pad.getTotalTrajectorySeconds(),
            waitSeconds};
        stations.back()->getController().removeExpectedArrival();
        pad.finishContractedTrajectory(event.timeSeconds);
        return completedTrip;
    }

    const double entryTimeSeconds = event.timeSeconds + route->getContract()[nextStep].waitSeconds;
    const double exitTimeSeconds = entryTimeSeconds + route->getContract()[nextStep].traversalSeconds;
    pad.enterContractStep(nextStep, entryTimeSeconds, exitTimeSeconds);
    return std::nullopt;
}
