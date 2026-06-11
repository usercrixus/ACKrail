#include "Garage.hpp"

#include "engine/EngineFactory.hpp"
#include <algorithm>

Garage::Garage(std::size_t engineCount)
{
    engines.reserve(engineCount);
    for (std::size_t index = 0; index < engineCount; ++index)
        engines.push_back(EngineFactory::createBiplace());
}

const std::vector<Biplace> &Garage::getEngines() const
{
    return engines;
}

std::vector<Biplace> &Garage::getEngines()
{
    return engines;
}

Biplace *Garage::getIdleEngine()
{
    for (Biplace &engine : engines) {
        if (!engine.isActive())
            return &engine;
    }
    return nullptr;
}

std::size_t Garage::getActiveEngineCount() const
{
    std::size_t count = 0;
    for (const Biplace &engine : engines) {
        if (engine.isActive())
            ++count;
    }
    return count;
}
