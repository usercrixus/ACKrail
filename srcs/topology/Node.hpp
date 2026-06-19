#pragma once

#include "../simulator/NodeController.hpp"
#include <QString>

class Node
{
public:
    Node(int id, QString name, double latitude, double longitude);

    int getId() const;
    const QString &getName() const;
    double getLatitude() const;
    double getLongitude() const;
    double getArrivalWeight() const;
    double getDepartureWeight() const;
    void setWeights(double arrivalWeight, double departureWeight);
    NodeController &getController();
    const NodeController &getController() const;

private:
    int id;
    QString name;
    double latitude;
    double longitude;
    double arrivalWeight = 1.0;
    double departureWeight = 1.0;
    NodeController controller;
};
