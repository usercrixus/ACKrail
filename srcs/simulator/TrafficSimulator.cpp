#include "TrafficSimulator.hpp"

#include <QTimer>
#include <algorithm>
#include <cmath>
#include <utility>

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager,
                                   TrafficGenerator &trafficGenerator,
                                   PassengerDispatcher &passengerDispatcher,
                                   TrafficBalancer &trafficBalancer,
                                   QObject *parent)
    : QObject(parent),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator),
      passengerDispatcher(passengerDispatcher),
      trafficBalancer(trafficBalancer)
{
}

void TrafficSimulator::start()
{
    if (started)
        return;
    started = true;
    lastGeneratorUpdateSeconds = 0.0;
    lastBalancerUpdateSeconds = 0.0;
    simulationClock.start();
    trafficManager.processCurrentEvents(0.0);
    if (updateObserver)
        updateObserver(0.0);
    scheduleNextWork();
}

double TrafficSimulator::getCurrentSimulationTimeSeconds() const
{
    if (!started)
        return 0.0;
    return static_cast<double>(simulationClock.elapsed()) / 1000.0;
}

void TrafficSimulator::setUpdateObserver(std::function<void(double)> observer)
{
    updateObserver = std::move(observer);
}

void TrafficSimulator::processScheduledWork()
{
    const double currentSimulationTimeSeconds = getCurrentSimulationTimeSeconds();
    trafficManager.processCurrentEvents(currentSimulationTimeSeconds);
    trafficGenerator.tryGenerate(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastGeneratorUpdateSeconds);
    passengerDispatcher.dispatchWaitingPassengers(currentSimulationTimeSeconds);
    trafficBalancer.tryRebalance(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastBalancerUpdateSeconds);
    if (updateObserver)
        updateObserver(currentSimulationTimeSeconds);
    lastGeneratorUpdateSeconds = currentSimulationTimeSeconds;
    lastBalancerUpdateSeconds = currentSimulationTimeSeconds;
    scheduleNextWork();
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
