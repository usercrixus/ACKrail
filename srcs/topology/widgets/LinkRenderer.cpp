#include "LinkRenderer.hpp"

#include <QColor>
#include <cmath>
#include <cstddef>
#include <numbers>

LinkRenderer::LinkRenderer(
    const Topology &topology,
    const MapViewport &mapViewport)
    : topology(topology),
      mapViewport(mapViewport)
{
}

void LinkRenderer::initialize()
{
    initializeOpenGLFunctions();
    initializeShader();
    vertexArray.create();
    initializeInstanceBuffer();
    initializeShapeBuffer();
}

void LinkRenderer::initializeShader()
{
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/shaders/link.vert")) || !program.addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/shaders/link.frag")) || !program.link())
    {
        qFatal("Unable to create link shader: %s",
               qPrintable(program.log()));
    }
}

void LinkRenderer::initializeInstanceBuffer()
{
    std::vector<LinkInstance> links;
    links.reserve(topology.getLinks().size());
    for (const Link &link : topology.getLinks())
    {
        const QColor color(link.getColor());
        const QPointF start = mapViewport.mapPosition(link.getFromNode().getLatitude(), link.getFromNode().getLongitude());
        const QPointF end = mapViewport.mapPosition(link.getToNode().getLatitude(), link.getToNode().getLongitude());
        links.push_back({static_cast<float>(start.x()), static_cast<float>(start.y()), static_cast<float>(end.x()), static_cast<float>(end.y()), color.redF(), color.greenF(), color.blueF()});
    }
    instanceCount = static_cast<int>(links.size());
    instanceBuffer.create();
    instanceBuffer.bind();
    instanceBuffer.allocate(links.data(), static_cast<int>(links.size() * sizeof(LinkInstance)));
    instanceBuffer.release();
}

void LinkRenderer::initializeShapeBuffer()
{
    const std::vector<QVector2D> shape = createLinkShape();
    shapeVertexCount = static_cast<int>(shape.size());
    shapeBuffer.create();
    shapeBuffer.bind();
    shapeBuffer.allocate(shape.data(), static_cast<int>(shape.size() * sizeof(QVector2D)));
    shapeBuffer.release();
}

std::vector<QVector2D> LinkRenderer::createLinkShape()
{
    constexpr float LinkRadius = 3.5f;
    constexpr float StartMarker = -0.0001f;
    constexpr float EndMarker = 0.0001f;
    constexpr int CapSegments = 12;

    std::vector<QVector2D> shape;
    shape.reserve(6 + CapSegments * 6);
    appendTriangle(shape, QVector2D(StartMarker, -LinkRadius), QVector2D(EndMarker, -LinkRadius), QVector2D(EndMarker, LinkRadius));
    appendTriangle(shape, QVector2D(StartMarker, -LinkRadius), QVector2D(EndMarker, LinkRadius), QVector2D(StartMarker, LinkRadius));
    appendRoundCap(shape, StartMarker, std::numbers::pi_v<float> / 2.0f);
    appendRoundCap(shape, EndMarker, -std::numbers::pi_v<float> / 2.0f);
    return shape;
}

void LinkRenderer::appendTriangle(std::vector<QVector2D> &shape, const QVector2D &first, const QVector2D &second, const QVector2D &third)
{
    shape.push_back(first);
    shape.push_back(second);
    shape.push_back(third);
}

void LinkRenderer::appendRoundCap(std::vector<QVector2D> &shape, float marker, float firstAngle)
{
    constexpr float LinkRadius = 3.5f;
    constexpr int CapSegments = 12;

    for (int segment = 0; segment < CapSegments; ++segment)
    {
        const float angle = firstAngle + std::numbers::pi_v<float> * segment / CapSegments;
        const float nextAngle = firstAngle + std::numbers::pi_v<float> * (segment + 1) / CapSegments;
        appendTriangle(shape, QVector2D(marker, 0.0f), QVector2D(std::cos(angle) * LinkRadius, std::sin(angle) * LinkRadius), QVector2D(std::cos(nextAngle) * LinkRadius, std::sin(nextAngle) * LinkRadius));
    }
}

void LinkRenderer::draw(const QMatrix4x4 &matrix, const QSize &viewportSize)
{
    if (instanceCount != 0)
    {
        QOpenGLVertexArrayObject::Binder binder(&vertexArray);
        program.bind();
        program.setUniformValue("sceneMatrix", matrix);
        program.setUniformValue("viewportSize", QVector2D(viewportSize.width(), viewportSize.height()));
        shapeBuffer.bind();
        program.enableAttributeArray(0);
        program.setAttributeBuffer(0, GL_FLOAT, 0, 2, sizeof(QVector2D));
        instanceBuffer.bind();
        for (int attribute = 1; attribute <= 3; ++attribute)
            program.enableAttributeArray(attribute);
        program.setAttributeBuffer(1, GL_FLOAT, offsetof(LinkInstance, startX), 2, sizeof(LinkInstance));
        program.setAttributeBuffer(2, GL_FLOAT, offsetof(LinkInstance, endX), 2, sizeof(LinkInstance));
        program.setAttributeBuffer(3, GL_FLOAT, offsetof(LinkInstance, red), 3, sizeof(LinkInstance));
        for (int attribute = 1; attribute <= 3; ++attribute)
            glVertexAttribDivisor(attribute, 1);
        glDrawArraysInstanced(GL_TRIANGLES, 0, shapeVertexCount, instanceCount);
        for (int attribute = 1; attribute <= 3; ++attribute)
            glVertexAttribDivisor(attribute, 0);
        instanceBuffer.release();
        shapeBuffer.release();
        program.release();
    }
}
