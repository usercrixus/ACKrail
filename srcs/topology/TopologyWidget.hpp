#pragma once

#include "Topology.hpp"

#include <QPointF>
#include <QWidget>

class QMouseEvent;
class QPainter;
class QWheelEvent;

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(const Topology &topology, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    const double Margin = 70.0;
    const double HeaderHeight = 64.0;
    const double MinimumZoom = 0.25;
    const double MaximumZoom = 20.0;

    void drawHeader(QPainter &painter) const;
    void drawTopology(QPainter &painter) const;
    void drawLinks(QPainter &painter) const;
    void drawNodes(QPainter &painter) const;
    QPointF mapPosition(double latitude, double longitude) const;

    const Topology &topology;
    double zoomFactor = 1.0;
    QPointF panOffset;
    QPointF lastMousePosition;
    bool isPanning = false;
};
