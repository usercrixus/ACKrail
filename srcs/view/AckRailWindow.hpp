#pragma once

#include "../simulator/SimulationSession.hpp"

#include <QAction>
#include <QMainWindow>
#include <QString>
#include <memory>

class QStackedWidget;
class QWidget;

class AckRailWindow : public QMainWindow
{
public:
    explicit AckRailWindow(QWidget *parent = nullptr);

private:
    void createMenus();
    void createCentralView();
    void setSimulation();
    void launchSimulation();
    void stopSimulation();
    void clearSimulation();
    void updateStatus();

    QString topologyFile = QStringLiteral(":/map/paris_metro.json");
    QString weightFile;
    std::unique_ptr<SimulationSession> simulationSession;
    QStackedWidget *centralStack = nullptr;
    QWidget *emptyPage = nullptr;
    QWidget *simulationPage = nullptr;
    QAction *launchAction = nullptr;
    QAction *stopAction = nullptr;
};
