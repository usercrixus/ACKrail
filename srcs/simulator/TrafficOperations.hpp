#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficEventManager.hpp"
#include "TrafficResults.hpp"
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

    EventProcessingResult processCurrentEvents(double currentSimulationTimeSeconds);
    std::optional<double> getNextEventTimeSeconds() const;

private:
    Garage &garage;
    TrafficRouteManager routeManager;
    TrafficEventManager eventManager;
};
