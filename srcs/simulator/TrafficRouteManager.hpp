#pragma once

#include "../algorithm/Dijkstra.hpp"
#include "../algorithm/StaticRouteCostMatrix.hpp"
#include "../garage/engine/Engine.hpp"
#include "../topology/Topology.hpp"
#include <optional>
#include <vector>

class TrafficRouteManager
{
public:
    struct RouteDispatch
    {
        int fromStationId;
        EnginePad::TravelType travelType;
    };

    struct ContractedRoute
    {
        Engine *engine;
        const Route *route;
        double departureTimeSeconds;
    };

    TrafficRouteManager(Topology &topology);
    std::optional<ContractedRoute> contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    std::optional<ContractedRoute> contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    double getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const;
    const std::vector<RouteDispatch> &getRouteDispatches() const;

private:
    std::optional<ContractedRoute> contractRouteTiming(Engine &engine, Route *route, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    Topology &topology;
    Dijkstra dijkstra;
    StaticRouteCostMatrix staticRouteCostMatrix;
    std::vector<RouteDispatch> routeDispatches;
};
