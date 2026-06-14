#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficManager.hpp"
#include <random>

class TrafficGenerator
{
public:
    /**
     * Creates a traffic generator for a topology and engine garage.
     *
     * @param topology Network used to generate trips and routes.
     * @param garage Garage that owns engines used for generated trips.
     */
    TrafficGenerator(const Topology &topology, Garage &garage, TrafficManager &trafficManager);

    /**
     * Advances the generation schedule and dispatches traffic when due.
     *
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void tryGenerate(double elapsedSeconds);

private:

    /**
     * Sends between 5 and 20 idle engines on random trips.
     */
    void generate();

    /**
     * Assigns a random reachable trip to an idle engine.
     *
     * @param engine Engine to dispatch.
     * @return true when a route was assigned; otherwise false.
     */
    bool dispatchEngine(Engine &engine);

    const Topology &topology;
    Garage &garage;
    TrafficManager &trafficManager;
    std::mt19937 randomGenerator;
    double secondsUntilDispatch = 1.0;
};
