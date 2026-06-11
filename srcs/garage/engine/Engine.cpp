#include "./Engine.hpp"

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

const Link *Engine::getCurrentLink() const
{
    if (!isActive())
        return nullptr;
    return route[currentSegmentIndex].link;
}

const Node *Engine::getFromNode() const
{
    return isActive()
        ? nodeOnCurrentLink(route[currentSegmentIndex].fromStationId)
        : nullptr;
}

const Node *Engine::getToNode() const
{
    return isActive()
        ? nodeOnCurrentLink(route[currentSegmentIndex].toStationId)
        : nullptr;
}

double Engine::getDistanceOnCurrentLinkKilometers() const
{
    return distanceOnCurrentLinkKilometers;
}

double Engine::getCurrentLinkProgress() const
{
    const Link *currentLink = getCurrentLink();
    if (currentLink == nullptr || currentLink->getDistanceKilometers() <= 0.0)
        return 0.0;
    return distanceOnCurrentLinkKilometers / currentLink->getDistanceKilometers();
}

bool Engine::isActive() const
{
    return currentSegmentIndex >= 0 && currentSegmentIndex < route.size();
}

void Engine::setCurrentSpeedKilometersPerHour(double speedKilometersPerHour)
{
    currentSpeedKilometersPerHour = std::clamp(
        speedKilometersPerHour,
        0.0,
        maximumSpeedKilometersPerHour);
}

void Engine::placeOnLink(const Link &link)
{
    assignRoute(QVector<const Link *>{&link}, link.getFromNode().getId());
}

bool Engine::assignRoute(const QVector<const Link *> &links, int startStationId)
{
    QVector<RouteSegment> newRoute;
    newRoute.reserve(links.size());
    int currentStationId = startStationId;

    for (const Link *link : links)
    {
        if (link == nullptr)
            return false;

        int nextStationId = -1;
        if (link->getFromNode().getId() == currentStationId)
            nextStationId = link->getToNode().getId();
        else if (link->getToNode().getId() == currentStationId)
            nextStationId = link->getFromNode().getId();
        else
            return false;

        newRoute.append({link, currentStationId, nextStationId});
        currentStationId = nextStationId;
    }

    route = std::move(newRoute);
    currentSegmentIndex = route.isEmpty() ? -1 : 0;
    distanceOnCurrentLinkKilometers = 0.0;
    return isActive();
}

void Engine::advance(double elapsedSeconds)
{
    if (!isActive() || elapsedSeconds <= 0.0)
        return;

    double distanceToTravel = currentSpeedKilometersPerHour * elapsedSeconds / 3600.0;
    while (distanceToTravel > 0.0 && isActive())
    {
        const double linkDistance = getCurrentLink()->getDistanceKilometers();
        if (linkDistance <= 0.0)
        {
            finishCurrentSegment();
            continue;
        }

        const double remainingDistance = linkDistance - distanceOnCurrentLinkKilometers;
        if (distanceToTravel < remainingDistance)
        {
            distanceOnCurrentLinkKilometers += distanceToTravel;
            return;
        }

        distanceToTravel -= remainingDistance;
        finishCurrentSegment();
    }
}

const Node *Engine::nodeOnCurrentLink(int stationId) const
{
    const Link *currentLink = getCurrentLink();
    if (currentLink == nullptr)
        return nullptr;
    if (currentLink->getFromNode().getId() == stationId)
        return &currentLink->getFromNode();
    if (currentLink->getToNode().getId() == stationId)
        return &currentLink->getToNode();
    return nullptr;
}

void Engine::finishCurrentSegment()
{
    distanceOnCurrentLinkKilometers = 0.0;
    ++currentSegmentIndex;
    if (currentSegmentIndex >= route.size())
    {
        route.clear();
        currentSegmentIndex = -1;
    }
}
