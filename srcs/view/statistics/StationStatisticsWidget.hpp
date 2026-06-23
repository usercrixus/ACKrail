#pragma once

#include "../../statistics/SimulationStatistics.hpp"
#include <QLabel>
#include <QWidget>

class StationStatisticsWidget : public QWidget
{
public:
    explicit StationStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent = nullptr);

private:
    void refresh();

    const SimulationStatistics &statistics;
    QLabel *averageIdleLabel = nullptr;
    QLabel *minimumAverageIdleLabel = nullptr;
    QLabel *maximumAverageIdleLabel = nullptr;
    QLabel *averageTargetGapLabel = nullptr;
    QLabel *maximumTargetGapLabel = nullptr;
    QLabel *noEngineTimeLabel = nullptr;
    QLabel *belowTargetTimeLabel = nullptr;
    QLabel *aboveTargetTimeLabel = nullptr;
};
