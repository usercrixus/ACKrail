#include "NodeWidget.hpp"
#include "../MapViewport.hpp"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>

NodeWidget::NodeWidget(const Node &node)
    : node(node)
{
}

void NodeWidget::drawAll(QPainter &painter, const QVector<Node> &nodes, const MapViewport &viewport)
{
    for (const Node &node : nodes)
        NodeWidget(node).draw(painter, viewport);
}

void NodeWidget::draw(QPainter &painter, const MapViewport &viewport) const
{
    const QPointF position = viewport.mapPosition(node.getLatitude(), node.getLongitude());
    painter.setPen(QPen(QColor(QStringLiteral("#f4f7f9")), 3));
    painter.setBrush(QColor(QStringLiteral("#101a24")));
    painter.drawEllipse(position, 7, 7);
    painter.setPen(QColor(QStringLiteral("#dce6ed")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 8));
    painter.drawText(position + QPointF(11, 4), node.getName());
}
