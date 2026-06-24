#pragma once

#include "../../statistics/SimulationStatistics.hpp"
#include "../../topology/Topology.hpp"

#include <QDialog>

class StatisticsDialog : public QDialog
{
public:
    StatisticsDialog(const SimulationStatistics &statistics, const Topology &topology, QWidget *parent = nullptr);
};
