#pragma once

#include "Topology.hpp"

#include <QPointF>
#include <QWidget>

class TopologyWidget : public QWidget
{
public:
    explicit TopologyWidget(const Topology &topology, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF mapPosition(double latitude, double longitude) const;
    const Topology &topology;
};
