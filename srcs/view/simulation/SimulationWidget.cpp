#include "SimulationWidget.hpp"
#include <QVBoxLayout>

SimulationWidget::SimulationWidget(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, QWidget *parent)
    : QWidget(parent),
      headerWidget(new HeaderWidget(topology, garage, this)),
      mapWidget(new MapWidget(topology, garage, trafficManager, this))
{
    setWindowTitle(QStringLiteral("ACKrail"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(headerWidget);
    layout->addWidget(mapWidget, 1);
}

void SimulationWidget::refresh(double simulationTimeSeconds)
{
    headerWidget->refresh();
    mapWidget->refresh(simulationTimeSeconds);
}

void SimulationWidget::setPassengerEnginesVisible(bool visible)
{
    mapWidget->setPassengerEnginesVisible(visible);
}

void SimulationWidget::setRebalancingEnginesVisible(bool visible)
{
    mapWidget->setRebalancingEnginesVisible(visible);
}
