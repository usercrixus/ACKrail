#pragma once

#include "../../../garage/Garage.hpp"
#include "../../../simulator/TrafficBalancer.hpp"
#include "../../../topology/Topology.hpp"
#include <QWidget>
#include <cstddef>

class QPaintEvent;

class HeaderWidget : public QWidget
{
public:
    explicit HeaderWidget(const Topology &topology, const Garage &garage, const TrafficBalancer &trafficBalancer, QWidget *parent = nullptr);
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int Height = 64;
    const Topology &topology;
    const Garage &garage;
    const TrafficBalancer &trafficBalancer;
    std::size_t activeEngineCount = 0;
    std::size_t engineCount = 0;
    double networkBalancePercent = 100.0;
};
