#include "./Engine.hpp"

#include <algorithm>
#include <utility>

Engine::Engine()
{
}

Engine::~Engine()
{
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
}

void Engine::setLengthMeters(double lengthMeters)
{
    this->lengthMeters = std::max(0.0, lengthMeters);
}

void Engine::setSecurityDistanceMeters(double securityDistanceMeters)
{
    this->securityDistanceMeters = std::max(0.0, securityDistanceMeters);
}
