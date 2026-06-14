#include "Dijkstra.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

Dijkstra::Dijkstra(QVector<Node> &nodes, QVector<Link> &links)
{
    for (Node &node : nodes)
        nodesById.emplace(node.getId(), &node);

    for (Link &link : links)
    {
        linksByNodeId[link.getFromNode().getId()].append(&link);
        linksByNodeId[link.getToNode().getId()].append(&link);
    }
}

Route *Dijkstra::findRoute(
    int fromNodeId,
    int toNodeId,
    double departureTimeSeconds,
    double speedKilometersPerHour,
    double entrySeparationSeconds)
{
    if (fromNodeId == toNodeId
        || !nodesById.contains(fromNodeId)
        || !nodesById.contains(toNodeId)
        || speedKilometersPerHour <= 0.0
        || entrySeparationSeconds < 0.0)
        return nullptr;

    struct QueueEntry
    {
        double arrivalTimeSeconds;
        int nodeId;
    };
    struct LaterArrival
    {
        bool operator()(const QueueEntry &left, const QueueEntry &right) const
        {
            return left.arrivalTimeSeconds > right.arrivalTimeSeconds;
        }
    };
    struct Predecessor
    {
        int nodeId;
        Link *link;
        double entryTimeSeconds;
        double traversalSeconds;
    };

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, LaterArrival> queue;
    std::unordered_map<int, double> arrivalTimes;
    std::unordered_map<int, Predecessor> predecessors;
    for (const auto &[nodeId, node] : nodesById)
        arrivalTimes[nodeId] = std::numeric_limits<double>::infinity();

    arrivalTimes[fromNodeId] = departureTimeSeconds;
    queue.push({departureTimeSeconds, fromNodeId});

    while (!queue.empty())
    {
        const QueueEntry current = queue.top();
        queue.pop();
        if (current.arrivalTimeSeconds > arrivalTimes[current.nodeId])
            continue;
        if (current.nodeId == toNodeId)
            break;

        const auto outgoingLinks = linksByNodeId.find(current.nodeId);
        if (outgoingLinks == linksByNodeId.end())
            continue;

        for (Link *link : outgoingLinks->second)
        {
            Node &departureNode = *nodesById.at(current.nodeId);
            const int destinationNodeId =
                link->getFromNode().getId() == current.nodeId
                ? link->getToNode().getId()
                : link->getFromNode().getId();
            const double entryTimeSeconds =
                departureNode.getController().findEarliestEntryTime(
                link->getId(),
                current.arrivalTimeSeconds,
                entrySeparationSeconds);
            const double traversalSeconds =
                link->getDistanceKilometers()
                / speedKilometersPerHour
                * 3600.0;
            const double destinationArrivalTime =
                entryTimeSeconds + traversalSeconds;

            if (destinationArrivalTime < arrivalTimes[destinationNodeId])
            {
                arrivalTimes[destinationNodeId] = destinationArrivalTime;
                predecessors.insert_or_assign(
                    destinationNodeId,
                    Predecessor{
                        current.nodeId,
                        link,
                        entryTimeSeconds,
                        traversalSeconds});
                queue.push({destinationArrivalTime, destinationNodeId});
            }
        }
    }

    if (!std::isfinite(arrivalTimes[toNodeId]))
        return nullptr;

    QVector<int> nodeIds;
    QVector<Link *> links;
    QVector<Route::ContractStep> contract;
    int currentNodeId = toNodeId;
    while (currentNodeId != fromNodeId)
    {
        const auto predecessor = predecessors.find(currentNodeId);
        if (predecessor == predecessors.end())
            return nullptr;

        nodeIds.prepend(currentNodeId);
        links.prepend(predecessor->second.link);
        contract.prepend({
            predecessor->second.entryTimeSeconds
                - arrivalTimes[predecessor->second.nodeId],
            predecessor->second.traversalSeconds});
        currentNodeId = predecessor->second.nodeId;
    }
    nodeIds.prepend(fromNodeId);

    QVector<Node *> stations;
    stations.reserve(nodeIds.size());
    for (const int nodeId : nodeIds)
        stations.append(nodesById.at(nodeId));

    Route *route = new Route(
        std::move(stations),
        std::move(links),
        std::move(contract));
    if (!route->hasValidContract())
    {
        delete route;
        return nullptr;
    }
    return route;
}
