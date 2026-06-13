#include "MapWidget.hpp"
#include "../MapViewport.hpp"
#include "EngineWidget.hpp"
#include "LinkWidget.hpp"
#include "NodeWidget.hpp"

#include <QColor>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleHints>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

MapWidget::MapWidget(const Topology &topology, const Garage &garage, QWidget *parent)
    : QWidget(parent),
      topology(topology),
      garage(garage)
{
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(QStringLiteral("#0b1118")));
    if (!topology.getError().isEmpty())
    {
        painter.setPen(QColor(QStringLiteral("#ff8f8f")));
        painter.drawText(rect(), Qt::AlignCenter, topology.getError());
    }
    else
        drawMap(painter);
}

void MapWidget::drawMap(QPainter &painter) const
{
    const MapViewport viewport(topology, width(), height(), Margin, zoomFactor, panOffset);
    LinkWidget::drawAll(painter, topology.getLinks(), viewport);
    NodeWidget::drawAll(painter, topology.getNodes(), viewport);
    EngineWidget::drawAll(painter, garage, selectedEngine, viewport);
}

const Engine *MapWidget::findEngineAt(const QPointF &position) const
{
    const Engine *closestEngine = nullptr;
    double closestDistance = SelectionRadius;
    const MapViewport viewport(topology, width(), height(), Margin, zoomFactor, panOffset);
    for (const Engine &engine : garage.getEngines())
    {
        if (engine.isActive())
        {
            QPointF enginePosition;
            if (EngineWidget(engine).setScreenPosition(viewport, enginePosition))
            {
                const double distance = std::hypot(position.x() - enginePosition.x(), position.y() - enginePosition.y());
                if (distance <= closestDistance)
                {
                    closestDistance = distance;
                    closestEngine = &engine;
                }
            }
        }
    }
    return closestEngine;
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isPanning = true;
        hasDragged = false;
        mousePressPosition = event->position();
        lastMousePosition = event->position();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isPanning)
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (!hasDragged
        && (event->position() - mousePressPosition).manhattanLength()
            >= QGuiApplication::styleHints()->startDragDistance())
    {
        hasDragged = true;
        setCursor(Qt::ClosedHandCursor);
    }

    if (hasDragged)
    {
        panOffset += event->position() - lastMousePosition;
        update();
    }
    lastMousePosition = event->position();
    event->accept();
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isPanning)
    {
        QWidget::mouseReleaseEvent(event);
    }
    else
    {
        isPanning = false;
        unsetCursor();
        if (!hasDragged)
        {
            selectedEngine = findEngineAt(event->position());
            update();
        }
        event->accept();
    }
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    const QPoint scrollDelta = event->pixelDelta().isNull() ? event->angleDelta() : event->pixelDelta();
    if (scrollDelta.y() == 0)
    {
        QWidget::wheelEvent(event);
        return;
    }
    const double step = event->pixelDelta().isNull() ? scrollDelta.y() / 120.0 : scrollDelta.y() / 40.0;
    const double oldZoom = zoomFactor;
    zoomFactor = std::clamp(zoomFactor * std::pow(1.2, step), MinimumZoom, MaximumZoom);
    const double appliedFactor = zoomFactor / oldZoom;
    const QPointF mapCenter(width() / 2.0, height() / 2.0);
    panOffset += (1.0 - appliedFactor) * (event->position() - mapCenter - panOffset);
    update();
    event->accept();
}
