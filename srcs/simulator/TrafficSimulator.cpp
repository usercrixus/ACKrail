#include "TrafficSimulator.hpp"

#include <QTimer>
#include <algorithm>
#include <cmath>

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, TrafficBalancer &trafficBalancer, QObject *parent)
    : QObject(parent),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator),
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
    trafficGenerator.tryGenerate(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastGeneratorUpdateSeconds);
    trafficBalancer.tryRebalance(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastBalancerUpdateSeconds);
    trafficManager.processCurrentEvents(currentSimulationTimeSeconds);
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
