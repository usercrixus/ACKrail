#pragma once

#include "SimulationController.hpp"

#include <QAction>
#include <QMainWindow>
#include <QMenu>
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
    void showStatistics();

    QString topologyFile = QStringLiteral(":/map/paris_metro.json");
    QString weightFile;
    SimulationController simulationController;
    QStackedWidget *centralStack = nullptr;
    QWidget *emptyPage = nullptr;
    QAction *launchAction = nullptr;
    QAction *stopAction = nullptr;
    QAction *showPassengerEnginesAction = nullptr;
    QAction *showRebalancingEnginesAction = nullptr;
    QAction *showStatisticsAction = nullptr;
};
