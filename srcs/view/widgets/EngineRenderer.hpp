#pragma once

#include "../helper/MapCamera.hpp"
#include "../../garage/Garage.hpp"
#include "../helper/MapViewport.hpp"
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QPainter>
#include <QSize>
#include <vector>

class EngineRenderer : protected QOpenGLExtraFunctions
{
public:
    EngineRenderer(const Garage &garage, const MapViewport &mapViewport);
    void initialize();
    void refresh();
    void draw(const QMatrix4x4 &matrix, const QSize &viewportSize);
    void selectAt(const QPointF &screenPosition, const MapCamera &camera);
    void drawInformation(QPainter &painter, const MapCamera &camera) const;

private:
    struct Vertex
    {
        float x;
        float y;
        float red;
        float green;
        float blue;
    };

    struct Instance
    {
        float x;
        float y;
        float angleRadians;
    };

    struct RenderState
    {
        const Engine *engine = nullptr;
        QPointF position;
        double angleRadians = 0.0;
        bool active = false;
    };

    void updateBuffer();
    void calculateState(const Engine &engine, RenderState &state) const;
    QString createInformation(const Engine &engine) const;
    QString getInformation(const Engine &engine, const Route &route) const;
    QString getRoadmap(const Route &route) const;

    const Garage &garage;
    const MapViewport &mapViewport;
    QOpenGLShaderProgram program;
    QOpenGLBuffer shapeBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer instanceBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject vertexArray;
    std::vector<RenderState> states;
    std::vector<Instance> instances;
    int shapeVertexCount = 0;
    int activeEngineCount = 0;
    int bufferCapacity = 0;
    const Engine *selectedEngine = nullptr;
};
