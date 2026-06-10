#pragma once

#include <QString>

class Engine
{
public:
    virtual ~Engine();
    const QString &getModelName() const;
    double getMaximumSpeedKilometersPerHour() const;
    double getCurrentSpeedKilometersPerHour() const;
    void setModelName(QString modelName);
    void setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour);
    void setCurrentSpeedKilometersPerHour(double speedKilometersPerHour);

protected:
    Engine();

private:
    QString modelName;
    double maximumSpeedKilometersPerHour = 0.0;
    double currentSpeedKilometersPerHour = 0.0;
};
