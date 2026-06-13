#pragma once

#include "../Link.hpp"
#include <QPointF>
#include <QVector>
#include <functional>

class QPainter;

class LinkWidget
{
public:
    LinkWidget(const Link &link);
    static void drawAll(QPainter &painter, const QVector<Link> &links, const std::function<QPointF(double, double)> &mapPosition);
    void draw(QPainter &painter, const std::function<QPointF(double, double)> &mapPosition) const;

private:
    const Link &link;
};
