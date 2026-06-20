#pragma once

#include "Link.hpp"
#include "Node.hpp"
#include <QString>
#include <QVector>
#include <vector>

class Topology
{
public:
    Topology();
    Topology(QVector<Node> nodes, QVector<Link> links);

    QVector<Node> &getNodes();
    const QVector<Node> &getNodes() const;
    QVector<Link> &getLinks();
    const QVector<Link> &getLinks() const;
    Node *findNode(int nodeId);
    const Node *findNode(int nodeId) const;
    const QString &getName() const;
    const QString &getError() const;
    double getMinimumLatitude() const;
    double getMaximumLatitude() const;
    double getMinimumLongitude() const;
    double getMaximumLongitude() const;
    const std::vector<double> &getArrivalWeights() const;
    const std::vector<double> &getDepartureWeights() const;

    bool loadTopology(const QString &fileName);
    bool loadWeights(const QString &fileName);

private:
    void clear(const QString &error);
    void resetWeights();

    QVector<Node> nodes;
    QVector<Link> links;
    std::vector<double> arrivalWeights;
    std::vector<double> departureWeights;
    QString name;
    QString error;
    double minimumLatitude;
    double maximumLatitude;
    double minimumLongitude;
    double maximumLongitude;
};
