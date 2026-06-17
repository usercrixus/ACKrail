#include "garage/Garage.hpp"
#include "simulator/TrafficGenerator.hpp"
#include "simulator/TrafficManager.hpp"
#include "simulator/TrafficSimulator.hpp"
#include "topology/Topology.hpp"
#include "view/TopologyWidget.hpp"

#include <QApplication>
#include <QElapsedTimer>
#include <QSurfaceFormat>
#include <QTimer>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    Topology topology;
    const QString topologyFile = argc > 1 ? QString::fromLocal8Bit(argv[1]) : QStringLiteral(":/map/paris_metro.json");
    topology.load(topologyFile);

    Garage garage(50000);
    TrafficManager trafficManager(topology, garage);
    TrafficGenerator trafficGenerator(topology, garage, trafficManager);
    TrafficSimulator trafficSimulator(trafficManager, trafficGenerator);
    TopologyWidget window(topology, garage, trafficManager);
    QElapsedTimer simulationClock;
    QTimer simulationTimer;
    QTimer renderTimer;
    simulationTimer.setInterval(16);
    renderTimer.setInterval(33);
    QObject::connect(&simulationTimer, &QTimer::timeout, [&trafficSimulator, &simulationClock]()
                     {
                         const double elapsedSeconds = static_cast<double>(simulationClock.restart()) / 1000.0;
                         trafficSimulator.advance(elapsedSeconds);
                     });
    QObject::connect(&renderTimer, &QTimer::timeout, &window, &TopologyWidget::refresh);
    window.resize(1100, 720);
    window.show();
    simulationClock.start();
    simulationTimer.start();
    renderTimer.start();

    const int result = app.exec();
    return result;
}
