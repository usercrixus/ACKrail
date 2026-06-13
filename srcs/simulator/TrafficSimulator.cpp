#include "TrafficSimulator.hpp"

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, QObject *parent)
    : QObject(parent),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator)
{
    timer.setInterval(16);
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, &QTimer::timeout, this, &TrafficSimulator::advance);
}

void TrafficSimulator::start()
{
    if (!timer.isActive())
    {
        clock.start();
        timer.start();
    }
}

void TrafficSimulator::stop()
{
    timer.stop();
    clock.invalidate();
}

void TrafficSimulator::advance()
{
    const qint64 elapsedMilliseconds = clock.restart();
    if (elapsedMilliseconds > 0)
    {
        const double elapsedSeconds = static_cast<double>(elapsedMilliseconds) / 1000.0;
        trafficManager.advance(elapsedSeconds);
        trafficGenerator.tryGenerate(elapsedSeconds);
        emit advanced();
    }
}
