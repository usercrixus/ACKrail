#pragma once

#include "../../statistics/SimulationStatistics.hpp"

#include <QLabel>
#include <QWidget>

class GlobalStationStatisticsWidget : public QWidget
{
public:
    explicit GlobalStationStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent = nullptr);

private:
    void refresh();

    const SimulationStatistics &statistics;
    QLabel *stationCount = nullptr;
    QLabel *currentIdle = nullptr;
    QLabel *targetIdle = nullptr;
    QLabel *waitingPassengers = nullptr;
    QLabel *stationsWithoutIdle = nullptr;
    QLabel *departures = nullptr;
    QLabel *arrivals = nullptr;
    QLabel *averageIdle = nullptr;
    QLabel *averageTargetGap = nullptr;
    QLabel *availability = nullptr;
    QLabel *belowTarget = nullptr;
};
