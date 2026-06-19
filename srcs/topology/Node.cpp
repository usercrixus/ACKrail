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

double Node::getArrivalWeight() const
{
    return arrivalWeight;
}

double Node::getDepartureWeight() const
{
    return departureWeight;
}

void Node::setWeights(double arrivalWeight, double departureWeight)
{
    this->arrivalWeight = arrivalWeight;
    this->departureWeight = departureWeight;
}

NodeController &Node::getController()
{
    return controller;
}

const NodeController &Node::getController() const
{
    return controller;
}
