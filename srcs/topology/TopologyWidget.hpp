#pragma once

#include "../garage/Garage.hpp"
#include "Topology.hpp"
#include <QWidget>

class HeaderWidget;
class MapWidget;

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(
        const Topology &topology,
        const Garage &garage,
        QWidget *parent = nullptr);

    void refresh();

private:
    HeaderWidget *headerWidget;
    MapWidget *mapWidget;
};
