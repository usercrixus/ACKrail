#pragma once

#include "../garage/Garage.hpp"
#include "TrafficEventManager.hpp"
#include "TrafficRouteManager.hpp"
#include "../topology/Topology.hpp"
#include <optional>
#include <vector>

/**
 * Advances and manages engine simulation.
 */
class TrafficManager
{
public:
    struct RouteDispatch
    {
        int fromStationId;
        EnginePad::TravelType travelType;
    };

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
    bool contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    bool contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    double getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const;

    /** Processes every scheduled event due at the current simulation time. */
    void processCurrentEvents(double currentSimulationTimeSeconds);

    /** @return Time of the next scheduled event, or empty when none exists. */
    std::optional<double> getNextEventTimeSeconds() const;
    const std::vector<RouteDispatch> &getRouteDispatches() const;
    const std::vector<CompletedTrip> &getCompletedTrips() const;

private:
    void recordRouteDispatch(int fromStationId, EnginePad::TravelType travelType);

    Garage &garage;
    TrafficRouteManager routeManager;
    TrafficEventManager eventManager;
    std::vector<RouteDispatch> routeDispatches;
};
