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

bool EnginePad::startContractedTrajectory(Route *route, double simulationTimeSeconds)
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
    completedDistanceKilometers = 0.0;
    currentContractStep = 0;
    trajectoryStartSeconds = simulationTimeSeconds;
    remainingWaitSeconds = trajectory->getContract().first().waitSeconds;
    remainingTraversalKilometers = trajectory->getLinks().first()->getDistanceKilometers();
    enterContractStep(0, simulationTimeSeconds + remainingWaitSeconds, simulationTimeSeconds + remainingWaitSeconds + trajectory->getContract().first().traversalSeconds);
    return true;
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

double EnginePad::getCurrentSpeedKilometersPerHour(double simulationTimeSeconds) const
{
    if (!isActive() || simulationTimeSeconds < currentLinkEntryTimeSeconds || simulationTimeSeconds >= currentLinkExitTimeSeconds)
        return 0.0;
    return maximumSpeedKilometersPerHour;
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

double EnginePad::getTravelledDistanceKilometers(double simulationTimeSeconds) const
{
    if (!isActive())
        return travelledDistanceKilometers;
    return completedDistanceKilometers + getCurrentLinkProgress(simulationTimeSeconds) * trajectory->getLinks()[currentContractStep]->getDistanceKilometers();
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

double EnginePad::getCurrentLinkProgress(double simulationTimeSeconds) const
{
    if (trajectory == nullptr || currentContractStep >= trajectory->getLinks().size())
        return 0.0;
    if (simulationTimeSeconds <= currentLinkEntryTimeSeconds)
        return 0.0;
    if (simulationTimeSeconds >= currentLinkExitTimeSeconds)
        return 1.0;
    const double traversalSeconds = currentLinkExitTimeSeconds - currentLinkEntryTimeSeconds;
    if (traversalSeconds <= 0.0)
        return 0.0;
    return std::clamp((simulationTimeSeconds - currentLinkEntryTimeSeconds) / traversalSeconds, 0.0, 1.0);
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

void EnginePad::enterContractStep(qsizetype step, double entryTimeSeconds, double exitTimeSeconds)
{
    if (trajectory == nullptr || step < 0 || step >= trajectory->getContract().size())
        return;
    currentContractStep = step;
    currentLinkEntryTimeSeconds = entryTimeSeconds;
    currentLinkExitTimeSeconds = exitTimeSeconds;
    remainingWaitSeconds = std::max(0.0, entryTimeSeconds - trajectoryStartSeconds);
    remainingTraversalKilometers = trajectory->getLinks()[step]->getDistanceKilometers();
    currentSpeedKilometersPerHour = 0.0;
}

void EnginePad::completeContractStep(qsizetype step, double completionTimeSeconds)
{
    if (trajectory == nullptr || step < 0 || step >= trajectory->getLinks().size())
        return;
    completedDistanceKilometers += trajectory->getLinks()[step]->getDistanceKilometers();
    travelledDistanceKilometers = std::min(completedDistanceKilometers, trajectory->getTotalDistanceKilometers());
    elapsedTrajectorySeconds = std::max(0.0, completionTimeSeconds - trajectoryStartSeconds);
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? travelledDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
    remainingTraversalKilometers = 0.0;
    currentSpeedKilometersPerHour = 0.0;
}

void EnginePad::finishContractedTrajectory(double completionTimeSeconds)
{
    if (trajectory == nullptr)
        return;
    travelledDistanceKilometers = trajectory->getTotalDistanceKilometers();
    elapsedTrajectorySeconds = std::max(0.0, completionTimeSeconds - trajectoryStartSeconds);
    averageSpeedKilometersPerHour = elapsedTrajectorySeconds > 0.0 ? travelledDistanceKilometers / elapsedTrajectorySeconds * 3600.0 : 0.0;
    currentContractStep = trajectory->getContract().size();
    finishTrajectory();
}
