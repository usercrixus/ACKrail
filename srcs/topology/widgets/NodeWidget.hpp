#pragma once

#include "../Node.hpp"
#include <QPointF>
#include <QVector>
#include <functional>

class QPainter;

class NodeWidget
{
public:
    NodeWidget(const Node &node);
    static void drawAll(QPainter &painter, const QVector<Node> &nodes, const std::function<QPointF(double, double)> &mapPosition);
    void draw(QPainter &painter, const std::function<QPointF(double, double)> &mapPosition) const;

private:
    const Node &node;
};
