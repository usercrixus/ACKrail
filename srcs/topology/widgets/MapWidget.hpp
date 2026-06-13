#pragma once

#include "../../garage/Garage.hpp"
#include "../MapViewport.hpp"
#include "../Topology.hpp"
#include "EngineWidget.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <vector>

class MapWidget : public QGraphicsView
{
public:
    explicit MapWidget(const Topology &topology, const Garage &garage, QWidget *parent = nullptr);
    void refresh();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    static constexpr double SceneMargin = 250.0;
    void createScene();
    void fitScene();
    const Topology &topology;
    const Garage &garage;
    MapViewport mapViewport;
    QGraphicsScene graphicsScene;
    std::vector<EngineWidget *> engineWidgets;
    QPoint mousePressPosition;
    bool hasFittedScene = false;
    bool isLeftButtonPressed = false;
    bool isDragging = false;
};
