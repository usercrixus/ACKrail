#pragma once

#include "../../algorithm/Route.hpp"

/**
 * Owns and executes the mutable journey state of an engine.
 *
 * EnginePad stores the active route, speed, timing, and progress while Engine
 * retains the physical specifications of the vehicle.
 */
class EnginePad
{
public:
    /** Creates an idle journey state. */
    EnginePad() = default;

    EnginePad(const EnginePad &other) = delete;

    EnginePad &operator=(const EnginePad &other) = delete;

    /** Releases the owned active route. */
    ~EnginePad();

    /**
     * Checks whether a route is active.
     *
     * @return true when a route is active; otherwise false.
     */
    bool isActive() const;

    /**
     * Starts a route governed by its timing contract.
     *
     * @param route Contracted route whose ownership is transferred on
     * success.
     * @return true when the contracted route was accepted; otherwise false.
     */
    bool startContractedTrajectory(Route *route);

    /**
     * Advances the active route.
     *
     * @param elapsedSeconds Elapsed simulation time.
     */
    void advance(double elapsedSeconds);

    /** @return Maximum speed in kilometers per hour. */
    double getMaximumSpeedKilometersPerHour() const;

    /** @return Current speed in kilometers per hour. */
    double getCurrentSpeedKilometersPerHour() const;

    /** @return Elapsed trajectory time in seconds. */
    double getElapsedTrajectorySeconds() const;

    /** @return Average trajectory speed in kilometers per hour. */
    double getAverageSpeedKilometersPerHour() const;

    /** @return Distance travelled on the active trajectory in kilometers. */
    double getTravelledDistanceKilometers() const;

    /** @return Active route, or nullptr when idle. */
    const Route *getTrajectory() const;

    /**
     * Sets the maximum speed.
     *
     * @param maximumSpeedKilometersPerHour Maximum speed in kilometers per
     * hour.
     */
    void setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour);

private:
    /** Completes the active route and resets transient state. */
    void finishTrajectory();

    Route *trajectory = nullptr;
    qsizetype currentContractStep = 0;
    double maximumSpeedKilometersPerHour = 0.0;
    double currentSpeedKilometersPerHour = 0.0;
    double elapsedTrajectorySeconds = 0.0;
    double averageSpeedKilometersPerHour = 0.0;
    double travelledDistanceKilometers = 0.0;
    double remainingTraversalKilometers = 0.0;
    double remainingWaitSeconds = 0.0;
};
