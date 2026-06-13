#pragma once

#include "../../garage/engine/Engine.hpp"
#include "../MapViewport.hpp"
#include <QGraphicsItem>

class EngineWidget : public QGraphicsItem
{
public:
    EngineWidget(const Engine &engine, const MapViewport &viewport);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void updatePosition();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    static constexpr double NormalZValue = 2.0;
    static constexpr double SelectedZValue = 1000.0;

    struct Position
    {
        const Node *fromNode;
        const Node *toNode;
        double linkProgress;
    };

    bool setPosition(Position &position) const;
    void drawEngine(QPainter &painter) const;
    void drawInformation(QPainter &painter) const;

    const Engine &engine;
    const MapViewport &viewport;
    double angleDegrees = 0.0;
};
