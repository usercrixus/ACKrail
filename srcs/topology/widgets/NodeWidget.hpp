#pragma once

#include "../MapViewport.hpp"
#include "../Node.hpp"
#include <QGraphicsItem>

class NodeWidget : public QGraphicsItem
{
public:
    NodeWidget(const Node &node, const MapViewport &viewport);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    const Node &node;
};
