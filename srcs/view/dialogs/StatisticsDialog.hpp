#pragma once

#include "../../statistics/SimulationStatistics.hpp"

#include <QDialog>

class StatisticsDialog : public QDialog
{
public:
    explicit StatisticsDialog(const SimulationStatistics &statistics, QWidget *parent = nullptr);
};
