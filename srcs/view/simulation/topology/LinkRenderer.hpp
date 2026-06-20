#pragma once

#include "../../helper/MapViewport.hpp"
#include "../../../topology/Topology.hpp"
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QSize>
#include <QVector2D>
#include <vector>

class LinkRenderer : protected QOpenGLExtraFunctions
{
public:
    LinkRenderer(
        const Topology &topology,
        const MapViewport &mapViewport);

    void initialize();
    void draw(const QMatrix4x4 &matrix, const QSize &viewportSize);

private:
    struct LinkInstance
    {
        float startX;
        float startY;
        float endX;
        float endY;
        float red;
        float green;
        float blue;
    };

    void initializeShader();
    void initializeInstanceBuffer();
    void initializeShapeBuffer();
    static std::vector<QVector2D> createLinkShape();
    static void appendTriangle(
        std::vector<QVector2D> &shape,
        const QVector2D &first,
        const QVector2D &second,
        const QVector2D &third);
    static void appendRoundCap(
        std::vector<QVector2D> &shape,
        float marker,
        float firstAngle);

    const Topology &topology;
    const MapViewport &mapViewport;
    QOpenGLShaderProgram program;
    QOpenGLBuffer shapeBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer instanceBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject vertexArray;
    int shapeVertexCount = 0;
    int instanceCount = 0;
};
