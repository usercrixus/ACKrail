#include "Node.hpp"
#include <utility>

Node::Node(int id, QString name, double latitude, double longitude)
    : id(id),
      name(std::move(name)),
      latitude(latitude),
      longitude(longitude),
      controller(id)
{
}

int Node::getId() const
{
    return id;
}

const QString &Node::getName() const
{
    return name;
}

double Node::getLatitude() const
{
    return latitude;
}

double Node::getLongitude() const
{
    return longitude;
}

NodeController &Node::getController()
{
    return controller;
}

const NodeController &Node::getController() const
{
    return controller;
}
