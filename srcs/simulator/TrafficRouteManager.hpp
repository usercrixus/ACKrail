#pragma once

#include "../algorithm/Dijkstra.hpp"
#include "../algorithm/StaticRouteCostMatrix.hpp"
#include "../garage/engine/Engine.hpp"
#include "../topology/Topology.hpp"
#include <optional>

class TrafficRouteManager
{
public:
    struct ContractedRoute
    {
        Engine *engine;
        const Route *route;
        double departureTimeSeconds;
    };

    TrafficRouteManager(Topology &topology);

    std::optional<ContractedRoute> contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    std::optional<ContractedRoute> contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    double getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const;

private:
    std::optional<ContractedRoute> contractRouteTiming(Engine &engine, Route *route, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);

    Topology &topology;
    Dijkstra dijkstra;
    StaticRouteCostMatrix staticRouteCostMatrix;
};
