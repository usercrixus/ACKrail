#pragma once

#include "../garage/Garage.hpp"
#include "Topology.hpp"
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
