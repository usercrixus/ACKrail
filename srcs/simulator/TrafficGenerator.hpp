#pragma once

#include "../algorithm/RouteFinder.hpp"
#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include <optional>
#include <random>

class TrafficGenerator
{
public:
    struct EnginePosition
    {
        const Node *fromNode;
        const Node *toNode;
        double linkProgress;
    };

    /**
     * Creates a traffic generator for a topology and engine garage.
     *
     * @param topology Network used to generate trips and routes.
     * @param garage Garage that owns engines used for generated trips.
     */
    TrafficGenerator(const Topology &topology, Garage &garage);

    /**
     * Advances active engines and dispatches new traffic.
     *
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void advance(double elapsedSeconds);

    /**
     * Calculates an engine position along its current trajectory.
     *
     * @param engine Engine whose position should be calculated.
     * @return Current route segment and progress, or an empty optional.
     */
    std::optional<EnginePosition> getEnginePosition(const Engine &engine) const;

private:
    /**
     * Sends between 5 and 20 idle engines on random trips.
     */
    void dispatchRandomEngines();

    /**
     * Assigns a random reachable trip to an idle engine.
     *
     * @param engine Engine to dispatch.
     * @return true when a route was assigned; otherwise false.
     */
    bool dispatchEngine(Biplace &engine);

    const Topology &topology;
    Garage &garage;
    RouteFinder routeFinder;
    std::mt19937 randomGenerator;
    double secondsUntilDispatch = 1.0;
};
