#pragma once

#include "../algorithm/Route.hpp"
#include "../garage/Garage.hpp"
#include "../statistics/helper/CompletedTrip.hpp"
#include "TrafficResults.hpp"
#include <optional>
#include <queue>
#include <vector>

class TrafficEventManager
{
public:
    explicit TrafficEventManager(Garage &garage);
    void scheduleRouteEvents(const Engine &engine, const Route &route, double departureTimeSeconds);
    EventProcessingResult processCurrentEvents(double currentSimulationTimeSeconds);
    std::optional<double> getNextEventTimeSeconds() const;

private:
    struct SimulationEvent
    {
        double timeSeconds;
        int engineId;
        qsizetype contractStep;
    };

    struct EarlierSimulationEvent
    {
        bool operator()(const SimulationEvent &left, const SimulationEvent &right) const;
    };

    EventProcessingResult processDueEvents(double currentSimulationTimeSeconds);
    std::optional<CompletedTrip> processStepCompletion(const SimulationEvent &event);

    Garage &garage;
    std::priority_queue<SimulationEvent, std::vector<SimulationEvent>, EarlierSimulationEvent> events;
};
