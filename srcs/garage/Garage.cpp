#include "Garage.hpp"

Garage::Garage(std::size_t engineCount)
{
    idleEngines.reserve(engineCount);
    activeEngines.reserve(engineCount);
    for (std::size_t index = 0; index < engineCount; ++index)
    {
        const int id = static_cast<int>(index);
        Engine *engine = new Biplace(id);
        idleEngines.push(id, engine);
    }
}

Garage::~Garage()
{
    for (Engine *engine : idleEngines)
        delete engine;
    for (Engine *engine : activeEngines)
        delete engine;
}

const IndexedRandomPool<int, Engine *> &Garage::getIdleEngines() const
{
    return idleEngines;
}

const IndexedRandomPool<int, Engine *> &Garage::getActiveEngines() const
{
    return activeEngines;
}

Engine *Garage::getIdleEngine()
{
    return idleEngines.empty() ? nullptr : idleEngines.back();
}

Engine *Garage::getRandomIdleEngine(std::mt19937 &randomGenerator)
{
    if (idleEngines.empty())
        return nullptr;
    return idleEngines.random(randomGenerator);
}

bool Garage::isIdleEngine(const Engine &engine) const
{
    Engine *const *idleEngine = idleEngines.find(engine.getId());
    return idleEngine != nullptr && *idleEngine == &engine;
}

std::size_t Garage::getActiveEngineCount() const
{
    return activeEngines.size();
}

std::size_t Garage::getEngineCount() const
{
    return idleEngines.size() + activeEngines.size();
}

void Garage::activateEngine(Engine *engine)
{
    if (engine == nullptr)
        return;
    Engine *const *idleEngine = idleEngines.find(engine->getId());
    if (idleEngine == nullptr || *idleEngine != engine)
        return;
    const int movedEngineId = idleEngines.back()->getId();
    idleEngines.erase(engine->getId(), movedEngineId);
    activeEngines.push(engine->getId(), engine);
}

void Garage::recycleInactiveEngines()
{
    std::size_t index = 0;
    while (index < activeEngines.size())
    {
        Engine *engine = activeEngines[index];
        if (engine->getPad().isActive())
        {
            ++index;
            continue;
        }
        const int movedEngineId = activeEngines.back()->getId();
        activeEngines.erase(engine->getId(), movedEngineId);
        idleEngines.push(engine->getId(), engine);
    }
}
