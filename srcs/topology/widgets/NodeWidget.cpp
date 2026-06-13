#include "NodeWidget.hpp"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

NodeWidget::NodeWidget(const Node &node, const MapViewport &viewport)
    : node(node)
{
    setPos(viewport.mapPosition(node.getLatitude(), node.getLongitude()));
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(1);
}

QRectF NodeWidget::boundingRect() const
{
    return QRectF(-9, -9, 220, 22);
}

void NodeWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(QStringLiteral("#f4f7f9")), 3));
    painter->setBrush(QColor(QStringLiteral("#101a24")));
    painter->drawEllipse(QPointF(0, 0), 7, 7);
    painter->setPen(QColor(QStringLiteral("#dce6ed")));
    painter->setFont(QFont(QStringLiteral("Sans Serif"), 8));
    painter->drawText(QPointF(11, 4), node.getName());
}
