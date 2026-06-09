#include "TopologyWidget.hpp"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
    constexpr double margin = 70.0;
    constexpr double headerHeight = 64.0;
}

TopologyWidget::TopologyWidget(const Topology &topology, QWidget *parent)
    : QWidget(parent),
      topology(topology)
{
    setMinimumSize(760, 520);
    setWindowTitle(QStringLiteral("ACKrail Topology"));
}

void TopologyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(QStringLiteral("#0b1118")));
    painter.fillRect(QRectF(0, 0, width(), headerHeight), QColor(QStringLiteral("#111b25")));

    painter.setPen(QColor(QStringLiteral("#f4f7f9")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 16, QFont::DemiBold));
    painter.drawText(QPointF(24, 40), QStringLiteral("ACKrail"));

    painter.setPen(QColor(QStringLiteral("#91a4b5")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 11));
    painter.drawText(QPointF(130, 39), topology.getName());

    if (!topology.getError().isEmpty())
    {
        painter.setPen(QColor(QStringLiteral("#ff8f8f")));
        painter.drawText(rect(), Qt::AlignCenter, topology.getError());
        return;
    }

    painter.setPen(Qt::NoPen);
    for (const Link &link : topology.getLinks())
    {
        const QPointF start = mapPosition(
            link.getFromNode().getLatitude(),
            link.getFromNode().getLongitude());
        const QPointF end = mapPosition(
            link.getToNode().getLatitude(),
            link.getToNode().getLongitude());

        painter.setPen(QPen(QColor(link.getColor()), 7, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(start, end);
    }

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

QPointF TopologyWidget::mapPosition(double latitude, double longitude) const
{
    const double longitudeScale = std::cos(48.86 * std::numbers::pi / 180.0);
    const double geographicWidth =
        (topology.getMaximumLongitude() - topology.getMinimumLongitude()) * longitudeScale;
    const double geographicHeight =
        topology.getMaximumLatitude() - topology.getMinimumLatitude();
    const double usableWidth = std::max(1.0, width() - margin * 2.0);
    const double usableHeight = std::max(1.0, height() - headerHeight - margin * 2.0);
    const double scale = std::min(
        usableWidth / std::max(geographicWidth, 0.000001),
        usableHeight / std::max(geographicHeight, 0.000001));
    const double mapWidth = geographicWidth * scale;
    const double mapHeight = geographicHeight * scale;
    const double left = (width() - mapWidth) / 2.0;
    const double top = headerHeight + (height() - headerHeight - mapHeight) / 2.0;

    return {
        left + (longitude - topology.getMinimumLongitude()) * longitudeScale * scale,
        top + (topology.getMaximumLatitude() - latitude) * scale};
}
