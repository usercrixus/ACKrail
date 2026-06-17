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
    nodesById.reserve(nodes.size());
    linksByNodeId.reserve(nodes.size());

    for (Node &node : nodes)
        nodesById.emplace(node.getId(), &node);

    for (Link &link : links)
    {
        linksByNodeId[link.getFromNode().getId()].append(&link);
        linksByNodeId[link.getToNode().getId()].append(&link);
    }
}

Route *Dijkstra::findRoute(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds)
{
    if (!isValidRequest(fromNodeId, toNodeId, speedKilometersPerHour, entrySeparationSeconds))
        return nullptr;
    const SearchState state = searchEarliestArrivals(fromNodeId, toNodeId, departureTimeSeconds, speedKilometersPerHour, entrySeparationSeconds);
    if (!std::isfinite(getArrivalTime(toNodeId, state)))
        return nullptr;
    return buildRoute(fromNodeId, toNodeId, state);
}

bool Dijkstra::isValidRequest(int fromNodeId, int toNodeId, double speedKilometersPerHour, double entrySeparationSeconds) const
{
    return fromNodeId != toNodeId && nodesById.contains(fromNodeId) && nodesById.contains(toNodeId) && speedKilometersPerHour > 0.0 && entrySeparationSeconds >= 0.0;
}

Dijkstra::SearchState Dijkstra::searchEarliestArrivals(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds) const
{
    SearchState state;
    state.arrivalTimes.reserve(nodesById.size());
    state.predecessors.reserve(nodesById.size());

    state.arrivalTimes[fromNodeId] = departureTimeSeconds;
    state.queue.push({departureTimeSeconds, fromNodeId});
    while (!state.queue.empty())
    {
        const QueueEntry current = state.queue.top();
        state.queue.pop();
        if (current.arrivalTimeSeconds > getArrivalTime(current.nodeId, state))
            continue;
        if (current.nodeId == toNodeId)
            break;
        relaxOutgoingLinks(current, speedKilometersPerHour, entrySeparationSeconds, state);
    }
    return state;
}

void Dijkstra::relaxOutgoingLinks(const QueueEntry &current, double speedKilometersPerHour, double entrySeparationSeconds, SearchState &state) const
{
    const auto outgoingLinks = linksByNodeId.find(current.nodeId);
    if (outgoingLinks == linksByNodeId.end())
        return;
    for (Link *link : outgoingLinks->second)
        relaxLink(link, current, speedKilometersPerHour, entrySeparationSeconds, state);
}

void Dijkstra::relaxLink(Link *link, const QueueEntry &current, double speedKilometersPerHour, double entrySeparationSeconds, SearchState &state) const
{
    Node *departureNode = nodesById.at(current.nodeId);
    const int destinationNodeId = link->getFromNode().getId() == current.nodeId ? link->getToNode().getId() : link->getFromNode().getId();
    const double entryTimeSeconds = departureNode->getController().findEarliestEntryTime(link->getId(), current.arrivalTimeSeconds, entrySeparationSeconds);
    const double traversalSeconds = link->getDistanceKilometers() / speedKilometersPerHour * 3600.0;
    const double destinationArrivalTime = entryTimeSeconds + traversalSeconds;

    if (destinationArrivalTime >= getArrivalTime(destinationNodeId, state))
        return;

    state.arrivalTimes[destinationNodeId] = destinationArrivalTime;
    state.predecessors.insert_or_assign(destinationNodeId, Predecessor{current.nodeId, link, entryTimeSeconds, traversalSeconds});
    state.queue.push({destinationArrivalTime, destinationNodeId});
}

Route *Dijkstra::buildRoute(int fromNodeId, int toNodeId, const SearchState &state) const
{
    QVector<Node *> stations;
    QVector<Link *> links;
    QVector<Route::ContractStep> contract;
    int currentNodeId = toNodeId;
    while (currentNodeId != fromNodeId)
    {
        const auto predecessor = state.predecessors.find(currentNodeId);
        if (predecessor == state.predecessors.end())
            return nullptr;

        stations.append(nodesById.at(currentNodeId));
        links.append(predecessor->second.link);
        contract.append({predecessor->second.entryTimeSeconds - state.arrivalTimes.at(predecessor->second.nodeId), predecessor->second.traversalSeconds});
        currentNodeId = predecessor->second.nodeId;
    }
    stations.append(nodesById.at(fromNodeId));

    std::reverse(stations.begin(), stations.end());
    std::reverse(links.begin(), links.end());
    std::reverse(contract.begin(), contract.end());

    Route *route = new Route(std::move(stations), std::move(links), std::move(contract));
    if (!route->hasValidContract())
    {
        delete route;
        return nullptr;
    }
    return route;
}

double Dijkstra::getArrivalTime(int nodeId, const SearchState &state) const
{
    const auto arrivalTime = state.arrivalTimes.find(nodeId);
    if (arrivalTime == state.arrivalTimes.end())
        return std::numeric_limits<double>::infinity();
    return arrivalTime->second;
}
