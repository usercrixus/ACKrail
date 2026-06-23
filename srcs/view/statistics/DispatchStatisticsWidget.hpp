#pragma once

#include "../../statistics/SimulationStatistics.hpp"
#include <QLabel>
#include <QWidget>

class DispatchStatisticsWidget : public QWidget
{
public:
    explicit DispatchStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent = nullptr);

private:
    void refresh();

    const SimulationStatistics &statistics;
    QLabel *failedPassengerDispatchesLabel = nullptr;
    QLabel *failedRebalancingDispatchesLabel = nullptr;
    QLabel *completedPassengerTripsLabel = nullptr;
    QLabel *completedRebalancingTripsLabel = nullptr;
};
