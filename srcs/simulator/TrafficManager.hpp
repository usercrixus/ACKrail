#pragma once

#include "TrafficPassenger.hpp"
#include "TrafficBalancer.hpp"
#include "TrafficData.hpp"
#include "TrafficGenerator.hpp"
#include "TrafficOperations.hpp"

/**
 * Owns and coordinates the traffic simulation subsystem.
 */
class TrafficManager
{
public:
    TrafficManager(Topology &topology, Garage &garage);

    void update(double currentSimulationTimeSeconds, double elapsedSeconds);
    double getSecondsUntilNextWork(double currentSimulationTimeSeconds) const;

    bool contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    bool contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType);
    double getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const;
    void processCurrentEvents(double currentSimulationTimeSeconds);
    std::optional<double> getNextEventTimeSeconds() const;
    const TrafficData &getData() const;
    const std::vector<RouteDispatch> &getRouteDispatches() const;
    const std::vector<CompletedTrip> &getCompletedTrips() const;
    const TrafficPassenger &getTrafficPassenger() const;
    const TrafficBalancer &getTrafficBalancer() const;
    TrafficOperations &getTrafficOperations();

private:
    TrafficData data;
    TrafficOperations trafficOperations;
    TrafficPassenger trafficPassenger;
    TrafficGenerator trafficGenerator;
    TrafficBalancer trafficBalancer;
};
