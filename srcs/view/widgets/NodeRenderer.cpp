#include "NodeRenderer.hpp"

#include <QFont>
#include <QVector3D>
#include <cstddef>
#include <vector>

NodeRenderer::NodeRenderer(const Topology &topology, const MapViewport &mapViewport)
    : topology(topology),
      mapViewport(mapViewport)
{
}

void NodeRenderer::initialize()
{
    initializeOpenGLFunctions();
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/shaders/node.vert")) || !program.addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/shaders/node.frag")) || !program.link())
    {
        qFatal("Unable to create node shader: %s", qPrintable(program.log()));
    }
    vertexArray.create();
    std::vector<Vertex> nodes;
    nodes.reserve(topology.getNodes().size());
    for (const Node &node : topology.getNodes())
    {
        const QPointF position = mapViewport.mapPosition(node.getLatitude(), node.getLongitude());
        nodes.push_back({static_cast<float>(position.x()), static_cast<float>(position.y()), 0.956f, 0.969f, 0.976f});
    }
    vertexCount = static_cast<int>(nodes.size());
    buffer.create();
    buffer.bind();
    buffer.allocate(nodes.data(), static_cast<int>(nodes.size() * sizeof(Vertex)));
    buffer.release();
}

void NodeRenderer::draw(const QMatrix4x4 &matrix)
{
    if (vertexCount != 0)
    {
        QOpenGLVertexArrayObject::Binder binder(&vertexArray);
        program.bind();
        program.setUniformValue("sceneMatrix", matrix);
        buffer.bind();
        program.enableAttributeArray(0);
        program.enableAttributeArray(1);
        program.setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, x), 2, sizeof(Vertex));
        program.setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, red), 3, sizeof(Vertex));
        program.setUniformValue("pointSize", 17.0f);
        glDrawArrays(GL_POINTS, 0, vertexCount);
        program.disableAttributeArray(1);
        program.setAttributeValue(1, QVector3D(0.063f, 0.102f, 0.141f));
        program.setUniformValue("pointSize", 11.0f);
        glDrawArrays(GL_POINTS, 0, vertexCount);
        buffer.release();
        program.release();
    }
}

void NodeRenderer::drawLabels(QPainter &painter, const MapCamera &camera) const
{
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 8));
    painter.setPen(QColor(QStringLiteral("#dce6ed")));
    for (const Node &node : topology.getNodes())
    {
        const QPointF screenPosition = camera.sceneToScreen(mapViewport.mapPosition(node.getLatitude(), node.getLongitude()));
        painter.drawText(screenPosition + QPointF(11, 4), node.getName());
    }
}
