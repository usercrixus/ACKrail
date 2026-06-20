#pragma once

#include "../../helper/MapCamera.hpp"
#include "../../helper/MapViewport.hpp"
#include "../../../topology/Topology.hpp"
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QPainter>

class NodeRenderer : protected QOpenGLExtraFunctions
{
public:
    NodeRenderer(const Topology &topology, const MapViewport &mapViewport);
    void initialize();
    void draw(const QMatrix4x4 &matrix);
    void drawLabels(QPainter &painter, const MapCamera &camera) const;

private:
    struct Vertex
    {
        float x;
        float y;
        float red;
        float green;
        float blue;
    };

    const Topology &topology;
    const MapViewport &mapViewport;
    QOpenGLShaderProgram program;
    QOpenGLBuffer buffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject vertexArray;
    int vertexCount = 0;
};
