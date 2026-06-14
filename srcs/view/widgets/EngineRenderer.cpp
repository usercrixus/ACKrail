#include "EngineRenderer.hpp"

#include <QFont>
#include <QFontMetrics>
#include <QStringList>
#include <QVector2D>
#include <algorithm>
#include <cmath>
#include <cstddef>

EngineRenderer::EngineRenderer(const Garage &garage, const MapViewport &mapViewport)
    : garage(garage),
      mapViewport(mapViewport)
{
    states.resize(garage.getActiveEngines().size());
}

void EngineRenderer::initialize()
{
    initializeOpenGLFunctions();
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/shaders/engine.vert")) || !program.addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/shaders/engine.frag")) || !program.link())
    {
        qFatal("Unable to create engine shader: %s", qPrintable(program.log()));
    }
    vertexArray.create();
    const float orange[] = {1.0f, 0.361f, 0.208f};
    const float cyan[] = {0.561f, 0.890f, 1.0f};
    const auto appendQuad = [](std::vector<Vertex> &vertices, float left, float top, float right, float bottom, const float color[3])
    {
        const float points[] = {left, top, right, top, right, bottom, left, top, right, bottom, left, bottom};
        for (int index = 0; index < 6; ++index)
        {
            vertices.push_back({points[index * 2], points[index * 2 + 1], color[0], color[1], color[2]});
        }
    };
    std::vector<Vertex> shape;
    appendQuad(shape, -11, -6, 11, 6, orange);
    appendQuad(shape, 1, -3.5f, 7, 3.5f, cyan);
    shapeVertexCount = static_cast<int>(shape.size());
    shapeBuffer.create();
    shapeBuffer.bind();
    shapeBuffer.allocate(shape.data(), static_cast<int>(shape.size() * sizeof(Vertex)));
    shapeBuffer.release();
    instanceBuffer.create();
    instanceBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    instanceBuffer.bind();
    bufferCapacity = static_cast<int>(states.size());
    instanceBuffer.allocate(bufferCapacity * static_cast<int>(sizeof(Instance)));
    instanceBuffer.release();
    refresh();
    updateBuffer();
}

void EngineRenderer::refresh()
{
    const auto &engines = garage.getActiveEngines();
    if (states.size() != engines.size())
        states.resize(engines.size());
    bool selectionFound = false;
    std::size_t index = 0;
    for (const auto &[id, engine] : engines)
    {
        calculateState(*engine, states[index]);
        selectionFound = selectionFound || states[index].engine == selectedEngine;
        ++index;
    }
    if (!selectionFound)
        selectedEngine = nullptr;
}

void EngineRenderer::draw(const QMatrix4x4 &matrix, const QSize &viewportSize)
{
    updateBuffer();
    if (activeEngineCount != 0)
    {
        QOpenGLVertexArrayObject::Binder binder(&vertexArray);
        program.bind();
        program.setUniformValue("sceneMatrix", matrix);
        program.setUniformValue("viewportSize", QVector2D(viewportSize.width(), viewportSize.height()));
        shapeBuffer.bind();
        program.enableAttributeArray(0);
        program.enableAttributeArray(1);
        program.setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, x), 2, sizeof(Vertex));
        program.setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, red), 3, sizeof(Vertex));
        instanceBuffer.bind();
        program.enableAttributeArray(2);
        program.enableAttributeArray(3);
        program.setAttributeBuffer(2, GL_FLOAT, offsetof(Instance, x), 2, sizeof(Instance));
        program.setAttributeBuffer(3, GL_FLOAT, offsetof(Instance, angleRadians), 1, sizeof(Instance));
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glDrawArraysInstanced(GL_TRIANGLES, 0, shapeVertexCount, activeEngineCount);
        glVertexAttribDivisor(2, 0);
        glVertexAttribDivisor(3, 0);
        instanceBuffer.release();
        shapeBuffer.release();
        program.release();
    }
}

void EngineRenderer::selectAt(const QPointF &screenPosition, const MapCamera &camera)
{
    constexpr double SelectionRadius = 14.0;
    selectedEngine = nullptr;
    double closestDistance = SelectionRadius;
    for (std::size_t index = 0; index < states.size(); ++index)
    {
        const RenderState &state = states[index];
        if (state.active)
        {
            const QPointF position = camera.sceneToScreen(state.position);
            const double distance = std::hypot(position.x() - screenPosition.x(), position.y() - screenPosition.y());
            if (distance <= closestDistance)
            {
                closestDistance = distance;
                selectedEngine = state.engine;
            }
        }
    }
}

void EngineRenderer::updateBuffer()
{
    instances.clear();
    instances.reserve(states.size());
    for (const RenderState &state : states)
    {
        if (state.active)
            instances.push_back({static_cast<float>(state.position.x()), static_cast<float>(state.position.y()), static_cast<float>(state.angleRadians)});
    }
    activeEngineCount = static_cast<int>(instances.size());
    instanceBuffer.bind();
    if (activeEngineCount > bufferCapacity)
    {
        bufferCapacity = activeEngineCount;
        instanceBuffer.allocate(instances.data(), activeEngineCount * static_cast<int>(sizeof(Instance)));
    }
    else if (activeEngineCount > 0)
    {
        instanceBuffer.write(0, instances.data(), activeEngineCount * static_cast<int>(sizeof(Instance)));
    }
    instanceBuffer.release();
}

