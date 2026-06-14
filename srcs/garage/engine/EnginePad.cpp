#include "EnginePad.hpp"

#include <algorithm>

EnginePad::~EnginePad()
{
    delete trajectory;
}

bool EnginePad::isActive() const
{
    return trajectory != nullptr;
}

bool EnginePad::startContractedTrajectory(Route *route)
{
    if (isActive() || route == nullptr || !route->hasValidContract() || maximumSpeedKilometersPerHour <= 0.0)
        return false;
    trajectory = route;
    elapsedTrajectorySeconds = 0.0;
    averageSpeedKilometersPerHour = 0.0;
    travelledDistanceKilometers = 0.0;
    currentContractStep = 0;
    remainingWaitSeconds = trajectory->getContract().first().waitSeconds;
    remainingTraversalKilometers = trajectory->getLinks().first()->getDistanceKilometers();
    currentSpeedKilometersPerHour = remainingWaitSeconds > 0.0 ? 0.0 : maximumSpeedKilometersPerHour;
    return true;
}

void EnginePad::advance(double elapsedSeconds)
{
    if (!isActive() || elapsedSeconds <= 0.0)
        return;

    double remainingSeconds = elapsedSeconds;
    double activeSeconds = 0.0;
    while (isActive() && remainingSeconds > 0.0)
    {
        if (remainingWaitSeconds > 0.0)
        {
            currentSpeedKilometersPerHour = 0.0;
            const double consumedSeconds = std::min(remainingSeconds, remainingWaitSeconds);
            remainingWaitSeconds -= consumedSeconds;
            remainingSeconds -= consumedSeconds;
            activeSeconds += consumedSeconds;
            if (remainingWaitSeconds > 0.0)
                break;
        }

        currentSpeedKilometersPerHour = maximumSpeedKilometersPerHour;
        const double secondsToEndOfLink = remainingTraversalKilometers / currentSpeedKilometersPerHour * 3600.0;
        const double consumedSeconds = std::min(remainingSeconds, secondsToEndOfLink);
        const double travelledKilometers = currentSpeedKilometersPerHour * consumedSeconds / 3600.0;
        remainingTraversalKilometers = std::max(0.0, remainingTraversalKilometers - travelledKilometers);
        travelledDistanceKilometers += travelledKilometers;
        remainingSeconds -= consumedSeconds;
        activeSeconds += consumedSeconds;

        if (remainingTraversalKilometers > 0.0)
            break;

        ++currentContractStep;
        if (currentContractStep >= trajectory->getContract().size())
        {
            travelledDistanceKilometers = trajectory->getTotalDistanceKilometers();
            elapsedTrajectorySeconds += activeSeconds;
            averageSpeedKilometersPerHour = travelledDistanceKilometers / elapsedTrajectorySeconds * 3600.0;
            finishTrajectory();
            return;
        }

        remainingWaitSeconds = trajectory->getContract()[currentContractStep].waitSeconds;
        remainingTraversalKilometers = trajectory->getLinks()[currentContractStep]->getDistanceKilometers();
    }

    elapsedTrajectorySeconds += activeSeconds;
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? travelledDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
}

void EnginePad::finishTrajectory()
{
    delete trajectory;
    trajectory = nullptr;
    currentSpeedKilometersPerHour = 0.0;
    currentContractStep = 0;
    remainingWaitSeconds = 0.0;
    remainingTraversalKilometers = 0.0;
}

double EnginePad::getCurrentSpeedKilometersPerHour() const
{
    return currentSpeedKilometersPerHour;
}

double EnginePad::getMaximumSpeedKilometersPerHour() const
{
    return maximumSpeedKilometersPerHour;
}

double EnginePad::getElapsedTrajectorySeconds() const
{
    return elapsedTrajectorySeconds;
}

double EnginePad::getAverageSpeedKilometersPerHour() const
{
    return averageSpeedKilometersPerHour;
}

double EnginePad::getTravelledDistanceKilometers() const
{
    return travelledDistanceKilometers;
}

qsizetype EnginePad::getCurrentContractStep() const
{
    return currentContractStep;
}

double EnginePad::getCurrentLinkProgress() const
{
    if (trajectory == nullptr || currentContractStep >= trajectory->getLinks().size())
        return 0.0;
    const double linkDistance = trajectory->getLinks()[currentContractStep]->getDistanceKilometers();
    if (linkDistance <= 0.0)
        return 0.0;
    return std::clamp(
        (linkDistance - remainingTraversalKilometers) / linkDistance,
        0.0,
        1.0);
}

const Route *EnginePad::getTrajectory() const
{
    return trajectory;
}

void EnginePad::setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour)
{
    this->maximumSpeedKilometersPerHour =
        std::max(0.0, maximumSpeedKilometersPerHour);
}
