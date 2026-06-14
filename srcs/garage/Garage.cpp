#include "Garage.hpp"

Garage::Garage(std::size_t engineCount)
{
    idleEngines.reserve(engineCount);
    activeEngines.reserve(engineCount);
    for (std::size_t index = 0; index < engineCount; ++index)
    {
        const int id = static_cast<int>(index);
        idleEngines.emplace(id, new Biplace(id));
    }
}

Garage::~Garage()
{
    const std::lock_guard lock(mutex);
    for (const auto &[id, engine] : idleEngines)
        delete engine;
    for (const auto &[id, engine] : activeEngines)
        delete engine;
}

const std::unordered_map<int, Engine *> &Garage::getIdleEngines() const
{
    return idleEngines;
}

const std::unordered_map<int, Engine *> &Garage::getActiveEngines() const
{
    return activeEngines;
}

Engine *Garage::getIdleEngine()
{
    const std::lock_guard lock(mutex);
    return idleEngines.empty() ? nullptr : idleEngines.begin()->second;
}

bool Garage::isIdleEngine(const Engine &engine) const
{
    const std::lock_guard lock(mutex);
    const auto position = idleEngines.find(engine.getId());
    return position != idleEngines.end() && position->second == &engine;
}

std::size_t Garage::getActiveEngineCount() const
{
    const std::lock_guard lock(mutex);
    return activeEngines.size();
}

std::size_t Garage::getEngineCount() const
{
    const std::lock_guard lock(mutex);
    return idleEngines.size() + activeEngines.size();
}

void Garage::activateEngine(Engine *engine)
{
    const std::lock_guard lock(mutex);
    if (engine == nullptr)
        return;
    auto node = idleEngines.extract(engine->getId());
    if (node.empty() || node.mapped() != engine)
    {
        if (!node.empty())
            idleEngines.insert(std::move(node));
        return;
    }
    activeEngines.insert(std::move(node));
}

void Garage::recycleInactiveEngines()
{
    const std::lock_guard lock(mutex);
    auto position = activeEngines.begin();
    while (position != activeEngines.end())
    {
        if (position->second->getPad().isActive())
        {
            ++position;
            continue;
        }
        auto current = position++;
        idleEngines.insert(activeEngines.extract(current));
    }
}

std::recursive_mutex &Garage::getMutex() const
{
    return mutex;
}
