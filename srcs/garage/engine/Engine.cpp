#include "./Engine.hpp"

#include <algorithm>
#include <utility>

Engine::Engine()
{
}

Engine::Engine(Engine &&other) noexcept
    : modelName(std::move(other.modelName)),
      maximumSpeedKilometersPerHour(other.maximumSpeedKilometersPerHour),
      currentSpeedKilometersPerHour(other.currentSpeedKilometersPerHour),
      elapsedTrajectorySeconds(other.elapsedTrajectorySeconds),
      averageSpeedKilometersPerHour(other.averageSpeedKilometersPerHour),
      trajectory(other.trajectory)
{
    other.trajectory = nullptr;
}

Engine &Engine::operator=(Engine &&other) noexcept
{
    if (this == &other)
        return *this;

    delete trajectory;
    modelName = std::move(other.modelName);
    maximumSpeedKilometersPerHour = other.maximumSpeedKilometersPerHour;
    currentSpeedKilometersPerHour = other.currentSpeedKilometersPerHour;
    elapsedTrajectorySeconds = other.elapsedTrajectorySeconds;
    averageSpeedKilometersPerHour = other.averageSpeedKilometersPerHour;
    trajectory = other.trajectory;
    other.trajectory = nullptr;
    return *this;
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
    return true;
}

void Engine::advance(double elapsedSeconds)
{
    if (!isActive() || elapsedSeconds <= 0.0)
        return;
    const double currentDistanceKilometers = averageSpeedKilometersPerHour * elapsedTrajectorySeconds / 3600.0;
    const double remainingDistanceKilometers = trajectory->getTotalDistanceKilometers() - currentDistanceKilometers;
    double activeSeconds = elapsedSeconds;
    bool reachesDestination = false;
    if (currentSpeedKilometersPerHour > 0.0)
    {
        const double secondsToDestination = remainingDistanceKilometers / currentSpeedKilometersPerHour * 3600.0;
        activeSeconds = std::min(elapsedSeconds, secondsToDestination);
        reachesDestination = elapsedSeconds >= secondsToDestination;
    }
    const double newDistanceKilometers = currentDistanceKilometers + currentSpeedKilometersPerHour * activeSeconds / 3600.0;
    elapsedTrajectorySeconds += activeSeconds;
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? newDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
    if (reachesDestination)
    {
        averageSpeedKilometersPerHour = trajectory->getTotalDistanceKilometers() / elapsedTrajectorySeconds * 3600.0;
        delete trajectory;
        trajectory = nullptr;
    }
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
}

void Engine::setModelName(QString modelName)
{
    this->modelName = std::move(modelName);
}

void Engine::setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour)
{
    this->maximumSpeedKilometersPerHour = maximumSpeedKilometersPerHour;
}
