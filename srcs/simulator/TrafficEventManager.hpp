#pragma once

#include "../algorithm/Route.hpp"
#include "../garage/Garage.hpp"
#include "../statistics/helper/CompletedTrip.hpp"
#include <optional>
#include <queue>
#include <vector>

class TrafficEventManager
{
public:
    explicit TrafficEventManager(Garage &garage);

    void scheduleRouteEvents(const Engine &engine, const Route &route, double departureTimeSeconds);
    void processCurrentEvents(double currentSimulationTimeSeconds);
    std::optional<double> getNextEventTimeSeconds() const;
    const std::vector<CompletedTrip> &getCompletedTrips() const;

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

    void processDueEvents(double currentSimulationTimeSeconds);
    void processStepCompletion(const SimulationEvent &event);

    Garage &garage;
    std::priority_queue<SimulationEvent, std::vector<SimulationEvent>, EarlierSimulationEvent> events;
    std::vector<CompletedTrip> completedTrips;
};
