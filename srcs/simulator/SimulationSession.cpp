#include "SimulationSession.hpp"

#include <utility>

SimulationSession::SimulationSession(QString topologyFile, QString weightFile, QObject *parent)
    : QObject(parent),
      topologyFile(std::move(topologyFile)),
      weightFile(std::move(weightFile))
{
    renderTimer.setInterval(33);
    QObject::connect(&renderTimer, &QTimer::timeout, this, [this]()
                     {
                         if (simulationWidget != nullptr && trafficSimulator != nullptr)
                             simulationWidget->refresh(trafficSimulator->getCurrentSimulationTimeSeconds()); });
}

bool SimulationSession::load()
{
    topology = std::make_unique<Topology>();
    if (!topologyFile.isEmpty() && !topology->loadTopology(topologyFile))
    {
        error = topology->getError();
        return false;
    }
    if (!weightFile.isEmpty() && !topology->loadWeights(weightFile))
    {
        error = topology->getError();
        return false;
    }
    statistics = std::make_unique<SimulationStatistics>();
    garage = std::make_unique<Garage>(50000);
    trafficManager = std::make_unique<TrafficManager>(*topology, *garage, *statistics);
    trafficGenerator = std::make_unique<TrafficGenerator>(*topology, *garage, *trafficManager);
    trafficBalancer = std::make_unique<TrafficBalancer>(*topology, *garage, *trafficManager);
    trafficSimulator = std::make_unique<TrafficSimulator>(*topology, *garage, *trafficManager, *trafficGenerator, *trafficBalancer, *statistics);
    error.clear();
    return true;
}

SimulationWidget *SimulationSession::createWidget(QWidget *parent)
{
    if (topology == nullptr || garage == nullptr || trafficManager == nullptr || trafficBalancer == nullptr)
        return nullptr;
    simulationWidget = new SimulationWidget(*topology, *garage, *trafficManager, *trafficBalancer, parent);
    return simulationWidget;
}

void SimulationSession::start()
{
    if (trafficSimulator == nullptr)
        return;
    trafficSimulator->start();
    renderTimer.start();
}

const QString &SimulationSession::getError() const
{
    return error;
}

const QString &SimulationSession::getTopologyFile() const
{
    return topologyFile;
}

const QString &SimulationSession::getWeightFile() const
{
    return weightFile;
}

SimulationStatistics *SimulationSession::getStatistics() const
{
    return statistics.get();
}

const Topology *SimulationSession::getTopology() const
{
    return topology.get();
}
