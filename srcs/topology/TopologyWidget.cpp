#include "TopologyWidget.hpp"

#include "../geography/Geography.hpp"

#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

TopologyWidget::TopologyWidget(const Topology &topology, QWidget *parent)
    : QWidget(parent),
      topology(topology)
{
    setWindowTitle(QStringLiteral("ACKrail"));
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
}

void TopologyWidget::drawTopology(QPainter &painter) const
{
    painter.save();
    painter.setClipRect(QRectF(0, HeaderHeight, width(), height() - HeaderHeight));
    drawLinks(painter);
    drawNodes(painter);
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

void TopologyWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() >= HeaderHeight)
    {
        isPanning = true;
        lastMousePosition = event->position();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void TopologyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning)
    {
        panOffset += event->position() - lastMousePosition;
        lastMousePosition = event->position();
        update();
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void TopologyWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isPanning)
    {
        isPanning = false;
        unsetCursor();
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void TopologyWidget::wheelEvent(QWheelEvent *event)
{
    if (event->position().y() < HeaderHeight)
    {
        QWidget::wheelEvent(event);
        return;
    }

    const QPoint scrollDelta =
        event->pixelDelta().isNull() ? event->angleDelta() : event->pixelDelta();
    if (scrollDelta.y() == 0)
    {
        QWidget::wheelEvent(event);
        return;
    }

    const double step = event->pixelDelta().isNull() ? scrollDelta.y() / 120.0
                                                     : scrollDelta.y() / 40.0;
    const double oldZoom = zoomFactor;
    zoomFactor = std::clamp(
        zoomFactor * std::pow(1.2, step),
        MinimumZoom,
        MaximumZoom);

    const double appliedFactor = zoomFactor / oldZoom;
    const QPointF mapCenter(
        width() / 2.0,
        HeaderHeight + (height() - HeaderHeight) / 2.0);
    panOffset +=
        (1.0 - appliedFactor) * (event->position() - mapCenter - panOffset);

    update();
    event->accept();
}

QPointF TopologyWidget::mapPosition(double latitude, double longitude) const
{
    const Geography::ProjectedPosition minimumPosition =
        Geography::webMercator(
            topology.getMinimumLatitude(),
            topology.getMinimumLongitude());
    const Geography::ProjectedPosition maximumPosition =
        Geography::webMercator(
            topology.getMaximumLatitude(),
            topology.getMaximumLongitude());
    const Geography::ProjectedPosition projectedPosition =
        Geography::webMercator(latitude, longitude);

    const double projectedWidth = maximumPosition.x - minimumPosition.x;
    const double projectedHeight = maximumPosition.y - minimumPosition.y;
    const double usableWidth = std::max(1.0, width() - Margin * 2.0);
    const double usableHeight = std::max(1.0, height() - HeaderHeight - Margin * 2.0);
    const double scale = std::min(usableWidth / std::max(projectedWidth, 0.000001), usableHeight / std::max(projectedHeight, 0.000001));
    const double mapWidth = projectedWidth * scale;
    const double mapHeight = projectedHeight * scale;
    const double left = (width() - mapWidth) / 2.0;
    const double top = HeaderHeight + (height() - HeaderHeight - mapHeight) / 2.0;

    const QPointF fittedPosition{
        left + (projectedPosition.x - minimumPosition.x) * scale,
        top + (maximumPosition.y - projectedPosition.y) * scale};
    const QPointF mapCenter(
        width() / 2.0,
        HeaderHeight + (height() - HeaderHeight) / 2.0);

    return mapCenter + (fittedPosition - mapCenter) * zoomFactor + panOffset;
}
