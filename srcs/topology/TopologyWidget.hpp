#pragma once

#include "Topology.hpp"

#include <QPointF>
#include <QWidget>

class QMouseEvent;
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
    QPointF mapPosition(double latitude, double longitude) const;
    const Topology &topology;
    double zoomFactor = 1.0;
    QPointF panOffset;
    QPointF lastMousePosition;
    bool isPanning = false;
};
