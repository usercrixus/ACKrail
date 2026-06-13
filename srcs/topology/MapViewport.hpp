#pragma once

#include "Topology.hpp"
#include <QPointF>

class MapViewport
{
public:
    MapViewport(
        const Topology &topology,
        double width,
        double height,
        double margin,
        double zoomFactor,
        const QPointF &panOffset);

    QPointF mapPosition(double latitude, double longitude) const;

private:
    double minimumProjectedX;
    double maximumProjectedY;
    double scale;
    double left;
    double top;
    double zoomFactor;
    QPointF mapCenter;
    QPointF panOffset;
};
