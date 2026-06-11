#pragma once

#include "../algorithm/RouteFinder.hpp"
#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
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
    TrafficGenerator(const Topology &topology, Garage &garage);

    /**
     * Advances active engines and dispatches new traffic.
     *
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void advance(double elapsedSeconds);

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

    /**
     * Converts a station route into topology links.
     *
     * @param route Ordered station route returned by RouteFinder.
     * @return Ordered links to traverse, or an empty vector if conversion fails.
     */
    QVector<const Link *> linksForRoute(const RouteFinder::Route &route) const;

    const Topology &topology;
    Garage &garage;
    RouteFinder routeFinder;
    std::mt19937 randomGenerator;
    double secondsUntilDispatch = 1.0;
};
