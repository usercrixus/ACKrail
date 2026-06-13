#include "EngineWidget.hpp"

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <algorithm>
#include <cmath>
#include <numbers>

EngineWidget::EngineWidget(const Engine &engine, const MapViewport &viewport)
    : engine(engine),
      viewport(viewport)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(NormalZValue);
    updatePosition();
}

QRectF EngineWidget::boundingRect() const
{
    return isSelected() ? QRectF(-14, -170, 300, 190) : QRectF(-14, -9, 28, 18);
}

QPainterPath EngineWidget::shape() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(-14, -9, 28, 18), 5, 5);
    return path;
}

void EngineWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    drawEngine(*painter);
    if (isSelected())
        drawInformation(*painter);
}

void EngineWidget::updatePosition()
{
    Position routePosition;
    if (!setPosition(routePosition))
    {
        setVisible(false);
        setSelected(false);
    }
    else
    {
        const QPointF start = viewport.mapPosition(routePosition.fromNode->getLatitude(), routePosition.fromNode->getLongitude());
        const QPointF end = viewport.mapPosition(routePosition.toNode->getLatitude(), routePosition.toNode->getLongitude());
        angleDegrees = std::atan2(end.y() - start.y(), end.x() - start.x()) * 180.0 / std::numbers::pi;
        setPos(start + (end - start) * routePosition.linkProgress);
        setVisible(true);
        update();
    }
}

QVariant EngineWidget::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        prepareGeometryChange();
        setZValue(value.toBool() ? SelectedZValue : NormalZValue);
    }
    return QGraphicsItem::itemChange(change, value);
}

void EngineWidget::drawEngine(QPainter &painter) const
{
    painter.save();
    painter.rotate(angleDegrees);
    painter.setPen(QPen(QColor(QStringLiteral("#ffffff")), 2));
    painter.setBrush(QColor(QStringLiteral("#ff5c35")));
    painter.drawRoundedRect(QRectF(-11, -6, 22, 12), 5, 5);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QStringLiteral("#8fe3ff")));
    painter.drawRoundedRect(QRectF(1, -3.5, 6, 7), 2, 2);
    painter.restore();
}

void EngineWidget::drawInformation(QPainter &painter) const
{
    const Route *route = engine.getTrajectory();
    if (route != nullptr && !route->getStations().isEmpty())
    {
        const QVector<Node> &stations = route->getStations();
        const double totalDistance = route->getTotalDistanceKilometers();
        const double travelledDistance = std::clamp(engine.getAverageSpeedKilometersPerHour() * engine.getElapsedTrajectorySeconds() / 3600.0, 0.0, totalDistance);
        const double progress = totalDistance > 0.0 ? travelledDistance / totalDistance * 100.0 : 0.0;
        const QString information = QStringLiteral(
                                        "%1\n"
                                        "%2 -> %3\n"
                                        "Speed: %4 km/h\n"
                                        "Distance: %5 / %6 km\n"
                                        "Progress: %7%")
                                        .arg(engine.getModelName())
                                        .arg(stations.first().getName())
                                        .arg(stations.last().getName())
                                        .arg(engine.getCurrentSpeedKilometersPerHour(), 0, 'f', 1)
                                        .arg(travelledDistance, 0, 'f', 2)
                                        .arg(totalDistance, 0, 'f', 2)
                                        .arg(progress, 0, 'f', 1);
        const QFont font(QStringLiteral("Sans Serif"), 9);
        const QFontMetrics metrics(font);
        QRectF panel = metrics.boundingRect(QRect(0, 0, 280, 200), Qt::AlignLeft | Qt::AlignTop, information);
        panel.adjust(-10, -8, 10, 8);
        panel.moveTopLeft(QPointF(16, -panel.height() - 12));
        painter.save();
        painter.setFont(font);
        painter.setPen(QPen(QColor(QStringLiteral("#ff8a65")), 1.5));
        painter.setBrush(QColor(QStringLiteral("#17232f")));
        painter.drawRoundedRect(panel, 6, 6);
        painter.setPen(QColor(QStringLiteral("#f4f7f9")));
        painter.drawText(panel.adjusted(10, 8, -10, -8), Qt::AlignLeft | Qt::AlignTop, information);
        painter.restore();
    }
}

bool EngineWidget::setPosition(Position &position) const
{
    const Route *trajectory = engine.getTrajectory();
    if (trajectory != nullptr)
    {
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
    }
    return false;
}
