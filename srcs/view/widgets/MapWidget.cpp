#include "MapWidget.hpp"

#include <QGuiApplication>
#include <QPainter>
#include <QStyleHints>

MapWidget::MapWidget(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, QWidget *parent)
    : QOpenGLWidget(parent),
      topology(topology),
      mapViewport(topology),
      camera(topology, mapViewport),
      linkRenderer(topology, mapViewport),
      nodeRenderer(topology, mapViewport),
      engineRenderer(garage, trafficManager, mapViewport)
{
    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
}

void MapWidget::refresh()
{
    engineRenderer.refresh();
    update();
}

void MapWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.043f, 0.067f, 0.094f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    linkRenderer.initialize();
    nodeRenderer.initialize();
    engineRenderer.initialize();
}

void MapWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (topology.getError().isEmpty())
    {
        const QMatrix4x4 matrix = camera.matrix();
        linkRenderer.draw(matrix, size());
        nodeRenderer.draw(matrix);
        engineRenderer.draw(matrix, size());
    }
    drawOverlay();
}

void MapWidget::resizeGL(int width, int height)
{
    camera.resize(width, height);
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isLeftButtonPressed = true;
        isDragging = false;
        mousePressPosition = event->position().toPoint();
        lastMousePosition = mousePressPosition;
        event->accept();
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!isLeftButtonPressed)
    {
        QOpenGLWidget::mouseMoveEvent(event);
    }
    else
    {
        const QPoint currentPosition = event->position().toPoint();
        if (!isDragging && (currentPosition - mousePressPosition).manhattanLength() >= QGuiApplication::styleHints()->startDragDistance())
        {
            isDragging = true;
            setCursor(Qt::ClosedHandCursor);
        }
        if (isDragging)
        {
            camera.pan(currentPosition - lastMousePosition);
            update();
        }
        lastMousePosition = currentPosition;
        event->accept();
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isLeftButtonPressed)
    {
        if (!isDragging)
            engineRenderer.selectAt(event->position(), camera);
        isLeftButtonPressed = false;
        isDragging = false;
        setCursor(Qt::ArrowCursor);
        update();
        event->accept();
        return;
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    const int verticalDelta = event->angleDelta().y();
    if (verticalDelta != 0)
    {
        camera.zoomAt(
            event->position(),
            verticalDelta > 0 ? 1.2 : 1.0 / 1.2);
        update();
    }
    event->accept();
}

void MapWidget::drawOverlay()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (!topology.getError().isEmpty())
    {
        painter.setPen(QColor(QStringLiteral("#ff8f8f")));
        painter.drawText(rect(), Qt::AlignCenter, topology.getError());
        return;
    }
    nodeRenderer.drawLabels(painter, camera);
    engineRenderer.drawInformation(painter, camera);
}
