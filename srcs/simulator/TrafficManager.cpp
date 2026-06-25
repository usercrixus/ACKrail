#include "TrafficManager.hpp"

#include <algorithm>

TrafficManager::TrafficManager(Topology &topology, Garage &garage)
    : trafficOperations(topology, garage),
      trafficPassenger(garage, trafficOperations),
      trafficGenerator(topology, garage, trafficPassenger),
      trafficBalancer(topology, garage, trafficOperations, trafficPassenger)
{
}

void TrafficManager::update(double currentSimulationTimeSeconds, double elapsedSeconds)
{
    trafficGenerator.initialize();
    data.record(trafficOperations.processCurrentEvents(currentSimulationTimeSeconds));
    data.record(trafficGenerator.tryGenerate(currentSimulationTimeSeconds, elapsedSeconds));
    data.record(trafficPassenger.dispatchWaitingPassengers(currentSimulationTimeSeconds));
    data.record(trafficBalancer.tryRebalance(currentSimulationTimeSeconds, elapsedSeconds));
}

double TrafficManager::getSecondsUntilNextWork(double currentSimulationTimeSeconds) const
{
    double delaySeconds = std::min(trafficGenerator.getSecondsUntilDispatch(), trafficBalancer.getSecondsUntilRebalance());
    const std::optional<double> nextEventTimeSeconds = trafficOperations.getNextEventTimeSeconds();
    if (nextEventTimeSeconds.has_value())
        delaySeconds = std::min(delaySeconds, std::max(0.0, *nextEventTimeSeconds - currentSimulationTimeSeconds));
    return delaySeconds;
}

bool TrafficManager::contractOptimizedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    const bool contracted = trafficOperations.contractOptimizedRoute(
        engine,
        fromStationId,
        toStationId,
        currentSimulationTimeSeconds,
        travelType);
    if (contracted)
        data.record(RouteDispatch{fromStationId, travelType});
    return contracted;
}

bool TrafficManager::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    const bool contracted = trafficOperations.contractPrecomputedRoute(
        engine,
        fromStationId,
        toStationId,
        currentSimulationTimeSeconds,
        travelType);
    if (contracted)
        data.record(RouteDispatch{fromStationId, travelType});
    return contracted;
}

double TrafficManager::getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const
{
    return trafficOperations.getStaticRouteDistanceKilometers(fromStationId, toStationId);
}

void TrafficManager::processCurrentEvents(double currentSimulationTimeSeconds)
{
    data.record(trafficOperations.processCurrentEvents(currentSimulationTimeSeconds));
}

std::optional<double> TrafficManager::getNextEventTimeSeconds() const
{
    return trafficOperations.getNextEventTimeSeconds();
}

const TrafficData &TrafficManager::getData() const
{
    return data;
}

const std::vector<RouteDispatch> &TrafficManager::getRouteDispatches() const
{
    return data.getRouteDispatches();
}

const std::vector<CompletedTrip> &TrafficManager::getCompletedTrips() const
{
    return data.getCompletedTrips();
}

const TrafficPassenger &TrafficManager::getTrafficPassenger() const
{
    return trafficPassenger;
}

const TrafficBalancer &TrafficManager::getTrafficBalancer() const
{
    return trafficBalancer;
}

TrafficOperations &TrafficManager::getTrafficOperations()
{
    return trafficOperations;
}
