#include "SimulationController.hpp"

#include "dialogs/SimulationConfigDialog.hpp"
#include <QMessageBox>
#include <QStackedWidget>
#include <QWidget>

void SimulationController::launch(const QString &topologyFile, const QString &weightFile, QStackedWidget *centralStack, QWidget *parent)
{
    clear(centralStack, nullptr);

    auto nextSession = std::make_unique<SimulationSession>(topologyFile, weightFile, parent);
    if (!nextSession->load())
    {
        QMessageBox::warning(parent, QStringLiteral("Cannot launch simulation"), nextSession->getError());
        return;
    }

    SimulationWidget *widget = nextSession->createWidget(centralStack);
    if (widget == nullptr)
    {
        QMessageBox::warning(parent, QStringLiteral("Cannot launch simulation"), QStringLiteral("Simulation view could not be created."));
        return;
    }

    simulationPage = widget;
    centralStack->addWidget(simulationPage);
    centralStack->setCurrentWidget(simulationPage);
    simulationSession = std::move(nextSession);
    simulationSession->start();
}

void SimulationController::configure(QString &topologyFile, QString &weightFile, QStackedWidget *centralStack, QWidget *emptyPage, QWidget *parent)
{
    const bool simulationWasVisible = simulationPage != nullptr && centralStack->currentWidget() == simulationPage;
    if (simulationWasVisible)
        centralStack->setCurrentWidget(emptyPage);

    SimulationConfigDialog dialog(topologyFile, weightFile, parent);
    QWidget overlay(parent);
    overlay.setAttribute(Qt::WA_StyledBackground, true);
    overlay.setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 150);"));
    overlay.setGeometry(parent->rect());
    overlay.show();
    overlay.raise();

    const auto restoreSimulationPage = [this, centralStack, simulationWasVisible]()
    {
        if (simulationWasVisible && simulationPage != nullptr)
            centralStack->setCurrentWidget(simulationPage);
    };

    if (dialog.exec() != QDialog::Accepted)
    {
        restoreSimulationPage();
        return;
    }

    const QString nextTopologyFile = dialog.getTopologyFile();
    if (nextTopologyFile.isEmpty())
    {
        QMessageBox::warning(parent, QStringLiteral("Cannot set simulation"), QStringLiteral("Topology path cannot be empty."));
        restoreSimulationPage();
        return;
    }

    clear(centralStack, emptyPage);
    topologyFile = nextTopologyFile;
    weightFile = dialog.getWeightFile();
}

void SimulationController::clear(QStackedWidget *centralStack, QWidget *emptyPage)
{
    if (centralStack != nullptr && emptyPage != nullptr)
        centralStack->setCurrentWidget(emptyPage);

    if (simulationPage != nullptr)
    {
        if (centralStack != nullptr)
            centralStack->removeWidget(simulationPage);
        delete simulationPage;
        simulationPage = nullptr;
    }
    simulationSession.reset();
}

bool SimulationController::isRunning() const
{
    return simulationSession != nullptr;
}

SimulationStatistics *SimulationController::getStatistics() const
{
    return simulationSession == nullptr ? nullptr : simulationSession->getStatistics();
}

SimulationWidget *SimulationController::getSimulationPage() const
{
    return simulationPage;
}

void SimulationController::setEngineVisibility(bool passengerEnginesVisible, bool rebalancingEnginesVisible)
{
    setPassengerEnginesVisible(passengerEnginesVisible);
    setRebalancingEnginesVisible(rebalancingEnginesVisible);
}

void SimulationController::setPassengerEnginesVisible(bool visible)
{
    if (simulationPage != nullptr)
        simulationPage->setPassengerEnginesVisible(visible);
}

void SimulationController::setRebalancingEnginesVisible(bool visible)
{
    if (simulationPage != nullptr)
        simulationPage->setRebalancingEnginesVisible(visible);
}
