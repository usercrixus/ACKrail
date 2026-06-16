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

bool EnginePad::hasParkingStation() const
{
    return parkingStationId >= 0;
}

int EnginePad::getParkingStationId() const
{
    return parkingStationId;
}

void EnginePad::setParkingStationId(int stationId)
{
    parkingStationId = stationId;
}

bool EnginePad::startContractedTrajectory(Route *route)
{
    if (isActive() || route == nullptr || !route->hasValidContract() || maximumSpeedKilometersPerHour <= 0.0)
        return false;
    if (hasParkingStation() && route->getStations().first()->getId() != parkingStationId)
        return false;
    trajectory = route;
    elapsedTrajectorySeconds = 0.0;
    totalTrajectorySeconds = 0.0;
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
        const double waitedSeconds = advanceWait(remainingSeconds);
        remainingSeconds -= waitedSeconds;
        activeSeconds += waitedSeconds;
        if (remainingWaitSeconds > 0.0)
            break;
        const double traversalSeconds = advanceCurrentLink(remainingSeconds);
        remainingSeconds -= traversalSeconds;
        activeSeconds += traversalSeconds;
        if (remainingTraversalKilometers > 0.0)
            break;
        if (!advanceContractStep())
        {
            travelledDistanceKilometers = trajectory->getTotalDistanceKilometers();
            updateTrajectoryMetrics(activeSeconds);
            finishTrajectory();
            return;
        }
    }

    updateTrajectoryMetrics(activeSeconds);
}

double EnginePad::advanceWait(double availableSeconds)
{
    if (remainingWaitSeconds <= 0.0)
        return 0.0;
    currentSpeedKilometersPerHour = 0.0;
    const double consumedSeconds = std::min(availableSeconds, remainingWaitSeconds);
    remainingWaitSeconds -= consumedSeconds;
    return consumedSeconds;
}

double EnginePad::advanceCurrentLink(double availableSeconds)
{
    currentSpeedKilometersPerHour = maximumSpeedKilometersPerHour;
    const double secondsToEndOfLink = remainingTraversalKilometers / currentSpeedKilometersPerHour * 3600.0;
    const double consumedSeconds = std::min(availableSeconds, secondsToEndOfLink);
    const double travelledKilometers = currentSpeedKilometersPerHour * consumedSeconds / 3600.0;
    remainingTraversalKilometers = std::max(0.0, remainingTraversalKilometers - travelledKilometers);
    travelledDistanceKilometers += travelledKilometers;
    return consumedSeconds;
}

bool EnginePad::advanceContractStep()
{
    ++currentContractStep;
    if (currentContractStep >= trajectory->getContract().size())
        return false;
    remainingWaitSeconds = trajectory->getContract()[currentContractStep].waitSeconds;
    remainingTraversalKilometers = trajectory->getLinks()[currentContractStep]->getDistanceKilometers();
    return true;
}

void EnginePad::updateTrajectoryMetrics(double activeSeconds)
{
    elapsedTrajectorySeconds += activeSeconds;
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? travelledDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
}

void EnginePad::finishTrajectory()
{
    const QVector<Node *> &stations = trajectory->getStations();
    if (!stations.isEmpty() && stations.last() != nullptr)
        parkingStationId = stations.last()->getId();
    delete trajectory;
    trajectory = nullptr;
    currentSpeedKilometersPerHour = 0.0;
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

double EnginePad::getTotalTrajectorySeconds() const
{
    return totalTrajectorySeconds;
}

void EnginePad::setTotalTrajectorySeconds(double totalTrajectorySeconds)
{
    this->totalTrajectorySeconds = std::max(0.0, totalTrajectorySeconds);
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
    return std::clamp((linkDistance - remainingTraversalKilometers) / linkDistance, 0.0, 1.0);
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
