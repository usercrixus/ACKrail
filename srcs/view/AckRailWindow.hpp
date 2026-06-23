#pragma once

#include "SimulationController.hpp"

#include <QAction>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QPointer>
#include <QStackedWidget>
#include <QString>
#include <QWidget>

class AckRailWindow : public QMainWindow
{
public:
    explicit AckRailWindow(QWidget *parent = nullptr);

private:
    void createMenus();
    void createFileMenu();
    void createSimulationMenu();
    void createSimulationViewMenu(QMenu *simulationMenu);
    void createStatisticsViewMenu(QMenu *viewMenu);
    void createCentralView();
    void updateStatus();
    void closeStatisticsDocks();
    void showEngineStatistics();
    void showStationStatistics();
    void showDispatchStatistics();
    QDockWidget *createStatisticsDock(const QString &title, QWidget *content);

    QString topologyFile = QStringLiteral(":/map/paris_metro.json");
    QString weightFile;
    SimulationController simulationController;
    QStackedWidget *centralStack = nullptr;
    QWidget *emptyPage = nullptr;
    QAction *launchAction = nullptr;
    QAction *stopAction = nullptr;
    QAction *showPassengerEnginesAction = nullptr;
    QAction *showRebalancingEnginesAction = nullptr;
    QAction *showEngineStatisticsAction = nullptr;
    QAction *showStationStatisticsAction = nullptr;
    QAction *showDispatchStatisticsAction = nullptr;
    QPointer<QDockWidget> engineStatisticsDock;
    QPointer<QDockWidget> stationStatisticsDock;
    QPointer<QDockWidget> dispatchStatisticsDock;
};
