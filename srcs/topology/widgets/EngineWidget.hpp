#pragma once

#include "../../garage/engine/Engine.hpp"
#include <QPointF>
#include <functional>

class QPainter;
class Garage;

class EngineWidget
{
public:
    EngineWidget(const Engine &engine);
    static void drawAll(QPainter &painter, const Garage &garage, const Engine *selectedEngine, const std::function<QPointF(double, double)> &mapPosition);
    void draw(QPainter &painter, const std::function<QPointF(double, double)> &mapPosition) const;
    void drawInformation(QPainter &painter, const QPointF &enginePosition) const;
    bool getScreenPosition(const std::function<QPointF(double, double)> &mapPosition, QPointF &position) const;

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
