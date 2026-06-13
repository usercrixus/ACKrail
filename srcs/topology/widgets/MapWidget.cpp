#include "MapWidget.hpp"
#include "LinkWidget.hpp"
#include "NodeWidget.hpp"

#include <QColor>
#include <QGuiApplication>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleHints>

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
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    graphicsScene.setItemIndexMethod(QGraphicsScene::BspTreeIndex);
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
        if ((currentPosition - mousePressPosition).manhattanLength() >= QGuiApplication::styleHints()->startDragDistance())
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
        if (!isDragging)
        {
            graphicsScene.clearSelection();
            if (auto *engineWidget = dynamic_cast<EngineWidget *>(itemAt(event->position().toPoint())))
            {
                engineWidget->setSelected(true);
            }
        }
        isLeftButtonPressed = false;
        isDragging = false;
    }
    viewport()->setCursor(Qt::ArrowCursor);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    const int verticalDelta = event->angleDelta().y();
    if (verticalDelta == 0)
    {
        event->accept();
        return;
    }

    const double factor = verticalDelta > 0 ? 1.2 : 1.0 / 1.2;
    scale(factor, factor);
    viewport()->update();
    event->accept();
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

void MapWidget::refresh()
{
    for (EngineWidget *engineWidget : engineWidgets)
        engineWidget->updatePosition();
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
        fitInView(graphicsScene.itemsBoundingRect(), Qt::KeepAspectRatio);
}
