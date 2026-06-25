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
    trafficOperations.processCurrentEvents(currentSimulationTimeSeconds);
    trafficGenerator.tryGenerate(currentSimulationTimeSeconds, elapsedSeconds);
    trafficPassenger.dispatchWaitingPassengers(currentSimulationTimeSeconds);
    trafficBalancer.tryRebalance(currentSimulationTimeSeconds, elapsedSeconds);
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
    return trafficOperations.contractOptimizedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
}

bool TrafficManager::contractPrecomputedRoute(Engine &engine, int fromStationId, int toStationId, double currentSimulationTimeSeconds, EnginePad::TravelType travelType)
{
    return trafficOperations.contractPrecomputedRoute(engine, fromStationId, toStationId, currentSimulationTimeSeconds, travelType);
}

double TrafficManager::getStaticRouteDistanceKilometers(int fromStationId, int toStationId) const
{
    return trafficOperations.getStaticRouteDistanceKilometers(fromStationId, toStationId);
}

void TrafficManager::processCurrentEvents(double currentSimulationTimeSeconds)
{
    trafficOperations.processCurrentEvents(currentSimulationTimeSeconds);
}

std::optional<double> TrafficManager::getNextEventTimeSeconds() const
{
    return trafficOperations.getNextEventTimeSeconds();
}

const std::vector<TrafficRouteManager::RouteDispatch> &TrafficManager::getRouteDispatches() const
{
    return trafficOperations.getRouteDispatches();
}

const std::vector<CompletedTrip> &TrafficManager::getCompletedTrips() const
{
    return trafficOperations.getCompletedTrips();
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
