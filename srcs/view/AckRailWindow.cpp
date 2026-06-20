#include "AckRailWindow.hpp"

#include "dialogs/SimulationConfigDialog.hpp"
#include "simulation/EmptyPageWidget.hpp"
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
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

void AckRailWindow::showDimOverlay(QWidget &overlay)
{
    overlay.setAttribute(Qt::WA_StyledBackground, true);
    overlay.setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 150);"));
    overlay.setGeometry(rect());
    overlay.show();
    overlay.raise();
}

void AckRailWindow::createCentralView()
{
    centralStack = new QStackedWidget(this);
    emptyPage = new EmptyPageWidget(centralStack);
    simulationPage = nullptr;
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
                     { setSimulation(); });

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
                     { launchSimulation(); });

    stopAction = simulationMenu->addAction(QStringLiteral("&Stop Simulation"));
    QObject::connect(stopAction, &QAction::triggered, this, [this]()
                     { stopSimulation(); });

    createSimulationViewMenu(simulationMenu);
}

void AckRailWindow::createSimulationViewMenu(QMenu *simulationMenu)
{
    QMenu *viewMenu = simulationMenu->addMenu(QStringLiteral("&View"));
    showPassengerEnginesAction = viewMenu->addAction(QStringLiteral("Show &Passenger Engines"));
    showPassengerEnginesAction->setCheckable(true);
    showPassengerEnginesAction->setChecked(true);
    QObject::connect(showPassengerEnginesAction, &QAction::toggled, this, [this]()
                     { applyEngineVisibilitySettings(); });

    showRebalancingEnginesAction = viewMenu->addAction(QStringLiteral("Show &Rebalancing Engines"));
    showRebalancingEnginesAction->setCheckable(true);
    showRebalancingEnginesAction->setChecked(true);
    QObject::connect(showRebalancingEnginesAction, &QAction::toggled, this, [this]()
                     { applyEngineVisibilitySettings(); });
}

void AckRailWindow::setSimulation()
{
    const bool simulationWasVisible = simulationPage != nullptr && centralStack->currentWidget() == simulationPage;
    if (simulationWasVisible)
        centralStack->setCurrentWidget(emptyPage);

    SimulationConfigDialog dialog(topologyFile, weightFile, this);
    QWidget overlay(this);

    const auto restoreSimulationPage = [this, simulationWasVisible]()
    {
        if (simulationWasVisible && simulationPage != nullptr)
            centralStack->setCurrentWidget(simulationPage);
    };

    showDimOverlay(overlay);

    if (dialog.exec() != QDialog::Accepted)
    {
        restoreSimulationPage();
        return;
    }

    const QString nextTopologyFile = dialog.getTopologyFile();
    if (nextTopologyFile.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("Cannot set simulation"), QStringLiteral("Topology path cannot be empty."));
        restoreSimulationPage();
        return;
    }

    stopSimulation();
    topologyFile = nextTopologyFile;
    weightFile = dialog.getWeightFile();
    updateStatus();
}

void AckRailWindow::launchSimulation()
{
    clearSimulation();

    auto nextSession = std::make_unique<SimulationSession>(topologyFile, weightFile, this);
    if (!nextSession->load())
    {
        QMessageBox::warning(this, QStringLiteral("Cannot launch simulation"), nextSession->getError());
        updateStatus();
        return;
    }

    SimulationWidget *widget = nextSession->createWidget(centralStack);
    if (widget == nullptr)
    {
        QMessageBox::warning(this, QStringLiteral("Cannot launch simulation"), QStringLiteral("Simulation view could not be created."));
        updateStatus();
        return;
    }

    simulationPage = widget;
    centralStack->addWidget(simulationPage);
    centralStack->setCurrentWidget(simulationPage);
    simulationSession = std::move(nextSession);
    applyEngineVisibilitySettings();
    simulationSession->start();
    updateStatus();
}

void AckRailWindow::stopSimulation()
{
    clearSimulation();
    updateStatus();
}

void AckRailWindow::clearSimulation()
{
    if (centralStack != nullptr && emptyPage != nullptr)
        centralStack->setCurrentWidget(emptyPage);

    if (simulationPage != nullptr)
    {
        centralStack->removeWidget(simulationPage);
        delete simulationPage;
        simulationPage = nullptr;
    }
    simulationSession.reset();
}

void AckRailWindow::updateStatus()
{
    const QString topologyName = topologyFile.startsWith(QStringLiteral(":/")) ? topologyFile : QFileInfo(topologyFile).fileName();
    const QString weightName = weightFile.isEmpty() ? QStringLiteral("none") : QFileInfo(weightFile).fileName();
    const QString simulationState = simulationSession == nullptr ? QStringLiteral("stopped") : QStringLiteral("running");
    statusBar()->showMessage(QStringLiteral("Topology: %1 | Weights: %2 | Simulation: %3").arg(topologyName, weightName, simulationState));
    if (launchAction != nullptr)
        launchAction->setEnabled(!topologyFile.isEmpty());
    if (stopAction != nullptr)
        stopAction->setEnabled(simulationSession != nullptr);
}

void AckRailWindow::applyEngineVisibilitySettings()
{
    if (simulationPage == nullptr)
        return;
    if (showPassengerEnginesAction != nullptr)
        simulationPage->setPassengerEnginesVisible(showPassengerEnginesAction->isChecked());
    if (showRebalancingEnginesAction != nullptr)
        simulationPage->setRebalancingEnginesVisible(showRebalancingEnginesAction->isChecked());
}
