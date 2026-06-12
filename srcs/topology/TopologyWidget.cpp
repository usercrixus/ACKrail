#include "TopologyWidget.hpp"
#include "../geography/Geography.hpp"
#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QTransform>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>
#include <numbers>

TopologyWidget::TopologyWidget(
    const Topology &topology,
    const Garage &garage,
    TrafficManager &trafficManager,
    TrafficGenerator &trafficGenerator,
    QWidget *parent)
    : QWidget(parent),
      topology(topology),
      garage(garage),
      trafficManager(trafficManager),
      trafficGenerator(trafficGenerator)
{
    setWindowTitle(QStringLiteral("ACKrail"));

    animationTimer.setInterval(16);
    connect(&animationTimer, &QTimer::timeout, this, [this]() {
        advanceTraffic();
    });
    animationClock.start();
    animationTimer.start();
}

void TopologyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(QStringLiteral("#0b1118")));
    drawHeader(painter);
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

void TopologyWidget::drawHeader(QPainter &painter) const
{
    painter.fillRect(QRectF(0, 0, width(), HeaderHeight), QColor(QStringLiteral("#111b25")));
    painter.setPen(QColor(QStringLiteral("#f4f7f9")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 16, QFont::DemiBold));
    painter.drawText(QRectF(24, 0, 100, HeaderHeight), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("ACKrail"));
    painter.setPen(QColor(QStringLiteral("#91a4b5")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 11));
    painter.drawText(QRectF(130, 0, width() - 154, HeaderHeight), Qt::AlignLeft | Qt::AlignVCenter, topology.getName());
    painter.drawText(
        QRectF(width() - 260, 0, 236, HeaderHeight),
        Qt::AlignRight | Qt::AlignVCenter,
        QStringLiteral("%1 / %2 engines active")
            .arg(static_cast<qulonglong>(garage.getActiveEngineCount()))
            .arg(static_cast<qulonglong>(garage.getEngines().size())));
}

void TopologyWidget::drawTopology(QPainter &painter) const
{
    painter.save();
    painter.setClipRect(QRectF(0, HeaderHeight, width(), height() - HeaderHeight));
    drawLinks(painter);
    drawNodes(painter);
    drawEngines(painter);
    painter.restore();
}

void TopologyWidget::drawLinks(QPainter &painter) const
{
    for (const Link &link : topology.getLinks())
    {
        const QPointF start = mapPosition(link.getFromNode().getLatitude(), link.getFromNode().getLongitude());
        const QPointF end = mapPosition(link.getToNode().getLatitude(), link.getToNode().getLongitude());
        painter.setPen(QPen(QColor(link.getColor()), 7, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(start, end);
    }
}

void TopologyWidget::drawNodes(QPainter &painter) const
{
    for (const Node &node : topology.getNodes())
    {
        const QPointF position = mapPosition(node.getLatitude(), node.getLongitude());
        painter.setPen(QPen(QColor(QStringLiteral("#f4f7f9")), 3));
        painter.setBrush(QColor(QStringLiteral("#101a24")));
        painter.drawEllipse(position, 7, 7);
        painter.setPen(QColor(QStringLiteral("#dce6ed")));
        painter.setFont(QFont(QStringLiteral("Sans Serif"), 8));
        painter.drawText(position + QPointF(11, 4), node.getName());
    }
}

void TopologyWidget::drawEngines(QPainter &painter) const
{
    for (const Engine &engine : garage.getEngines())
    {
        if (engine.isActive())
            drawEngine(painter, engine);
    }
}

void TopologyWidget::drawEngine(QPainter &painter, const Engine &engine) const
{
    EnginePosition positionStorage;
    if (!setEnginePosition(engine, positionStorage))
        return;

    const QPointF start = this->mapPosition(
        positionStorage.fromNode->getLatitude(),
        positionStorage.fromNode->getLongitude());
    const QPointF end = this->mapPosition(
        positionStorage.toNode->getLatitude(),
        positionStorage.toNode->getLongitude());
    const QPointF position =
        start + (end - start) * positionStorage.linkProgress;
    const double angleDegrees = std::atan2(end.y() - start.y(), end.x() - start.x()) * 180.0 / std::numbers::pi;

    painter.save();
    painter.translate(position);
    painter.rotate(angleDegrees);
    painter.setPen(QPen(QColor(QStringLiteral("#ffffff")), 2));
    painter.setBrush(QColor(QStringLiteral("#ff5c35")));
    painter.drawRoundedRect(QRectF(-11, -6, 22, 12), 5, 5);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QStringLiteral("#8fe3ff")));
    painter.drawRoundedRect(QRectF(1, -3.5, 6, 7), 2, 2);
    painter.restore();
}

bool TopologyWidget::setEnginePosition(const Engine &engine, EnginePosition &position) const
{
    const Route *trajectory = engine.getTrajectory();
    if (trajectory == nullptr)
        return false;

    const double travelledDistance = engine.getAverageSpeedKilometersPerHour() * engine.getElapsedTrajectorySeconds() / 3600.0;
    double distanceBeforeLink = 0.0;
    const QVector<const Link *> &links = trajectory->getLinks();
    const QVector<Node> &stations = trajectory->getStations();
    for (qsizetype index = 0; index < links.size(); ++index)
    {
        const double linkDistance = links[index]->getDistanceKilometers();
        if (travelledDistance < distanceBeforeLink + linkDistance)
        {
            position.fromNode = &stations[index];
            position.toNode = &stations[index + 1];
            position.linkProgress = (travelledDistance - distanceBeforeLink) / linkDistance;
            return true;
        }
        distanceBeforeLink += linkDistance;
    }
    return false;
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
    const double usableHeight = std::max(1.0, height() - HeaderHeight - Margin * 2.0);
    const double scale = std::min(usableWidth / std::max(projectedWidth, 0.000001), usableHeight / std::max(projectedHeight, 0.000001));
    const double mapWidth = projectedWidth * scale;
    const double mapHeight = projectedHeight * scale;
    const double left = (width() - mapWidth) / 2.0;
    const double top = HeaderHeight + (height() - HeaderHeight - mapHeight) / 2.0;
    const QPointF fittedPosition{
        left + (projectedPosition.getX() - minimumPosition.getX()) * scale,
        top + (maximumPosition.getY() - projectedPosition.getY()) * scale};
    const QPointF mapCenter(width() / 2.0, HeaderHeight + (height() - HeaderHeight) / 2.0);
    return mapCenter + (fittedPosition - mapCenter) * zoomFactor + panOffset;
}

void TopologyWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() >= HeaderHeight)
    {
        isPanning = true;
        lastMousePosition = event->position();
        setCursor(Qt::ClosedHandCursor);
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
        panOffset += event->position() - lastMousePosition;
        lastMousePosition = event->position();
        update();
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
    if (event->position().y() > HeaderHeight && scrollDelta.y() != 0)
    {
        const double step = event->pixelDelta().isNull() ? scrollDelta.y() / 120.0 : scrollDelta.y() / 40.0;
        const double oldZoom = zoomFactor;
        zoomFactor = std::clamp(zoomFactor * std::pow(1.2, step), MinimumZoom, MaximumZoom);
        const double appliedFactor = zoomFactor / oldZoom;
        const QPointF mapCenter(width() / 2.0, HeaderHeight + (height() - HeaderHeight) / 2.0);
        panOffset += (1.0 - appliedFactor) * (event->position() - mapCenter - panOffset);
        update();
        event->accept();
    }
    else
    {
        QWidget::wheelEvent(event);
    }
}
