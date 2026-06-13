#include "MapViewport.hpp"
#include "../geography/Geography.hpp"

#include <algorithm>

MapViewport::MapViewport(const Topology &topology)
{
    const Point minimumPosition = Geography::webMercator(
        topology.getMinimumLatitude(),
        topology.getMinimumLongitude());
    const Point maximumPosition = Geography::webMercator(
        topology.getMaximumLatitude(),
        topology.getMaximumLongitude());

    minimumProjectedX = minimumPosition.getX();
    maximumProjectedY = maximumPosition.getY();

    const double projectedWidth =
        maximumPosition.getX() - minimumPosition.getX();
    const double projectedHeight =
        maximumPosition.getY() - minimumPosition.getY();
    scale = 1000.0
        / std::max({projectedWidth, projectedHeight, 0.000001});
}

QPointF MapViewport::mapPosition(double latitude, double longitude) const
{
    const Point projectedPosition =
        Geography::webMercator(latitude, longitude);
    return {
        (projectedPosition.getX() - minimumProjectedX) * scale,
        (maximumProjectedY - projectedPosition.getY()) * scale};
}
