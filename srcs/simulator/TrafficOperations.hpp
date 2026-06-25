#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficEventManager.hpp"
#include "TrafficRouteManager.hpp"
#include <optional>

/**
 * Contracts routes and processes the resulting engine events.
 */
class TrafficOperations
{
public:
    TrafficOperations(Topology &topology, Garage &garage);

    bool contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    bool contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    double getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const;

    void processCurrentEvents(double currentSimulationTimeSeconds);
    std::optional<double> getNextEventTimeSeconds() const;
    const std::vector<TrafficRouteManager::RouteDispatch> &getRouteDispatches() const;
    const std::vector<CompletedTrip> &getCompletedTrips() const;

private:
    Garage &garage;
    TrafficRouteManager routeManager;
    TrafficEventManager eventManager;
};
