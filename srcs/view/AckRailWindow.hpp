#pragma once

#include "../simulator/SimulationSession.hpp"

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <memory>

class AckRailWindow : public QMainWindow
{
public:
    explicit AckRailWindow(QWidget *parent = nullptr);

private:
    void createMenus();
    void createFileMenu();
    void createSimulationMenu();
    void createSimulationViewMenu(QMenu *simulationMenu);
    void createCentralView();
    void setSimulation();
    void launchSimulation();
    void stopSimulation();
    void clearSimulation();
    void updateStatus();
    void applyEngineVisibilitySettings();
    void showDimOverlay(QWidget &overlay);

    QString topologyFile = QStringLiteral(":/map/paris_metro.json");
    QString weightFile;
    std::unique_ptr<SimulationSession> simulationSession;
    QStackedWidget *centralStack = nullptr;
    QWidget *emptyPage = nullptr;
    SimulationWidget *simulationPage = nullptr;
    QAction *launchAction = nullptr;
    QAction *stopAction = nullptr;
    QAction *showPassengerEnginesAction = nullptr;
    QAction *showRebalancingEnginesAction = nullptr;
};
