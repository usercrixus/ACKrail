#pragma once

#include "../MapViewport.hpp"
#include "../Topology.hpp"
#include <QMatrix4x4>
#include <QPointF>
#include <QRectF>
#include <QSize>

class MapCamera
{
public:
    MapCamera(const Topology &topology, const MapViewport &mapViewport);

    void resize(int width, int height);
    void pan(const QPointF &delta);
    void zoomAt(const QPointF &screenPosition, double factor);

    QMatrix4x4 matrix() const;
    QPointF sceneToScreen(const QPointF &position) const;
    QPointF screenToScene(const QPointF &position) const;
    QSize viewportSize() const;

private:
    static constexpr double Margin = 70.0;
    static constexpr double MinimumZoom = 0.25;
    static constexpr double MaximumZoom = 80.0;

    void updateFitScale();

    QRectF contentBounds;
    QPointF cameraCenter;
    QPointF panOffset;
    double fitScale = 1.0;
    double zoomFactor = 1.0;
    int viewportWidth = 1;
    int viewportHeight = 1;
};
