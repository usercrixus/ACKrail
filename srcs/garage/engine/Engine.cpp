#include "./Engine.hpp"

#include <algorithm>
#include <limits>
#include <utility>

Engine::Engine()
{
}

Engine::~Engine()
{
    delete trajectory;
}

bool Engine::startTrajectory(const Route &route)
{
    if (isActive() || !route.isValid())
        return false;
    trajectory = new Route(route);
    elapsedTrajectorySeconds = 0.0;
    averageSpeedKilometersPerHour = 0.0;
    secondsToDestinationAtCurrentSpeed = calculateSecondsToDestinationAtCurrentSpeed();
    return true;
}

void Engine::advance(double elapsedSeconds)
{
    if (!isActive() || elapsedSeconds <= 0.0)
        return;
    const bool reachesDestination = elapsedSeconds >= secondsToDestinationAtCurrentSpeed;
    const double activeSeconds = std::min(elapsedSeconds, secondsToDestinationAtCurrentSpeed);
    const double currentDistanceKilometers = averageSpeedKilometersPerHour * elapsedTrajectorySeconds / 3600.0;
    const double newDistanceKilometers = currentDistanceKilometers + currentSpeedKilometersPerHour * activeSeconds / 3600.0;
    elapsedTrajectorySeconds += activeSeconds;
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? newDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
    secondsToDestinationAtCurrentSpeed -= activeSeconds;
    if (reachesDestination)
    {
        averageSpeedKilometersPerHour = trajectory->getTotalDistanceKilometers() / elapsedTrajectorySeconds * 3600.0;
        delete trajectory;
        trajectory = nullptr;
        secondsToDestinationAtCurrentSpeed = 0.0;
    }
}

double Engine::calculateSecondsToDestinationAtCurrentSpeed() const
{
    if (!isActive())
        return 0.0;
    if (currentSpeedKilometersPerHour <= 0.0)
        return std::numeric_limits<double>::infinity();
    const double travelledDistanceKilometers = averageSpeedKilometersPerHour * elapsedTrajectorySeconds / 3600.0;
    const double remainingDistanceKilometers = trajectory->getTotalDistanceKilometers() - travelledDistanceKilometers;
    return remainingDistanceKilometers / currentSpeedKilometersPerHour * 3600.0;
}

bool Engine::isActive() const
{
    return trajectory != nullptr;
}

const QString &Engine::getModelName() const
{
    return modelName;
}

double Engine::getMaximumSpeedKilometersPerHour() const
{
    return maximumSpeedKilometersPerHour;
}

double Engine::getCurrentSpeedKilometersPerHour() const
{
    return currentSpeedKilometersPerHour;
}

double Engine::getElapsedTrajectorySeconds() const
{
    return elapsedTrajectorySeconds;
}

double Engine::getAverageSpeedKilometersPerHour() const
{
    return averageSpeedKilometersPerHour;
}

const Route *Engine::getTrajectory() const
{
    return trajectory;
}

void Engine::setCurrentSpeedKilometersPerHour(double speedKilometersPerHour)
{
    currentSpeedKilometersPerHour = std::clamp(speedKilometersPerHour, 0.0, maximumSpeedKilometersPerHour);
    secondsToDestinationAtCurrentSpeed = calculateSecondsToDestinationAtCurrentSpeed();
}

void Engine::setModelName(QString modelName)
{
    this->modelName = std::move(modelName);
}

void Engine::setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour)
{
    this->maximumSpeedKilometersPerHour = maximumSpeedKilometersPerHour;
}
