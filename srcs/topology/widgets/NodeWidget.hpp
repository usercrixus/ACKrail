#pragma once

#include "../Node.hpp"
#include <QVector>

class MapViewport;
class QPainter;

class NodeWidget
{
public:
    NodeWidget(const Node &node);
    static void drawAll(QPainter &painter, const QVector<Node> &nodes, const MapViewport &viewport);
    void draw(QPainter &painter, const MapViewport &viewport) const;

private:
    const Node &node;
};
