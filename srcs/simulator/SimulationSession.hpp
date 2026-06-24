#pragma once

#include "../garage/Garage.hpp"
#include "../statistics/SimulationStatistics.hpp"
#include "../topology/Topology.hpp"
#include "../view/simulation/SimulationWidget.hpp"
#include "TrafficBalancer.hpp"
#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include "TrafficSimulator.hpp"

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QString>
#include <memory>

class SimulationSession : public QObject
{
public:
    SimulationSession(QString topologyFile, QString weightFile = QString(), QObject *parent = nullptr);

    bool load();
    SimulationWidget *createWidget(QWidget *parent = nullptr);
    void start();

    const QString &getError() const;
    const QString &getTopologyFile() const;
    const QString &getWeightFile() const;
    SimulationStatistics *getStatistics() const;
    const Topology *getTopology() const;

private:
    QString topologyFile;
    QString weightFile;
    QString error;
    std::unique_ptr<Topology> topology;
    std::unique_ptr<SimulationStatistics> statistics;
    std::unique_ptr<Garage> garage;
    std::unique_ptr<TrafficManager> trafficManager;
    std::unique_ptr<TrafficGenerator> trafficGenerator;
    std::unique_ptr<TrafficBalancer> trafficBalancer;
    std::unique_ptr<TrafficSimulator> trafficSimulator;
    QPointer<SimulationWidget> simulationWidget;
    QTimer renderTimer;
};
