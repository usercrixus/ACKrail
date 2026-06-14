#pragma once

#include "../../topology/Topology.hpp"
#include <QPointF>

class MapViewport
{
public:
    explicit MapViewport(const Topology &topology);
    QPointF mapPosition(double latitude, double longitude) const;

private:
    double minimumProjectedX;
    double maximumProjectedY;
    double scale;
};
