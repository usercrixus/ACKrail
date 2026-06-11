#pragma once

#include "../topology/Link.hpp"
#include "../topology/Node.hpp"

#include <QVector>

class Route
{
public:
    Route(QVector<Node> stations, QVector<const Link *> links);

    bool isValid() const;
    const QVector<Node> &getStations() const;
    const QVector<const Link *> &getLinks() const;
    double getTotalDistanceKilometers() const;

private:
    QVector<Node> stations;
    QVector<const Link *> links;
    double totalDistanceKilometers = 0.0;
};
