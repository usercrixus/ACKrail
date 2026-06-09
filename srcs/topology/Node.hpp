#pragma once

#include <QString>

class Node
{
public:
    Node(int id, QString name, double latitude, double longitude);

    int getId() const;
    const QString &getName() const;
    double getLatitude() const;
    double getLongitude() const;

private:
    int id;
    QString name;
    double latitude;
    double longitude;
};
