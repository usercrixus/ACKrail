#pragma once

#include "../garage/Garage.hpp"
#include "../simulator/TrafficGenerator.hpp"
#include "../simulator/TrafficManager.hpp"
#include "Topology.hpp"
#include <QElapsedTimer>
#include <QPointF>
#include <QTimer>
#include <QWidget>

class QMouseEvent;
class QPainter;
class QWheelEvent;

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(
        const Topology &topology,
        const Garage &garage,
        TrafficManager &trafficManager,
        TrafficGenerator &trafficGenerator,
        QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    const double Margin = 70.0;
    const double MinimumZoom = 0.25;
    const double MaximumZoom = 20.0;

    void drawTopology(QPainter &painter) const;
    const Engine *findEngineAt(const QPointF &position) const;
    void advanceTraffic();
    QPointF mapPosition(double latitude, double longitude) const;

    const Topology &topology;
    const Garage &garage;
    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    QTimer animationTimer;
    QElapsedTimer animationClock;
    double zoomFactor = 1.0;
    QPointF panOffset;
    QPointF lastMousePosition;
    QPointF mousePressPosition;
    const Engine *selectedEngine = nullptr;
    bool isPanning = false;
    bool hasDragged = false;
};
