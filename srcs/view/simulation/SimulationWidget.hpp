#pragma once

#include "../../garage/Garage.hpp"
#include "../../simulator/TrafficManager.hpp"
#include "../../topology/Topology.hpp"
#include "header/HeaderWidget.hpp"
#include "map/MapWidget.hpp"
#include <QWidget>

class SimulationWidget : public QWidget
{
public:
    explicit SimulationWidget(const Topology &topology,
                              const Garage &garage,
                              const TrafficManager &trafficManager,
                              QWidget *parent = nullptr);
    void refresh(double simulationTimeSeconds);
    void setPassengerEnginesVisible(bool visible);
    void setRebalancingEnginesVisible(bool visible);

private:
    HeaderWidget *headerWidget;
    MapWidget *mapWidget;
};
