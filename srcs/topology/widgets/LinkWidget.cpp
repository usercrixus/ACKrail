#include "LinkWidget.hpp"
#include "../MapViewport.hpp"

#include <QColor>
#include <QPainter>
#include <QPen>

LinkWidget::LinkWidget(const Link &link)
    : link(link)
{
}

void LinkWidget::drawAll(QPainter &painter, const QVector<Link> &links, const MapViewport &viewport)
{
    for (const Link &link : links)
        LinkWidget(link).draw(painter, viewport);
}

void LinkWidget::draw(QPainter &painter, const MapViewport &viewport) const
{
    const QPointF start = viewport.mapPosition(link.getFromNode().getLatitude(), link.getFromNode().getLongitude());
    const QPointF end = viewport.mapPosition(link.getToNode().getLatitude(), link.getToNode().getLongitude());
    painter.setPen(QPen(QColor(link.getColor()), 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(start, end);
}
