#pragma once

#include "Route.hpp"
#include "../topology/Link.hpp"
#include "../topology/Node.hpp"
#include <QVector>
#include <queue>
#include <unordered_map>
#include <vector>

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
    /** Node waiting to be processed, with its earliest known arrival time. */
    struct QueueEntry
    {
        /** Earliest known arrival time at the node. */
        double arrivalTimeSeconds;

        /** Identifier of the node to process. */
        int nodeId;
    };

    /** Orders queue entries from earliest to latest arrival. */
    struct LaterArrival
    {
        /** Returns true when the left entry arrives later than the right one. */
        bool operator()(const QueueEntry &left, const QueueEntry &right) const
        {
            return left.arrivalTimeSeconds > right.arrivalTimeSeconds;
        }
    };

    /** Information used to reconstruct the route to a node. */
    struct Predecessor
    {
        /** Identifier of the previous node. */
        int nodeId;

        /** Link used to reach the current node. */
        Link *link;

        /** Time at which the link traversal starts. */
        double entryTimeSeconds;

        /** Time required to traverse the link. */
        double traversalSeconds;
    };

    /** Mutable data collected while searching for the earliest route. */
    struct SearchState
    {
        /** Nodes waiting to be processed, ordered by arrival time. */
        std::priority_queue<QueueEntry, std::vector<QueueEntry>, LaterArrival> queue;

        /** Earliest known arrival time for each node identifier. */
        std::unordered_map<int, double> arrivalTimes;

        /** Previous route step for each reached node identifier. */
        std::unordered_map<int, Predecessor> predecessors;
    };

    bool isValidRequest(int fromNodeId, int toNodeId, double speedKilometersPerHour, double entrySeparationSeconds) const;
    SearchState searchEarliestArrivals(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds) const;

    void relaxOutgoingLinks(
        const QueueEntry &current,
        double speedKilometersPerHour,
        double entrySeparationSeconds,
        SearchState &state) const;

    void relaxLink(
        Link *link,
        const QueueEntry &current,
        double speedKilometersPerHour,
        double entrySeparationSeconds,
        SearchState &state) const;

    Route *buildRoute(
        int fromNodeId,
        int toNodeId,
        const SearchState &state) const;

    double getArrivalTime(int nodeId, const SearchState &state) const;

    std::unordered_map<int, Node *> nodesById;
    std::unordered_map<int, QVector<Link *>> linksByNodeId;
};
