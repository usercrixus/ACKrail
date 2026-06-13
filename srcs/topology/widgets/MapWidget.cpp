#include "MapWidget.hpp"
#include "LinkWidget.hpp"
#include "NodeWidget.hpp"

#include <QColor>
#include <QEasingCurve>
#include <QGuiApplication>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleHints>
#include <algorithm>
#include <cmath>

MapWidget::MapWidget(const Topology &topology, const Garage &garage, QWidget *parent)
    : QGraphicsView(parent),
      topology(topology),
      garage(garage),
      mapViewport(topology)
{
    setScene(&graphicsScene);
    setBackgroundBrush(QColor(QStringLiteral("#0b1118")));
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    viewport()->setCursor(Qt::ArrowCursor);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsScene.setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    zoomAnimation.setDuration(500);
    zoomAnimation.setEasingCurve(QEasingCurve::OutCubic);
    connect(
        &zoomAnimation,
        &QVariantAnimation::valueChanged,
        this,
        [this](const QVariant &value)
        {
            setAnimatedZoom(value.toDouble());
        });
    createScene();
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isLeftButtonPressed = true;
        isDragging = false;
        mousePressPosition = event->position().toPoint();
    }
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isLeftButtonPressed && !isDragging)
    {
        const QPoint currentPosition = event->position().toPoint();
        if ((currentPosition - mousePressPosition).manhattanLength()
            >= QGuiApplication::styleHints()->startDragDistance())
            isDragging = true;
    }

    QGraphicsView::mouseMoveEvent(event);
    viewport()->setCursor(
        isDragging ? Qt::ClosedHandCursor : Qt::ArrowCursor);
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton && isLeftButtonPressed)
    {
        isLeftButtonPressed = false;
        isDragging = false;
    }
    viewport()->setCursor(Qt::ArrowCursor);
}

void MapWidget::refresh()
{
    for (EngineWidget *engineWidget : engineWidgets)
        engineWidget->updatePosition();
    QGraphicsView::viewport()->update();
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (!hasFittedScene)
    {
        fitScene();
        hasFittedScene = true;
    }
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    const QPoint scrollDelta = event->pixelDelta().isNull()
        ? event->angleDelta()
        : event->pixelDelta();
    if (scrollDelta.y() == 0)
    {
        QGraphicsView::wheelEvent(event);
        return;
    }

    const double step = event->pixelDelta().isNull()
        ? scrollDelta.y() / 120.0
        : scrollDelta.y() / 40.0;
    targetZoom = std::clamp(
        targetZoom * std::pow(1.2, step),
        MinimumZoom,
        MaximumZoom);
    zoomAnchorPosition = event->position().toPoint();
    zoomAnchorScenePosition = mapToScene(zoomAnchorPosition);

    zoomAnimation.stop();
    zoomAnimation.setStartValue(zoomFactor);
    zoomAnimation.setEndValue(targetZoom);
    zoomAnimation.start();
    event->accept();
}

void MapWidget::setAnimatedZoom(double zoom)
{
    if (zoom == zoomFactor)
        return;

    const double factor = zoom / zoomFactor;
    zoomFactor = zoom;
    scale(factor, factor);

    const QPointF shiftedAnchor = mapToScene(zoomAnchorPosition);
    const QPointF correction = shiftedAnchor - zoomAnchorScenePosition;
    translate(correction.x(), correction.y());
}

void MapWidget::createScene()
{
    if (!topology.getError().isEmpty())
    {
        QGraphicsTextItem *errorItem =
            graphicsScene.addText(topology.getError());
        errorItem->setDefaultTextColor(
            QColor(QStringLiteral("#ff8f8f")));
        graphicsScene.setSceneRect(errorItem->boundingRect());
        return;
    }

    for (const Link &link : topology.getLinks())
        graphicsScene.addItem(new LinkWidget(link, mapViewport));

    for (const Node &node : topology.getNodes())
        graphicsScene.addItem(new NodeWidget(node, mapViewport));

    engineWidgets.reserve(garage.getEngines().size());
    for (const Engine &engine : garage.getEngines())
    {
        auto *engineWidget = new EngineWidget(engine, mapViewport);
        engineWidgets.push_back(engineWidget);
        graphicsScene.addItem(engineWidget);
    }

    graphicsScene.setSceneRect(
        graphicsScene.itemsBoundingRect().adjusted(
            -SceneMargin,
            -SceneMargin,
            SceneMargin,
            SceneMargin));
}

void MapWidget::fitScene()
{
    if (!graphicsScene.sceneRect().isEmpty())
        fitInView(graphicsScene.sceneRect(), Qt::KeepAspectRatio);
}
