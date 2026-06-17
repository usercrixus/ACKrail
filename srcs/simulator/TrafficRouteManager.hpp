#pragma once

#include "../algorithm/Dijkstra.hpp"
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

    std::optional<ContractedRoute> contractRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType = EnginePad::TravelType::Passenger);

private:
    Topology &topology;
    Dijkstra dijkstra;
};
