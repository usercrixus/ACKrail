#include "Link.hpp"

#include <utility>

Link::Link(int id, const Node &fromNode, const Node &toNode, QString lineName, QString color)
    : id(id),
      fromNode(fromNode),
      toNode(toNode),
      lineName(std::move(lineName)),
      color(std::move(color))
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
