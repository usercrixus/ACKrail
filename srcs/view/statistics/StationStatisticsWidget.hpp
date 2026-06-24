#pragma once

#include "../../statistics/SimulationStatistics.hpp"
#include "../../topology/Topology.hpp"
#include <QLabel>
#include <QListWidget>
#include <QWidget>

class StationStatisticsWidget : public QWidget
{
public:
    StationStatisticsWidget(const SimulationStatistics &statistics, const Topology &topology, QWidget *parent = nullptr);

private:
    void refresh();
    void selectStation(QListWidgetItem *item);
    void clearDetails();

    const SimulationStatistics &statistics;
    QListWidget *stationList = nullptr;
    int selectedStationId = -1;
    QLabel *selectedStationName = nullptr;
    QLabel *departures = nullptr;
    QLabel *arrivals = nullptr;
    QLabel *netFlow = nullptr;
    QLabel *currentIdle = nullptr;
    QLabel *waitingPassengers = nullptr;
    QLabel *averageIdle = nullptr;
    QLabel *idleRange = nullptr;
    QLabel *targetIdle = nullptr;
    QLabel *activityShare = nullptr;
    QLabel *availability = nullptr;
    QLabel *belowTarget = nullptr;
    QLabel *averageTargetGap = nullptr;
};
