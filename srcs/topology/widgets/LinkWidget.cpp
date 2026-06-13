#include "LinkWidget.hpp"

#include <QColor>
#include <QPainter>
#include <QPen>

LinkWidget::LinkWidget(const Link &link)
    : link(link)
{
}

void LinkWidget::drawAll(QPainter &painter, const QVector<Link> &links, const std::function<QPointF(double, double)> &mapPosition)
{
    for (const Link &link : links)
        LinkWidget(link).draw(painter, mapPosition);
}

void LinkWidget::draw(QPainter &painter, const std::function<QPointF(double, double)> &mapPosition) const
{
    const QPointF start = mapPosition(link.getFromNode().getLatitude(), link.getFromNode().getLongitude());
    const QPointF end = mapPosition(link.getToNode().getLatitude(), link.getToNode().getLongitude());
    painter.setPen(QPen(QColor(link.getColor()), 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(start, end);
}
