#include "Garage.hpp"

#include <algorithm>

Garage::Garage(std::size_t engineCount)
    : engines(engineCount)
{
}

const std::deque<Biplace> &Garage::getEngines() const
{
    return engines;
}

std::deque<Biplace> &Garage::getEngines()
{
    return engines;
}

Biplace *Garage::getIdleEngine()
{
    for (Biplace &engine : engines) {
        if (!engine.getPad().isActive())
            return &engine;
    }
    return nullptr;
}

std::size_t Garage::getActiveEngineCount() const
{
    std::size_t count = 0;
    for (const Biplace &engine : engines) {
        if (engine.getPad().isActive())
            ++count;
    }
    return count;
}
