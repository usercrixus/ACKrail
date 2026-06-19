#pragma once

#include "../garage/Garage.hpp"
#include "../topology/Topology.hpp"
#include "../view/TopologyWidget.hpp"
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
    explicit SimulationSession(QString topologyFile, QString weightFile = QString(), QObject *parent = nullptr);

    bool load();
    TopologyWidget *createWidget(QWidget *parent = nullptr);
    void start();

    const QString &getError() const;
    const QString &getTopologyFile() const;
    const QString &getWeightFile() const;

private:
    QString topologyFile;
    QString weightFile;
    QString error;
    std::unique_ptr<Topology> topology;
    std::unique_ptr<Garage> garage;
    std::unique_ptr<TrafficManager> trafficManager;
    std::unique_ptr<TrafficGenerator> trafficGenerator;
    std::unique_ptr<TrafficBalancer> trafficBalancer;
    std::unique_ptr<TrafficSimulator> trafficSimulator;
    QPointer<TopologyWidget> topologyWidget;
    QTimer renderTimer;
};
