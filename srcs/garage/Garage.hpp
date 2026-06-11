#pragma once

#include "engine/Biplace.hpp"
#include <cstddef>
#include <vector>

/**
 * Owns the simulated rail-module fleet and provides access to its engines.
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
     * Returns the complete fleet.
     *
     * @return Active and idle engines owned by the garage.
     */
    const std::vector<Biplace> &getEngines() const;

    /**
     * Returns the mutable fleet for simulation updates.
     *
     * @return Active and idle engines owned by the garage.
     */
    std::vector<Biplace> &getEngines();

    /**
     * Finds an engine that is not currently travelling.
     *
     * @return An idle engine, or nullptr when every engine is active.
     */
    Biplace *getIdleEngine();

    /**
     * Counts engines currently travelling.
     *
     * @return Number of active engines.
     */
    std::size_t getActiveEngineCount() const;

private:
    std::vector<Biplace> engines;
};
