#include "garage/Garage.hpp"
#include "simulator/TrafficGenerator.hpp"
#include "simulator/TrafficManager.hpp"
#include "simulator/TrafficSimulator.hpp"
#include "topology/Topology.hpp"
#include "topology/TopologyWidget.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Topology topology;
    const QString topologyFile = argc > 1 ? QString::fromLocal8Bit(argv[1]) : QStringLiteral(":/map/paris_metro.json");
    topology.load(topologyFile);

    Garage garage(1000);
    TrafficGenerator trafficGenerator(topology, garage);
    TrafficManager trafficManager(garage);
    TrafficSimulator trafficSimulator(trafficManager, trafficGenerator);
    TopologyWidget window(topology, garage);
    QTimer renderTimer;
    renderTimer.setInterval(33);
    QObject::connect(&renderTimer, &QTimer::timeout, &window, &TopologyWidget::refresh);
    window.resize(1100, 720);
    window.show();
    trafficSimulator.start();
    renderTimer.start();

    const int result = app.exec();
    trafficSimulator.stop();
    return result;
}
