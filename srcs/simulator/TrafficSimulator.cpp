#include "TrafficSimulator.hpp"

#include <QTimer>
#include <algorithm>
#include <cmath>

TrafficSimulator::TrafficSimulator(const Topology &topology,
                                   const Garage &garage,
                                   TrafficManager &trafficManager,
                                   TrafficGenerator &trafficGenerator,
                                   PassengerDispatcher &passengerDispatcher,
                                   TrafficBalancer &trafficBalancer,
                                   SimulationStatistics &statistics,
                                   QObject *parent)
    : QObject(parent),
      topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator),
      passengerDispatcher(passengerDispatcher),
      trafficBalancer(trafficBalancer),
      statistics(statistics)
{
}

void TrafficSimulator::start()
{
    if (started)
        return;
    started = true;
    lastGeneratorUpdateSeconds = 0.0;
    lastBalancerUpdateSeconds = 0.0;
    lastStationStatisticsUpdateSeconds = 0.0;
    simulationClock.start();
    trafficManager.processCurrentEvents(0.0);
    recordStationSnapshots(0.0);
    scheduleNextWork();
}

double TrafficSimulator::getCurrentSimulationTimeSeconds() const
{
    if (!started)
        return 0.0;
    return static_cast<double>(simulationClock.elapsed()) / 1000.0;
}

void TrafficSimulator::processScheduledWork()
{
    const double currentSimulationTimeSeconds = getCurrentSimulationTimeSeconds();
    trafficManager.processCurrentEvents(currentSimulationTimeSeconds);
    trafficGenerator.tryGenerate(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastGeneratorUpdateSeconds);
    passengerDispatcher.dispatchWaitingPassengers(currentSimulationTimeSeconds);
    trafficBalancer.tryRebalance(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastBalancerUpdateSeconds);
    recordStationSnapshots(currentSimulationTimeSeconds);
    lastGeneratorUpdateSeconds = currentSimulationTimeSeconds;
    lastBalancerUpdateSeconds = currentSimulationTimeSeconds;
    scheduleNextWork();
}

void TrafficSimulator::recordStationSnapshots(double currentSimulationTimeSeconds)
{
    if (currentSimulationTimeSeconds - lastStationStatisticsUpdateSeconds < 1.0 && currentSimulationTimeSeconds > 0.0)
        return;

    for (const Node &station : topology.getNodes())
    {
        const auto stationPool = garage.getIdleEnginesByStation().find(station.getId());
        const std::size_t idleEngineCount = stationPool == garage.getIdleEnginesByStation().end() ? 0 : stationPool->second.size();
        statistics.getStationStatistics().setTargetIdleEngines(station.getId(), static_cast<double>(trafficBalancer.getTargetEngineCountAtStation(station.getId())));
        statistics.getStationStatistics().setWaitingPassengerCount(
            station.getId(),
            passengerDispatcher.getQueueSizeAtStation(station.getId()));
        statistics.getStationStatistics().recordSnapshot(station.getId(), static_cast<double>(idleEngineCount), currentSimulationTimeSeconds);
    }
    lastStationStatisticsUpdateSeconds = currentSimulationTimeSeconds;
}

void TrafficSimulator::scheduleNextWork()
{
    const double currentSimulationTimeSeconds = getCurrentSimulationTimeSeconds();
    double delaySeconds = std::min(trafficGenerator.getSecondsUntilDispatch(), trafficBalancer.getSecondsUntilRebalance());
    const std::optional<double> nextEventTimeSeconds = trafficManager.getNextEventTimeSeconds();
    if (nextEventTimeSeconds.has_value())
        delaySeconds = std::min(delaySeconds, std::max(0.0, *nextEventTimeSeconds - currentSimulationTimeSeconds));
    const int delayMilliseconds = std::max(0, static_cast<int>(std::ceil(delaySeconds * 1000.0)));
    QTimer::singleShot(delayMilliseconds, this, [this]()
                       { processScheduledWork(); });
}
