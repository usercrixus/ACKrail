#pragma once

#include "Route.hpp"
#include "../topology/Link.hpp"
#include "../topology/Node.hpp"
#include <QVector>
#include <cstddef>
#include <unordered_map>
#include <vector>

class StaticRouteCostMatrix
{
public:
    StaticRouteCostMatrix(QVector<Node> &nodes, QVector<Link> &links);

    Route *findRoute(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds) const;
    double getDistanceKilometers(int fromNodeId, int toNodeId) const;
    std::vector<int> getLinkIds(int fromNodeId, int toNodeId) const;

private:
    struct Edge
    {
        int toNodeId;
        int linkId;
        double distanceKilometers;
    };

    struct SearchResult
    {
        std::vector<double> distancesKilometers;
        std::vector<int> predecessorNodeIds;
        std::vector<int> predecessorLinkIds;
    };

    struct QueueEntry
    {
        double distanceKilometers;
        int nodeId;
    };

    struct FartherDistance
    {
        bool operator()(const QueueEntry &left, const QueueEntry &right) const
        {
            return left.distanceKilometers > right.distanceKilometers;
        }
    };

    SearchResult findRoutesFrom(int fromNodeId) const;
    std::vector<int> buildLinkPath(int fromNodeId, int toNodeId, const SearchResult &result) const;
    std::size_t getNodeIndex(int nodeId) const;

    std::unordered_map<int, std::size_t> nodeIndexById;
    std::unordered_map<int, Node *> nodesById;
    std::unordered_map<int, Link *> linksById;
    std::unordered_map<int, std::vector<Edge>> edgesByNodeId;
    std::vector<std::vector<double>> distancesKilometers;
    std::vector<std::vector<std::vector<int>>> linkIdsByNodeIndex;
};
