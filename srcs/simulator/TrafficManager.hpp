#pragma once

#include "../algorithm/Dijkstra.hpp"
#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include <queue>
#include <vector>

/**
 * Advances and manages engine simulation.
 */
class TrafficManager
{
public:
    /**
     * Creates a traffic manager for a garage.
     *
     * @param topology Topology whose nodes control directional link entries.
     * @param garage Garage containing the simulated engines.
     */
    TrafficManager(Topology &topology, Garage &garage);

    /**
     * Finds and commits the earliest-arrival route contract for an engine.
     */
    bool contractRoute(Engine &engine, int fromStationId, int toStationId);

    /**
     * Advances active engines.
     *
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void advance(double elapsedSeconds);

    /** @return Current simulation time in seconds. */
    double getSimulationTimeSeconds() const;

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

    void scheduleRouteEvents(const Engine &engine, const Route &route, double departureTimeSeconds);
    void processDueEvents();
    void processStepCompletion(const SimulationEvent &event);

    Topology &topology;
    Garage &garage;
    Dijkstra dijkstra;
    std::priority_queue<SimulationEvent, std::vector<SimulationEvent>, EarlierSimulationEvent> events;
    double simulationTimeSeconds = 0.0;
};
