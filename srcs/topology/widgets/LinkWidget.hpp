#pragma once

#include "../Link.hpp"
#include "../MapViewport.hpp"
#include <QGraphicsLineItem>

class LinkWidget : public QGraphicsLineItem
{
public:
    LinkWidget(const Link &link, const MapViewport &viewport);
};
