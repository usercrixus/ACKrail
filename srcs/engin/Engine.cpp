#include "Engine.hpp"

#include <algorithm>
#include <utility>

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::setModelName(QString modelName)
{
    this->modelName = std::move(modelName);
}

void Engine::setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour)
{
    this->maximumSpeedKilometersPerHour = maximumSpeedKilometersPerHour;
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

void Engine::setCurrentSpeedKilometersPerHour(double speedKilometersPerHour)
{
    currentSpeedKilometersPerHour = std::clamp(speedKilometersPerHour, 0.0, maximumSpeedKilometersPerHour);
}
