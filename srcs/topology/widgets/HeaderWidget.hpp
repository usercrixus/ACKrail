#pragma once

#include "../../garage/Garage.hpp"
#include "../Topology.hpp"
#include <QWidget>

class QPaintEvent;

class HeaderWidget : public QWidget
{
public:
    explicit HeaderWidget(const Topology &topology, const Garage &garage, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int Height = 64;
    const Topology &topology;
    const Garage &garage;
};
