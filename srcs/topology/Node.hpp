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
    NodeController &getController();
    const NodeController &getController() const;

private:
    int id;
    QString name;
    double latitude;
    double longitude;
    NodeController controller;
};
