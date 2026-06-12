#pragma once

#include "../garage/Garage.hpp"

/**
 * Advances and manages engine simulation.
 */
class TrafficManager
{
public:
    /**
     * Creates a traffic manager for a garage.
     *
     * @param garage Garage containing the simulated engines.
     */
    TrafficManager(Garage &garage);

    /**
     * Advances active engines.
     *
     * @param elapsedSeconds Elapsed real time in seconds.
     */
    void advance(double elapsedSeconds);

private:
    Garage &garage;
};
