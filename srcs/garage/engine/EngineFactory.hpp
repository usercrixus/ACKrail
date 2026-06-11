#pragma once

#include "./Biplace.hpp"

/**
 * Creates configured engine models.
 */
class EngineFactory
{
public:
    /**
     * Creates a biplace passenger engine with its default specifications.
     *
     * @return Configured biplace engine.
     */
    static Biplace createBiplace();
};
