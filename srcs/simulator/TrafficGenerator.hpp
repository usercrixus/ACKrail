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
     * @param currentSimulationTimeSeconds Current simulation time in seconds.
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void tryGenerate(double currentSimulationTimeSeconds, double elapsedSeconds);

    /** @return Seconds before the next generated traffic dispatch. */
    double getSecondsUntilDispatch() const;

private:

    /**
     * Sends idle engines on random trips.
     */
    void generate(double currentSimulationTimeSeconds);

    /**
     * Assigns a weighted passenger trip to an idle engine parked at the
     * sampled departure station.
     *
     * @return true when a route was assigned; otherwise false.
     */
    bool dispatchPassengerRoute(double currentSimulationTimeSeconds);

    /** Parks idle engines evenly across topology stations. */
    void initializeEngineParkingStations();

    const Topology &topology;
    Garage &garage;
    TrafficManager &trafficManager;
    std::mt19937 randomGenerator;
    std::discrete_distribution<int> arrivalStationDistribution;
    std::discrete_distribution<int> departureStationDistribution;
    double secondsUntilDispatch = 0.10;
};
