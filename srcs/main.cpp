#include "garage/Garage.hpp"
#include "simulator/TrafficGenerator.hpp"
#include "topology/Topology.hpp"
#include "topology/TopologyWidget.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Topology topology;
    const QString topologyFile = argc > 1
        ? QString::fromLocal8Bit(argv[1])
        : QStringLiteral(":/map/paris_metro.json");
    topology.load(topologyFile);

    Garage garage(1000);
    TrafficGenerator trafficGenerator(topology, garage);
    TopologyWidget window(topology, garage, trafficGenerator);
    window.resize(1100, 720);
    window.show();

    return app.exec();
}
