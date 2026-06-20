#pragma once

#include "../helper/MapCamera.hpp"
#include "../../garage/Garage.hpp"
#include "../../simulator/TrafficManager.hpp"
#include "../helper/MapViewport.hpp"
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <limits>
#include <vector>

class EngineRenderer : protected QOpenGLExtraFunctions
{
public:
    EngineRenderer(const Garage &garage, const TrafficManager &trafficManager, const MapViewport &mapViewport);
    void initialize();
    void refresh(double simulationTimeSeconds);
    void draw(const QMatrix4x4 &matrix, const QSize &viewportSize);
    void selectAt(const QPointF &screenPosition, const MapCamera &camera);
    void drawInformation(QPainter &painter, const MapCamera &camera, double simulationTimeSeconds) const;
    void setPassengerEnginesVisible(bool visible);
    void setRebalancingEnginesVisible(bool visible);

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
        float red;
        float green;
        float blue;
    };

    struct RenderState
    {
        const Engine *engine = nullptr;
        const Route *route = nullptr;
        QPointF linkStart;
        QPointF linkDelta;
        QPointF position;
        double angleRadians = 0.0;
        EnginePad::TravelType travelType = EnginePad::TravelType::Idle;
        qsizetype contractStep = std::numeric_limits<qsizetype>::max();
        bool active = false;
    };

    void updateBuffer();
    static Instance createInstance(const RenderState &state);
    static QColor colorForTravelType(EnginePad::TravelType travelType);
    void calculateState(const Engine &engine, double simulationTimeSeconds, RenderState &state) const;
    bool isTravelTypeVisible(EnginePad::TravelType travelType) const;
    QString createInformation(const Engine &engine, double simulationTimeSeconds) const;
    QString getInformation(const Engine &engine, const Route &route, double simulationTimeSeconds) const;
    QString getRoadmap(const Route &route) const;

    const Garage &garage;
    const MapViewport &mapViewport;
    QOpenGLShaderProgram program;
    QOpenGLBuffer shapeBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer instanceBuffer{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject vertexArray;
    QPixmap enginePreview;
    std::vector<RenderState> states;
    std::vector<Instance> instances;
    int shapeVertexCount = 0;
    int activeEngineCount = 0;
    int bufferCapacity = 0;
    bool passengerEnginesVisible = true;
    bool rebalancingEnginesVisible = true;
    const Engine *selectedEngine = nullptr;
};
