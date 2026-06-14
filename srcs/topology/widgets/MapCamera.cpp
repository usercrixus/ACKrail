#include "MapCamera.hpp"

#include <algorithm>

MapCamera::MapCamera(const Topology &topology, const MapViewport &mapViewport)
{
    for (const Node &node : topology.getNodes())
    {
        const QPointF position = mapViewport.mapPosition(node.getLatitude(), node.getLongitude());
        contentBounds |= QRectF(position, QSizeF(1, 1));
    }
    cameraCenter = contentBounds.center();
}

void MapCamera::resize(int width, int height)
{
    viewportWidth = std::max(1, width);
    viewportHeight = std::max(1, height);
    updateFitScale();
}

void MapCamera::pan(const QPointF &delta)
{
    panOffset += delta;
}

void MapCamera::zoomAt(const QPointF &screenPosition, double factor)
{
    const QPointF anchorScene = screenToScene(screenPosition);
    zoomFactor = std::clamp(zoomFactor * factor, MinimumZoom, MaximumZoom);
    const double displayScale = fitScale * zoomFactor;
    panOffset = screenPosition - QPointF(viewportWidth / 2.0, viewportHeight / 2.0) - (anchorScene - cameraCenter) * displayScale;
}

QMatrix4x4 MapCamera::matrix() const
{
    const double displayScale = fitScale * zoomFactor;
    QMatrix4x4 result;
    result.setToIdentity();
    result(0, 0) = static_cast<float>(2.0 * displayScale / viewportWidth);
    result(1, 1) = static_cast<float>(-2.0 * displayScale / viewportHeight);
    result(0, 3) = static_cast<float>(-cameraCenter.x() * result(0, 0) + 2.0 * panOffset.x() / viewportWidth);
    result(1, 3) = static_cast<float>(-cameraCenter.y() * result(1, 1) - 2.0 * panOffset.y() / viewportHeight);
    return result;
}

QPointF MapCamera::sceneToScreen(const QPointF &position) const
{
    const double displayScale = fitScale * zoomFactor;
    return QPointF(viewportWidth / 2.0, viewportHeight / 2.0) + (position - cameraCenter) * displayScale + panOffset;
}

QPointF MapCamera::screenToScene(const QPointF &position) const
{
    const double displayScale = fitScale * zoomFactor;
    return cameraCenter + (position - QPointF(viewportWidth / 2.0, viewportHeight / 2.0) - panOffset) / displayScale;
}

QSize MapCamera::viewportSize() const
{
    return QSize(viewportWidth, viewportHeight);
}

void MapCamera::updateFitScale()
{
    if (contentBounds.isEmpty())
        return;
    fitScale = std::min(
        std::max(1.0, viewportWidth - Margin * 2.0) / std::max(1.0, contentBounds.width()),
        std::max(1.0, viewportHeight - Margin * 2.0) / std::max(1.0, contentBounds.height()));
}
