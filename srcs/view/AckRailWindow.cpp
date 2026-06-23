#include "AckRailWindow.hpp"

#include "simulation/EmptyPageWidget.hpp"
#include "statistics/DispatchStatisticsWidget.hpp"
#include "statistics/EngineStatisticsWidget.hpp"
#include "statistics/StationStatisticsWidget.hpp"
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QWidget>

AckRailWindow::AckRailWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("ACKrail"));
    createCentralView();
    createMenus();
}

void AckRailWindow::createCentralView()
{
    centralStack = new QStackedWidget(this);
    emptyPage = new EmptyPageWidget(centralStack);
    centralStack->addWidget(emptyPage);
    setCentralWidget(centralStack);
}

void AckRailWindow::createMenus()
{
    createFileMenu();
    createSimulationMenu();
    updateStatus();
}

void AckRailWindow::createFileMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    QAction *setSimulationAction = fileMenu->addAction(QStringLiteral("Set &Simulation..."));
    QObject::connect(setSimulationAction, &QAction::triggered, this, [this]()
                     {
                         simulationController.configure(topologyFile, weightFile, centralStack, emptyPage, this);
                         updateStatus();
                     });

    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(QStringLiteral("&Quit"));
    QObject::connect(quitAction, &QAction::triggered, this, [this]()
                     { close(); });
}

void AckRailWindow::createSimulationMenu()
{
    QMenu *simulationMenu = menuBar()->addMenu(QStringLiteral("&Simulation"));
    launchAction = simulationMenu->addAction(QStringLiteral("&Launch Simulation"));
    QObject::connect(launchAction, &QAction::triggered, this, [this]()
                     {
                         closeStatisticsDocks();
                         simulationController.launch(topologyFile, weightFile, centralStack, this);
                         simulationController.setEngineVisibility(showPassengerEnginesAction->isChecked(), showRebalancingEnginesAction->isChecked());
                         updateStatus();
                     });

    stopAction = simulationMenu->addAction(QStringLiteral("&Stop Simulation"));
    QObject::connect(stopAction, &QAction::triggered, this, [this]()
                     {
                         closeStatisticsDocks();
                         simulationController.clear(centralStack, emptyPage);
                         updateStatus();
                     });

    createSimulationViewMenu(simulationMenu);
}

void AckRailWindow::createSimulationViewMenu(QMenu *simulationMenu)
{
    QMenu *viewMenu = simulationMenu->addMenu(QStringLiteral("&View"));
    showPassengerEnginesAction = viewMenu->addAction(QStringLiteral("Show &Passenger Engines"));
    showPassengerEnginesAction->setCheckable(true);
    showPassengerEnginesAction->setChecked(true);
    QObject::connect(showPassengerEnginesAction, &QAction::toggled, this, [this]()
                     {
                         simulationController.setEngineVisibility(showPassengerEnginesAction->isChecked(), showRebalancingEnginesAction->isChecked());
                     });

    showRebalancingEnginesAction = viewMenu->addAction(QStringLiteral("Show &Rebalancing Engines"));
    showRebalancingEnginesAction->setCheckable(true);
    showRebalancingEnginesAction->setChecked(true);
    QObject::connect(showRebalancingEnginesAction, &QAction::toggled, this, [this]()
                     {
                         simulationController.setEngineVisibility(showPassengerEnginesAction->isChecked(), showRebalancingEnginesAction->isChecked());
                     });

    createStatisticsViewMenu(viewMenu);
}

void AckRailWindow::createStatisticsViewMenu(QMenu *viewMenu)
{
    QMenu *statisticsMenu = viewMenu->addMenu(QStringLiteral("&Statistics"));
    showEngineStatisticsAction = statisticsMenu->addAction(QStringLiteral("&Engine Statistics"));
    QObject::connect(showEngineStatisticsAction, &QAction::triggered, this, [this]()
                     { showEngineStatistics(); });

    showStationStatisticsAction = statisticsMenu->addAction(QStringLiteral("&Station Statistics"));
    QObject::connect(showStationStatisticsAction, &QAction::triggered, this, [this]()
                     { showStationStatistics(); });

    showDispatchStatisticsAction = statisticsMenu->addAction(QStringLiteral("&Dispatch Statistics"));
    QObject::connect(showDispatchStatisticsAction, &QAction::triggered, this, [this]()
                     { showDispatchStatistics(); });
}

void AckRailWindow::closeStatisticsDocks()
{
    if (engineStatisticsDock != nullptr)
        delete engineStatisticsDock.data();
    if (stationStatisticsDock != nullptr)
        delete stationStatisticsDock.data();
    if (dispatchStatisticsDock != nullptr)
        delete dispatchStatisticsDock.data();
}

void AckRailWindow::showEngineStatistics()
{
    SimulationStatistics *statistics = simulationController.getStatistics();
    if (statistics == nullptr)
        return;
    if (engineStatisticsDock == nullptr)
        engineStatisticsDock = createStatisticsDock(QStringLiteral("Engine Statistics"), new EngineStatisticsWidget(*statistics, this));
    engineStatisticsDock->show();
    engineStatisticsDock->raise();
}

void AckRailWindow::showStationStatistics()
{
    SimulationStatistics *statistics = simulationController.getStatistics();
    if (statistics == nullptr)
        return;
    if (stationStatisticsDock == nullptr)
        stationStatisticsDock = createStatisticsDock(QStringLiteral("Station Statistics"), new StationStatisticsWidget(*statistics, this));
    stationStatisticsDock->show();
    stationStatisticsDock->raise();
}

void AckRailWindow::showDispatchStatistics()
{
    SimulationStatistics *statistics = simulationController.getStatistics();
    if (statistics == nullptr)
        return;
    if (dispatchStatisticsDock == nullptr)
        dispatchStatisticsDock = createStatisticsDock(QStringLiteral("Dispatch Statistics"), new DispatchStatisticsWidget(*statistics, this));
    dispatchStatisticsDock->show();
    dispatchStatisticsDock->raise();
}

QDockWidget *AckRailWindow::createStatisticsDock(const QString &title, QWidget *content)
{
    auto *dock = new QDockWidget(title, this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(content);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    return dock;
}

void AckRailWindow::updateStatus()
{
    const QString topologyName = topologyFile.startsWith(QStringLiteral(":/")) ? topologyFile : QFileInfo(topologyFile).fileName();
    const QString weightName = weightFile.isEmpty() ? QStringLiteral("none") : QFileInfo(weightFile).fileName();
    const QString simulationState = simulationController.isRunning() ? QStringLiteral("running") : QStringLiteral("stopped");
    statusBar()->showMessage(QStringLiteral("Topology: %1 | Weights: %2 | Simulation: %3").arg(topologyName, weightName, simulationState));
    if (launchAction != nullptr)
        launchAction->setEnabled(!topologyFile.isEmpty());
    if (stopAction != nullptr)
        stopAction->setEnabled(simulationController.isRunning());
    const bool statisticsAvailable = simulationController.getStatistics() != nullptr;
    if (showEngineStatisticsAction != nullptr)
        showEngineStatisticsAction->setEnabled(statisticsAvailable);
    if (showStationStatisticsAction != nullptr)
        showStationStatisticsAction->setEnabled(statisticsAvailable);
    if (showDispatchStatisticsAction != nullptr)
        showDispatchStatisticsAction->setEnabled(statisticsAvailable);
}
