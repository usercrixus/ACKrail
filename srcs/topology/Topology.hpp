#pragma once

#include "Link.hpp"
#include "Node.hpp"
#include <QString>
#include <QVector>

class Topology
{
public:
    Topology();

    const QVector<Node> &getNodes() const;
    const QVector<Link> &getLinks() const;
    const QString &getName() const;
    const QString &getError() const;
    double getMinimumLatitude() const;
    double getMaximumLatitude() const;
    double getMinimumLongitude() const;
    double getMaximumLongitude() const;

    bool load(const QString &fileName);

private:
    void clear(const QString &error);

    QVector<Node> nodes;
    QVector<Link> links;
    QString name;
    QString error;
    double minimumLatitude;
    double maximumLatitude;
    double minimumLongitude;
    double maximumLongitude;
};
