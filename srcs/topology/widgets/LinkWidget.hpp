#pragma once

#include "../Link.hpp"
#include "../MapViewport.hpp"
#include <QPainter>
#include <QVector>

class LinkWidget
{
public:
    LinkWidget(const Link &link);
    static void drawAll(QPainter &painter, const QVector<Link> &links, const MapViewport &viewport);
    void draw(QPainter &painter, const MapViewport &viewport) const;

private:
    const Link &link;
};
