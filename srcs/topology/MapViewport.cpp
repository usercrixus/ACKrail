#include "MapViewport.hpp"
#include "../geography/Geography.hpp"

#include <algorithm>

MapViewport::MapViewport(
    const Topology &topology,
    double width,
    double height,
    double margin,
    double zoomFactor,
    const QPointF &panOffset)
    : zoomFactor(zoomFactor),
      mapCenter(width / 2.0, height / 2.0),
      panOffset(panOffset)
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
    const double usableWidth = std::max(1.0, width - margin * 2.0);
    const double usableHeight = std::max(1.0, height - margin * 2.0);

    scale = std::min(
        usableWidth / std::max(projectedWidth, 0.000001),
        usableHeight / std::max(projectedHeight, 0.000001));

    const double mapWidth = projectedWidth * scale;
    const double mapHeight = projectedHeight * scale;
    left = (width - mapWidth) / 2.0;
    top = (height - mapHeight) / 2.0;
}

QPointF MapViewport::mapPosition(double latitude, double longitude) const
{
    const Point projectedPosition =
        Geography::webMercator(latitude, longitude);
    const QPointF fittedPosition(
        left + (projectedPosition.getX() - minimumProjectedX) * scale,
        top + (maximumProjectedY - projectedPosition.getY()) * scale);

    return mapCenter
        + (fittedPosition - mapCenter) * zoomFactor
        + panOffset;
}
