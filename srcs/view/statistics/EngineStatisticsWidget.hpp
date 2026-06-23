#pragma once

#include "../../statistics/SimulationStatistics.hpp"
#include <QLabel>
#include <QWidget>

class EngineStatisticsWidget : public QWidget
{
public:
    explicit EngineStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent = nullptr);

private:
    void refresh();

    const SimulationStatistics &statistics;
    QLabel *tripCountLabel = nullptr;
    QLabel *passengerTripCountLabel = nullptr;
    QLabel *rebalancingTripCountLabel = nullptr;
    QLabel *totalDistanceLabel = nullptr;
    QLabel *passengerDistanceLabel = nullptr;
    QLabel *rebalancingDistanceLabel = nullptr;
    QLabel *averageTripDistanceLabel = nullptr;
    QLabel *maximumTripDistanceLabel = nullptr;
    QLabel *averageWaitLabel = nullptr;
    QLabel *maximumWaitLabel = nullptr;
};
