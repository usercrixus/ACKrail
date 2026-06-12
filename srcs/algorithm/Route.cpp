#include "Route.hpp"

#include <utility>

Route::Route(QVector<Node> stations, QVector<const Link *> links)
    : stations(std::move(stations)),
      links(std::move(links))
{
    for (const Link *link : this->links)
    {
        if (link != nullptr)
            totalDistanceKilometers += link->getDistanceKilometers();
    }
}

bool Route::isValid() const
{
    if (stations.size() < 2 || links.size() != stations.size() - 1 || totalDistanceKilometers <= 0.0)
        return false;
    for (qsizetype index = 0; index < links.size(); ++index)
    {
        const Link *link = links[index];
        if (link == nullptr)
            return false;
        const int stationId = stations[index].getId();
        const int nextStationId = stations[index + 1].getId();
        const int linkFromStationId = link->getFromNode().getId();
        const int linkToStationId = link->getToNode().getId();
        if (!((linkFromStationId == stationId && linkToStationId == nextStationId) || (linkFromStationId == nextStationId && linkToStationId == stationId)))
            return false;
    }
    return true;
}

const QVector<Node> &Route::getStations() const
{
    return stations;
}

const QVector<const Link *> &Route::getLinks() const
{
    return links;
}

double Route::getTotalDistanceKilometers() const
{
    return totalDistanceKilometers;
}
