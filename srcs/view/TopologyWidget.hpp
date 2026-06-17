#pragma once

#include "../garage/Garage.hpp"
#include "../simulator/TrafficManager.hpp"
#include "../topology/Topology.hpp"
#include "widgets/HeaderWidget.hpp"
#include "widgets/MapWidget.hpp"
#include <QWidget>

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(const Topology &topology, const Garage &garage, const TrafficManager &trafficManager, QWidget *parent = nullptr);
    void refresh();

private:
    HeaderWidget *headerWidget;
    MapWidget *mapWidget;
};
