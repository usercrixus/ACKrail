#pragma once

#include "../../garage/Garage.hpp"
#include "../Topology.hpp"
#include <QPointF>
#include <QWidget>

class Engine;
class QMouseEvent;
class QPaintEvent;
class QPainter;
class QWheelEvent;

class MapWidget : public QWidget
{
public:
    explicit MapWidget(const Topology &topology, const Garage &garage, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    static constexpr double Margin = 70.0;
    static constexpr double MinimumZoom = 0.25;
    static constexpr double MaximumZoom = 20.0;
    static constexpr double SelectionRadius = 14.0;

    void drawMap(QPainter &painter) const;
    const Engine *findEngineAt(const QPointF &position) const;

    const Topology &topology;
    const Garage &garage;
    double zoomFactor = 1.0;
    QPointF panOffset;
    QPointF lastMousePosition;
    QPointF mousePressPosition;
    const Engine *selectedEngine = nullptr;
    bool isPanning = false;
    bool hasDragged = false;
};
