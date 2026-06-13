#pragma once

#include "../../garage/Garage.hpp"
#include "../../garage/engine/Engine.hpp"
#include "../MapViewport.hpp"
#include <QPainter>
#include <QPointF>

class EngineWidget
{
public:
    EngineWidget(const Engine &engine);
    void draw(QPainter &painter, const MapViewport &viewport) const;
    void drawInformation(QPainter &painter, const QPointF &enginePosition) const;
    bool setScreenPosition(const MapViewport &viewport, QPointF &position) const;
    static void drawAll(QPainter &painter, const Garage &garage, const Engine *selectedEngine, const MapViewport &viewport);

private:
    struct Position
    {
        const Node *fromNode;
        const Node *toNode;
        double linkProgress;
    };
    bool setPosition(Position &position) const;
    const Engine &engine;
};
