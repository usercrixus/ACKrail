#pragma once

#include "../algorithm/Dijkstra.hpp"
#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"

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

private:
    double calculateEntrySeparationSeconds(const Engine &engine) const;

    Topology &topology;
    Garage &garage;
    Dijkstra dijkstra;
    double simulationTimeSeconds = 0.0;
};
