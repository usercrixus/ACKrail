#include "TrafficSimulator.hpp"

#include <QTimer>
#include <algorithm>
#include <cmath>
#include <utility>

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager, QObject *parent)
    : QObject(parent),
      trafficManager(trafficManager)
{
}

void TrafficSimulator::start()
{
    if (started)
        return;
    started = true;
    lastUpdateSeconds = 0.0;
    simulationClock.start();
    trafficManager.update(0.0, 0.0);
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
    trafficManager.update(currentSimulationTimeSeconds, currentSimulationTimeSeconds - lastUpdateSeconds);
    if (updateObserver)
        updateObserver(currentSimulationTimeSeconds);
    lastUpdateSeconds = currentSimulationTimeSeconds;
    scheduleNextWork();
}

void TrafficSimulator::scheduleNextWork()
{
    const double currentSimulationTimeSeconds = getCurrentSimulationTimeSeconds();
    const double delaySeconds = trafficManager.getSecondsUntilNextWork(currentSimulationTimeSeconds);
    const int delayMilliseconds = std::max(0, static_cast<int>(std::ceil(delaySeconds * 1000.0)));
    QTimer::singleShot(delayMilliseconds, this, [this]()
                       { processScheduledWork(); });
}
