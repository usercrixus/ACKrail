#pragma once

#include "../../topology/Link.hpp"
#include <QString>
#include <QVector>

class Engine
{
public:
    virtual ~Engine();
    const QString &getModelName() const;
    double getMaximumSpeedKilometersPerHour() const;
    double getCurrentSpeedKilometersPerHour() const;
    const Link *getCurrentLink() const;
    const Node *getFromNode() const;
    const Node *getToNode() const;
    double getDistanceOnCurrentLinkKilometers() const;
    double getCurrentLinkProgress() const;
    bool isActive() const;
    void setModelName(QString modelName);
    void setMaximumSpeedKilometersPerHour(double maximumSpeedKilometersPerHour);
    void setCurrentSpeedKilometersPerHour(double speedKilometersPerHour);
    void placeOnLink(const Link &link);
    bool assignRoute(const QVector<const Link *> &links, int startStationId);
    void advance(double elapsedSeconds);

protected:
    Engine();

private:
    struct RouteSegment
    {
        const Link *link;
        int fromStationId;
        int toStationId;
    };

    const Node *nodeOnCurrentLink(int stationId) const;
    void finishCurrentSegment();

    QString modelName;
    double maximumSpeedKilometersPerHour = 0.0;
    double currentSpeedKilometersPerHour = 0.0;
    QVector<RouteSegment> route;
    qsizetype currentSegmentIndex = -1;
    double distanceOnCurrentLinkKilometers = 0.0;
};
