#include "Link.hpp"

#include "../geography/Geography.hpp"

#include <utility>

Link::Link(int id, const Node &fromNode, const Node &toNode, QString lineName, QString color)
    : id(id),
      fromNode(fromNode),
      toNode(toNode),
      lineName(std::move(lineName)),
      color(std::move(color)),
      distanceKilometers(Geography::distanceKilometers(fromNode.getLatitude(), fromNode.getLongitude(), toNode.getLatitude(), toNode.getLongitude()))
{
}

int Link::getId() const
{
    return id;
}

const Node &Link::getFromNode() const
{
    return fromNode;
}

const Node &Link::getToNode() const
{
    return toNode;
}

const QString &Link::getLineName() const
{
    return lineName;
}

const QString &Link::getColor() const
{
    return color;
}

double Link::getDistanceKilometers() const
{
    return distanceKilometers;
}
