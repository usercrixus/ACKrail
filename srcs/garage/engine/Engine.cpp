#include "./Engine.hpp"

#include <algorithm>
#include <utility>

Engine::Engine(int id)
    : id(id)
{
}

Engine::~Engine()
{
}

int Engine::getId() const
{
    return id;
}

const QString &Engine::getModelName() const
{
    return modelName;
}

double Engine::getLengthMeters() const
{
    return lengthMeters;
}

double Engine::getSecurityDistanceMeters() const
{
    return securityDistanceMeters;
}

double Engine::getEntrySeparationSeconds() const
{
    return entrySeparationSeconds;
}

EnginePad &Engine::getPad()
{
    return pad;
}

const EnginePad &Engine::getPad() const
{
    return pad;
}

void Engine::setModelName(QString modelName)
{
    this->modelName = std::move(modelName);
}

void Engine::setMaximumSpeedKilometersPerHour(
    double maximumSpeedKilometersPerHour)
{
    pad.setMaximumSpeedKilometersPerHour(maximumSpeedKilometersPerHour);
    updateEntrySeparationSeconds();
}

void Engine::setLengthMeters(double lengthMeters)
{
    this->lengthMeters = std::max(0.0, lengthMeters);
    updateEntrySeparationSeconds();
}

void Engine::setSecurityDistanceMeters(double securityDistanceMeters)
{
    this->securityDistanceMeters = std::max(0.0, securityDistanceMeters);
    updateEntrySeparationSeconds();
}

void Engine::updateEntrySeparationSeconds()
{
    const double maximumSpeedKilometersPerHour = pad.getMaximumSpeedKilometersPerHour();
    if (maximumSpeedKilometersPerHour <= 0.0)
    {
        entrySeparationSeconds = 0.0;
        return;
    }
    const double protectedDistanceKilometers = (lengthMeters + securityDistanceMeters) / 1000.0;
    entrySeparationSeconds = protectedDistanceKilometers / maximumSpeedKilometersPerHour * 3600.0;
}