void EngineRenderer::calculateState(const Engine &engine, RenderState &state) const
{
    const EnginePad &pad = engine.getPad();
    state.engine = &engine;
    if (!pad.isActive())
    {
        state.active = false;
        return;
    }
    const qsizetype index = pad.getCurrentContractStep();
    const QVector<Node *> &stations = pad.getTrajectory()->getStations();
    const QPointF start = mapViewport.mapPosition(stations[index]->getLatitude(), stations[index]->getLongitude());
    const QPointF end = mapViewport.mapPosition(stations[index + 1]->getLatitude(), stations[index + 1]->getLongitude());
    state.position = start + (end - start) * pad.getCurrentLinkProgress();
    state.angleRadians = std::atan2(end.y() - start.y(), end.x() - start.x());
    state.active = true;
}

void EngineRenderer::drawInformation(QPainter &painter, const MapCamera &camera) const
{
    if (selectedEngine == nullptr)
        return;
    for (const RenderState &state : states)
    {
        if (state.engine == selectedEngine && state.active)
        {
            const QString information = createInformation(*selectedEngine);
            if (!information.isEmpty())
            {
                const QFont font(QStringLiteral("Sans Serif"), 9);
                const QFontMetrics metrics(font);
                QRectF panel = metrics.boundingRect(QRect(0, 0, 420, camera.viewportSize().height()), Qt::AlignLeft | Qt::AlignTop, information);
                panel.adjust(-10, -8, 10, 8);
                panel.moveTopLeft(camera.sceneToScreen(state.position) + QPointF(16, -panel.height() - 12));
                if (panel.right() > camera.viewportSize().width() - 8)
                    panel.moveRight(camera.viewportSize().width() - 8);
                if (panel.top() < 8)
                    panel.moveTop(8);
                painter.setFont(font);
                painter.setPen(QPen(QColor(QStringLiteral("#ff8a65")), 1.5));
                painter.setBrush(QColor(QStringLiteral("#17232f")));
                painter.drawRoundedRect(panel, 6, 6);
                painter.setPen(QColor(QStringLiteral("#f4f7f9")));
                painter.drawText(panel.adjusted(10, 8, -10, -8), Qt::AlignLeft | Qt::AlignTop, information);
            }
            return;
        }
    }
}

QString EngineRenderer::getInformation(const Engine &engine, const Route &route) const
{
    const QVector<Node *> &stations = route.getStations();
    const double totalDistance = route.getTotalDistanceKilometers();
    const double travelledDistance = std::clamp(engine.getPad().getTravelledDistanceKilometers(), 0.0, totalDistance);
    const double progress = totalDistance > 0.0 ? travelledDistance / totalDistance * 100.0 : 0.0;
    return QStringLiteral("%1\n"
                          "%2 -> %3\n"
                          "Speed: %4 km/h\n"
                          "Distance: %5 / %6 km\n"
                          "Progress: %7%")
        .arg(engine.getModelName())
        .arg(stations.first()->getName())
        .arg(stations.last()->getName())
        .arg(engine.getPad().getCurrentSpeedKilometersPerHour(), 0, 'f', 1)
        .arg(travelledDistance, 0, 'f', 2)
        .arg(totalDistance, 0, 'f', 2)
        .arg(progress, 0, 'f', 1);
}

QString EngineRenderer::getRoadmap(const Route &route) const
{
    const QVector<Node *> &stations = route.getStations();
    const QVector<Link *> &links = route.getLinks();
    const QVector<Route::ContractStep> &contract = route.getContract();
    if (contract.size() != links.size() || stations.size() != links.size() + 1)
        return {};

    QStringList roadmap;
    roadmap.reserve(contract.size());
    for (qsizetype index = 0; index < contract.size(); ++index)
    {
        roadmap.append(
            QStringLiteral("%1. Wait %2 s -> Link %3: %4 -> %5 (%6 s)")
                .arg(index + 1)
                .arg(contract[index].waitSeconds, 0, 'f', 2)
                .arg(links[index]->getId())
                .arg(stations[index]->getName())
                .arg(stations[index + 1]->getName())
                .arg(contract[index].traversalSeconds, 0, 'f', 2));
    }
    return roadmap.join(QLatin1Char('\n'));
}

QString EngineRenderer::createInformation(const Engine &engine) const
{
    const Route *route = engine.getPad().getTrajectory();
    if (route == nullptr || route->getStations().isEmpty())
        return {};
    QString information = getInformation(engine, *route);
    const QString roadmap = getRoadmap(*route);
    if (!roadmap.isEmpty())
    {
        information += QStringLiteral("\n\nRoadmap (total time: %1 s):\n").arg(engine.getPad().getTotalTrajectorySeconds(), 0, 'f', 2) + roadmap;
    }
    return information;
}
