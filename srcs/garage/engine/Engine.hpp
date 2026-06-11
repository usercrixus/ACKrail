#pragma once

#include "../../algorithm/Route.hpp"
#include <QString>

class Engine
{
public:
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&other) noexcept;
    Engine &operator=(Engine &&other) noexcept;

    /**
     * Destroys the engine.
     */
    virtual ~Engine();

    /**
     * Checks whether the engine is currently travelling on a route.
     *
     * @return true when the engine has a current route segment; otherwise false.
     */
    bool isActive() const;

    /**
     * Starts travelling along a route.
     *
     * @param route Route returned by RouteFinder.
     * @return true when the route contains a valid trajectory; otherwise false.
     */
    bool startTrajectory(const Route &route);

    /**
     * Moves the engine along its assigned route.
     *
     * @param elapsedSeconds Elapsed simulation time in seconds.
     */
    void advance(double elapsedSeconds);

    /**
     * Returns the engine model name.
     *
     * @return Engine model name.
     */
    const QString &getModelName() const;

    /**
     * Returns the maximum engine speed.
     *
     * @return Maximum speed in kilometers per hour.
     */
    double getMaximumSpeedKilometersPerHour() const;

    /**
     * Returns the current engine speed.
     *
     * @return Current speed in kilometers per hour.
     */
    double getCurrentSpeedKilometersPerHour() const;

    /**
     * Returns the elapsed time since the current trajectory started.
     *
     * @return Elapsed trajectory time in seconds.
     */
    double getElapsedTrajectorySeconds() const;

    /**
     * Returns the average speed since the current trajectory started.
     *
     * @return Average speed in kilometers per hour.
     */
    double getAverageSpeedKilometersPerHour() const;

    /**
     * Returns the current trajectory.
     *
     * @return Active trajectory, or nullptr when the engine is idle.
     */
    const Route *getTrajectory() const;

    /**
     * Sets the engine model name.
     *
     * @param modelName New model name.
     */
    void setModelName(QString modelName);

    /**
     * Sets the maximum engine speed.
     *
     * @param maximumSpeedKilometersPerHour Maximum speed in kilometers per hour.
     */
    void setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour);

    /**
     * Sets the current speed, clamped to the valid engine speed range.
     *
     * @param speedKilometersPerHour Requested speed in kilometers per hour.
     */
    void setCurrentSpeedKilometersPerHour(double speedKilometersPerHour);

protected:
    /**
     * Creates an idle engine.
     */
    Engine();

private:
    QString modelName;
    double maximumSpeedKilometersPerHour = 0.0;
    double currentSpeedKilometersPerHour = 0.0;
    double elapsedTrajectorySeconds = 0.0;
    double averageSpeedKilometersPerHour = 0.0;
    Route *trajectory = nullptr;
};
