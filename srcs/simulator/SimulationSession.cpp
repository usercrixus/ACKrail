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
    garage = std::make_unique<Garage>(70000);
    trafficManager = std::make_unique<TrafficManager>(*topology, *garage);
    statisticsCollector = std::make_unique<SimulationStatisticsCollector>(*topology, *garage, *trafficManager);
    trafficSimulator = std::make_unique<TrafficSimulator>(*trafficManager);
    trafficSimulator->setUpdateObserver(
        [this](double currentSimulationTimeSeconds)
        {
            statisticsCollector->update(currentSimulationTimeSeconds);
        });
    error.clear();
    return true;
}

SimulationWidget *SimulationSession::createWidget(QWidget *parent)
{
    if (topology == nullptr || garage == nullptr || trafficManager == nullptr)
        return nullptr;
    simulationWidget = new SimulationWidget(*topology, *garage, *trafficManager, parent);
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
    return statisticsCollector == nullptr
               ? nullptr
               : &statisticsCollector->getStatistics();
}

const Topology *SimulationSession::getTopology() const
{
    return topology.get();
}
