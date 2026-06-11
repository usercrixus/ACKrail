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
    const auto engine = std::find_if(
        engines.begin(),
        engines.end(),
        [](const Biplace &candidate) {
            return !candidate.isActive();
        });
    return engine == engines.end() ? nullptr : &*engine;
}

std::size_t Garage::getActiveEngineCount() const
{
    return static_cast<std::size_t>(std::count_if(
        engines.begin(),
        engines.end(),
        [](const Biplace &engine) {
            return engine.isActive();
        }));
}
