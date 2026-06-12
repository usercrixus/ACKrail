#include "TrafficManager.hpp"

TrafficManager::TrafficManager(Garage &garage)
    : garage(garage)
{
}

void TrafficManager::advance(double elapsedSeconds)
{
    if (elapsedSeconds > 0.0)
    {
        for (Biplace &engine : garage.getEngines())
            engine.advance(elapsedSeconds);
    }
}
