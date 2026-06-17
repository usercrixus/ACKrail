#include "TrafficSimulator.hpp"

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator)
    : trafficManager(trafficManager),
      trafficGenerator(trafficGenerator)
{
}

void TrafficSimulator::advance(double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        trafficManager.advance(elapsedSeconds);
        trafficGenerator.tryGenerate(elapsedSeconds);
    }
}
