#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "widgets/HeaderWidget.hpp"
#include "widgets/MapWidget.hpp"
#include <QWidget>

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(const Topology &topology, const Garage &garage, QWidget *parent = nullptr);
    void refresh();

private:
    HeaderWidget *headerWidget;
    MapWidget *mapWidget;
};
