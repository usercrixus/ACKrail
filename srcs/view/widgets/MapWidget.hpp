#pragma once

#include "EngineRenderer.hpp"
#include "LinkRenderer.hpp"
#include "../helper/MapCamera.hpp"
#include "NodeRenderer.hpp"
#include "../../garage/Garage.hpp"
#include "../../simulator/TrafficManager.hpp"
#include "../helper/MapViewport.hpp"
#include "../../topology/Topology.hpp"
#include <QMouseEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>
#include <QWheelEvent>

class MapWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
public:
    explicit MapWidget(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, QWidget *parent = nullptr);
    void refresh(double simulationTimeSeconds);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawOverlay();

    const Topology &topology;
    MapViewport mapViewport;
    MapCamera camera;
    LinkRenderer linkRenderer;
    NodeRenderer nodeRenderer;
    EngineRenderer engineRenderer;
    double simulationTimeSeconds = 0.0;
    QPoint mousePressPosition;
    QPoint lastMousePosition;
    bool isLeftButtonPressed = false;
    bool isDragging = false;
};
