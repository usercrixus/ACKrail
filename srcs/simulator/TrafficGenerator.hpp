#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "TrafficPassenger.hpp"
#include "TrafficResults.hpp"
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
    TrafficGenerator(const Topology &topology, Garage &garage, TrafficPassenger &trafficPassenger);

    /** Assigns initial parking stations to engines that do not have one. */
    void initialize();

    /**
     * Advances the generation schedule and dispatches traffic when due.
     *
     * @param currentSimulationTimeSeconds Current simulation time in seconds.
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    PassengerGenerationResult tryGenerate(double currentSimulationTimeSeconds, double elapsedSeconds);

    /** @return Seconds before the next generated traffic dispatch. */
    double getSecondsUntilDispatch() const;

private:

    /**
     * Sends idle engines on random trips.
     */
    PassengerGenerationResult generate(double currentSimulationTimeSeconds);

    /** Adds one weighted passenger request to its departure-station queue. */
    bool generatePassengerRequest(double currentSimulationTimeSeconds);

    /** Parks idle engines evenly across topology stations. */
    void initializeEngineParkingStations();

    const Topology &topology;
    Garage &garage;
    TrafficPassenger &trafficPassenger;
    std::mt19937 randomGenerator;
    std::discrete_distribution<int> arrivalStationDistribution;
    std::discrete_distribution<int> departureStationDistribution;
    double secondsUntilDispatch = 0.10;
    bool initialized = false;
};
