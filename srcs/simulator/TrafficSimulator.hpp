#pragma once

#include "TrafficGenerator.hpp"
#include "TrafficManager.hpp"
#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

class TrafficSimulator : public QObject
{
    Q_OBJECT

public:
    TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator, QObject *parent = nullptr);
    void start();
    void stop();

signals:
    void advanced();

private:
    void advance();
    TrafficManager &trafficManager;
    TrafficGenerator &trafficGenerator;
    QTimer timer;
    QElapsedTimer clock;
};
