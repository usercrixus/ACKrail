#include "TopologyWidget.hpp"
#include "../geography/Geography.hpp"
#include "widgets/EngineWidget.hpp"
#include "widgets/HeaderWidget.hpp"
#include "widgets/LinkWidget.hpp"
#include "widgets/NodeWidget.hpp"
#include <QColor>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleHints>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

TopologyWidget::TopologyWidget(const Topology &topology, const Garage &garage, TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, QWidget *parent)
    : QWidget(parent),
      topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator)
{
    setWindowTitle(QStringLiteral("ACKrail"));
    animationTimer.setInterval(16);
    connect(&animationTimer, &QTimer::timeout, this, [this]()
            { advanceTraffic(); });
    animationClock.start();
    animationTimer.start();
}

void TopologyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(QStringLiteral("#0b1118")));
    HeaderWidget::draw(painter, topology, garage, width());
    if (!topology.getError().isEmpty())
    {
        painter.setPen(QColor(QStringLiteral("#ff8f8f")));
        painter.drawText(rect(), Qt::AlignCenter, topology.getError());
    }
    else
    {
        drawTopology(painter);
    }
}

void TopologyWidget::drawTopology(QPainter &painter) const
{
    const auto positionMapper = [this](double latitude, double longitude)
    {
        return mapPosition(latitude, longitude);
    };

    painter.save();
    painter.setClipRect(QRectF(0, HeaderWidget::Height, width(), height() - HeaderWidget::Height));
    LinkWidget::drawAll(painter, topology.getLinks(), positionMapper);
    NodeWidget::drawAll(painter, topology.getNodes(), positionMapper);
    EngineWidget::drawAll(
        painter,
        garage,
        selectedEngine,
        positionMapper);
    painter.restore();
}

const Engine *TopologyWidget::findEngineAt(const QPointF &position) const
{
    constexpr double SelectionRadius = 14.0;
    const Engine *closestEngine = nullptr;
    double closestDistance = SelectionRadius;
    const auto positionMapper = [this](double latitude, double longitude)
    {
        return mapPosition(latitude, longitude);
    };

    for (const Engine &engine : garage.getEngines())
    {
        if (!engine.isActive())
            continue;

        QPointF enginePosition;
        if (!EngineWidget(engine).getScreenPosition(positionMapper, enginePosition))
            continue;

        const double distance = std::hypot(
            position.x() - enginePosition.x(),
            position.y() - enginePosition.y());
        if (distance <= closestDistance)
        {
            closestDistance = distance;
            closestEngine = &engine;
        }
    }

    return closestEngine;
}

void TopologyWidget::advanceTraffic()
{
    const qint64 elapsedMilliseconds = animationClock.restart();
    if (elapsedMilliseconds <= 0)
        return;

    const double elapsedSeconds = static_cast<double>(elapsedMilliseconds) / 1000.0;
    trafficManager.advance(elapsedSeconds);
    trafficGenerator.tryGenerate(elapsedSeconds);
    update();
}

QPointF TopologyWidget::mapPosition(double latitude, double longitude) const
{
    const Point minimumPosition = Geography::webMercator(topology.getMinimumLatitude(), topology.getMinimumLongitude());
    const Point maximumPosition = Geography::webMercator(topology.getMaximumLatitude(), topology.getMaximumLongitude());
    const Point projectedPosition = Geography::webMercator(latitude, longitude);
    const double projectedWidth = maximumPosition.getX() - minimumPosition.getX();
    const double projectedHeight = maximumPosition.getY() - minimumPosition.getY();
    const double usableWidth = std::max(1.0, width() - Margin * 2.0);
    const double usableHeight = std::max(1.0, height() - HeaderWidget::Height - Margin * 2.0);
    const double scale = std::min(usableWidth / std::max(projectedWidth, 0.000001), usableHeight / std::max(projectedHeight, 0.000001));
    const double mapWidth = projectedWidth * scale;
    const double mapHeight = projectedHeight * scale;
    const double left = (width() - mapWidth) / 2.0;
    const double top = HeaderWidget::Height + (height() - HeaderWidget::Height - mapHeight) / 2.0;
    const QPointF fittedPosition{left + (projectedPosition.getX() - minimumPosition.getX()) * scale, top + (maximumPosition.getY() - projectedPosition.getY()) * scale};
    const QPointF mapCenter(width() / 2.0, HeaderWidget::Height + (height() - HeaderWidget::Height) / 2.0);
    return mapCenter + (fittedPosition - mapCenter) * zoomFactor + panOffset;
}

void TopologyWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() >= HeaderWidget::Height)
    {
        isPanning = true;
        hasDragged = false;
        mousePressPosition = event->position();
        lastMousePosition = event->position();
        event->accept();
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}

void TopologyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning)
    {
        if (!hasDragged && (event->position() - mousePressPosition).manhattanLength() >= QGuiApplication::styleHints()->startDragDistance())
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
    else
    {
        QWidget::mouseMoveEvent(event);
    }
}

void TopologyWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isPanning)
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
    else
    {
        QWidget::mouseReleaseEvent(event);
    }
}

void TopologyWidget::wheelEvent(QWheelEvent *event)
{
    const QPoint scrollDelta = event->pixelDelta().isNull() ? event->angleDelta() : event->pixelDelta();
    if (event->position().y() > HeaderWidget::Height && scrollDelta.y() != 0)
    {
        const double step = event->pixelDelta().isNull() ? scrollDelta.y() / 120.0 : scrollDelta.y() / 40.0;
        const double oldZoom = zoomFactor;
        zoomFactor = std::clamp(zoomFactor * std::pow(1.2, step), MinimumZoom, MaximumZoom);
        const double appliedFactor = zoomFactor / oldZoom;
        const QPointF mapCenter(width() / 2.0, HeaderWidget::Height + (height() - HeaderWidget::Height) / 2.0);
        panOffset += (1.0 - appliedFactor) * (event->position() - mapCenter - panOffset);
        update();
        event->accept();
    }
    else
    {
        QWidget::wheelEvent(event);
    }
}
