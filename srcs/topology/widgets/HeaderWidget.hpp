#pragma once

#include "../../garage/Garage.hpp"
#include "../Topology.hpp"
#include <QWidget>
#include <cstddef>

class QPaintEvent;

class HeaderWidget : public QWidget
{
public:
    explicit HeaderWidget(const Topology &topology, const Garage &garage, QWidget *parent = nullptr);
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int Height = 64;
    const Topology &topology;
    const Garage &garage;
    std::size_t activeEngineCount = 0;
    std::size_t engineCount = 0;
};
