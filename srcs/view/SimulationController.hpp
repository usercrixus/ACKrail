#pragma once

#include "../simulator/SimulationSession.hpp"
#include "simulation/SimulationWidget.hpp"

#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <memory>

class SimulationController
{
public:
    void launch(const QString &topologyFile, const QString &weightFile, QStackedWidget *centralStack, QWidget *parent);
    void configure(QString &topologyFile, QString &weightFile, QStackedWidget *centralStack, QWidget *emptyPage, QWidget *parent);
    void clear(QStackedWidget *centralStack, QWidget *emptyPage);

    bool isRunning() const;
    SimulationStatistics *getStatistics() const;
    SimulationWidget *getSimulationPage() const;
    void setEngineVisibility(bool passengerEnginesVisible, bool rebalancingEnginesVisible);
    void setPassengerEnginesVisible(bool visible);
    void setRebalancingEnginesVisible(bool visible);

private:
    std::unique_ptr<SimulationSession> simulationSession;
    SimulationWidget *simulationPage = nullptr;
};
