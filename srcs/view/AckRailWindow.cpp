#include "AckRailWindow.hpp"

#include "dialogs/StatisticsDialog.hpp"
#include "simulation/EmptyPageWidget.hpp"
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
                         simulationController.launch(topologyFile, weightFile, centralStack, this);
                         simulationController.setEngineVisibility(showPassengerEnginesAction->isChecked(), showRebalancingEnginesAction->isChecked());
                         updateStatus();
                     });

    stopAction = simulationMenu->addAction(QStringLiteral("&Stop Simulation"));
    QObject::connect(stopAction, &QAction::triggered, this, [this]()
                     {
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
    showStatisticsAction = viewMenu->addAction(QStringLiteral("&Statistics..."));
    QObject::connect(showStatisticsAction, &QAction::triggered, this, [this]()
                     { showStatistics(); });
}

void AckRailWindow::showStatistics()
{
    SimulationStatistics *statistics = simulationController.getStatistics();
    const Topology *topology = simulationController.getTopology();
    if (statistics == nullptr || topology == nullptr)
        return;

    QWidget overlay(this);
    overlay.setAttribute(Qt::WA_StyledBackground, true);
    overlay.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    overlay.setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 150);"));
    overlay.setGeometry(rect());
    overlay.show();
    overlay.raise();

    StatisticsDialog dialog(*statistics, *topology, this);
    dialog.exec();
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
    if (showStatisticsAction != nullptr)
        showStatisticsAction->setEnabled(statisticsAvailable);
}
