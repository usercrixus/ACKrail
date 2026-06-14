#pragma once

#include "Route.hpp"
#include "../topology/Link.hpp"
#include "../topology/Node.hpp"
#include <QVector>
#include <unordered_map>

/**
 * Finds the earliest-arrival route through a time-dependent rail network.
 */
class Dijkstra
{
public:
    /**
     * Builds a reusable graph index from nodes and links.
     *
     * @param nodes Network nodes.
     * @param links Bidirectional network links.
     */
    Dijkstra(QVector<Node> &nodes, QVector<Link> &links);

    /**
     * Finds the route producing the earliest destination arrival.
     *
     * @param fromNodeId Departure node identifier.
     * @param toNodeId Destination node identifier.
     * @param departureTimeSeconds Requested journey departure time.
     * @param speedKilometersPerHour Constant link traversal speed.
     * @param entrySeparationSeconds Required time between link entries.
     * @return A newly allocated contracted route, or nullptr when no route
     * exists. The caller owns the returned route.
     */
    Route *findRoute(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds);

private:
    std::unordered_map<int, Node *> nodesById;
    std::unordered_map<int, QVector<Link *>> linksByNodeId;
};
