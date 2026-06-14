#pragma once

#include "engine/Biplace.hpp"
#include <cstddef>
#include <mutex>
#include <unordered_map>

/**
 * Owns and manages the simulated engine fleet.
 *
 * Engines are dynamically allocated as Biplace instances and stored as
 * Engine pointers. Each engine belongs to exactly one collection: idle or
 * active.
 */
class Garage
{
public:
    /**
     * Creates and owns the engine fleet.
     *
     * @param engineCount Number of engines to create.
     */
    Garage(std::size_t engineCount);

    /**
     * Destroys every engine owned by the garage.
     */
    ~Garage();

    /** Garage ownership cannot be copied. */
    Garage(const Garage &) = delete;

    /** Garage ownership cannot be copied. */
    Garage &operator=(const Garage &) = delete;

    /**
     * Returns the engines that are available for a new route.
     *
     * @return Read-only collection of idle engine pointers.
     */
    const std::unordered_map<int, Engine *> &getIdleEngines() const;

    /**
     * Returns the engines currently executing a route.
     *
     * @return Read-only collection of active engine pointers.
     */
    const std::unordered_map<int, Engine *> &getActiveEngines() const;

    /**
     * Returns an engine available for a new route.
     *
     * @return An idle engine, or nullptr when every engine is active.
     */
    Engine *getIdleEngine();

    /**
     * Checks whether an engine belongs to the idle collection.
     *
     * @param engine Engine to find.
     * @return true when the engine is idle; otherwise false.
     */
    bool isIdleEngine(const Engine &engine) const;

    /**
     * Returns the number of engines currently executing a route.
     *
     * @return Number of active engines.
     */
    std::size_t getActiveEngineCount() const;

    /**
     * Returns the total number of engines owned by the garage.
     *
     * @return Combined number of idle and active engines.
     */
    std::size_t getEngineCount() const;

    /**
     * Moves an idle engine to the active collection in average constant time.
     *
     * The operation has no effect if the pointer is null or does not match the
     * idle engine stored under its identifier.
     *
     * @param engine Engine whose contracted route has started.
     */
    void activateEngine(Engine *engine);

    /**
     * Moves every engine with a completed route back to the idle collection.
     */
    void recycleInactiveEngines();

    /**
     * Returns the mutex protecting engine collections and mutable engine state.
     *
     * @return Garage synchronization mutex.
     */
    std::recursive_mutex &getMutex() const;

private:
    mutable std::recursive_mutex mutex;
    std::unordered_map<int, Engine *> idleEngines;
    std::unordered_map<int, Engine *> activeEngines;
};
