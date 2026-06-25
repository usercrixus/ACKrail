#include "SimulationStatisticsCollector.hpp"

#include "../garage/Garage.hpp"
#include "../simulator/PassengerDispatcher.hpp"
#include "../simulator/TrafficBalancer.hpp"
#include "../simulator/TrafficManager.hpp"
#include "../topology/Topology.hpp"
#include "helper/CompletedTrip.hpp"

SimulationStatisticsCollector::SimulationStatisticsCollector(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, const PassengerDispatcher &passengerDispatcher, const TrafficBalancer &trafficBalancer)
    : topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      passengerDispatcher(passengerDispatcher),
      trafficBalancer(trafficBalancer)
{
}

void SimulationStatisticsCollector::update(double currentSimulationTimeSeconds)
{
    collectEvents();
    collectStationSnapshots(currentSimulationTimeSeconds);
}

SimulationStatistics &SimulationStatisticsCollector::getStatistics()
{
    return statistics;
}

const SimulationStatistics &SimulationStatisticsCollector::getStatistics() const
{
    return statistics;
}

void SimulationStatisticsCollector::collectEvents()
{
    const auto &routeDispatches = trafficManager.getRouteDispatches();
    while (collectedRouteDispatchCount < routeDispatches.size())
    {
        statistics.getStationStatistics().recordDeparture(routeDispatches[collectedRouteDispatchCount].fromStationId);
        ++collectedRouteDispatchCount;
    }

    const auto &completedTrips = trafficManager.getCompletedTrips();
    while (collectedCompletedTripCount < completedTrips.size())
    {
        const CompletedTrip &trip =
            completedTrips[collectedCompletedTripCount];
        statistics.getEngineStatistics().recordTrip(
            trip.engineId,
            trip.travelType,
            trip.distanceKilometers,
            trip.travelTimeSeconds,
            trip.waitSeconds);
        statistics.getStationStatistics().recordArrival(
            trip.destinationStationId);
        ++collectedCompletedTripCount;
    }
}

void SimulationStatisticsCollector::collectStationSnapshots(
    double currentSimulationTimeSeconds)
{
    if (hasStationSnapshot && currentSimulationTimeSeconds - lastStationSnapshotTimeSeconds < 1.0)
        return;

    for (const Node &station : topology.getNodes())
    {
        const int stationId = station.getId();
        StationStatistics &stationStatistics = statistics.getStationStatistics();
        stationStatistics.setTargetIdleEngines(
            stationId,
            static_cast<double>(
                trafficBalancer.getTargetEngineCountAtStation(stationId)));
        stationStatistics.setWaitingPassengerCount(
            stationId,
            passengerDispatcher.getQueueSizeAtStation(stationId));
        stationStatistics.recordSnapshot(
            stationId,
            static_cast<double>(garage.getIdleEngineCountAtStation(stationId)),
            currentSimulationTimeSeconds);
    }

    lastStationSnapshotTimeSeconds = currentSimulationTimeSeconds;
    hasStationSnapshot = true;
}
