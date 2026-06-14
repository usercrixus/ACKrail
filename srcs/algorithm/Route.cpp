#include "Route.hpp"

#include <utility>

Route::Route(QVector<Node *> stations, QVector<Link *> links)
    : Route(std::move(stations), std::move(links), {})
{
}

Route::Route(QVector<Node *> stations, QVector<Link *> links, QVector<ContractStep> contract)
    : stations(std::move(stations)),
      links(std::move(links)),
      contract(std::move(contract))
{
    for (Link *link : this->links)
    {
        if (link != nullptr)
            totalDistanceKilometers += link->getDistanceKilometers();
    }
}

bool Route::hasValidContract() const
{
    if (!isValid() || contract.size() != links.size())
        return false;
    for (const ContractStep &step : contract)
    {
        if (step.waitSeconds < 0.0 || step.traversalSeconds <= 0.0)
            return false;
    }
    return true;
}

bool Route::isValid() const
{
    if (stations.size() < 2 || links.size() != stations.size() - 1 || totalDistanceKilometers <= 0.0)
        return false;
    for (qsizetype index = 0; index < links.size(); ++index)
    {
        Link *link = links[index];
        if (link == nullptr)
            return false;
        if (stations[index] == nullptr || stations[index + 1] == nullptr)
            return false;
        const int stationId = stations[index]->getId();
        const int nextStationId = stations[index + 1]->getId();
        const int linkFromStationId = link->getFromNode().getId();
        const int linkToStationId = link->getToNode().getId();
        if (!((linkFromStationId == stationId && linkToStationId == nextStationId) || (linkFromStationId == nextStationId && linkToStationId == stationId)))
            return false;
    }
    return true;
}

const QVector<Node *> &Route::getStations() const
{
    return stations;
}

const QVector<Link *> &Route::getLinks() const
{
    return links;
}

const QVector<Route::ContractStep> &Route::getContract() const
{
    return contract;
}

double Route::getTotalDistanceKilometers() const
{
    return totalDistanceKilometers;
}
