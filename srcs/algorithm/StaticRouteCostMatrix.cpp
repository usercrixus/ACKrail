#include "StaticRouteCostMatrix.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <utility>

StaticRouteCostMatrix::StaticRouteCostMatrix(QVector<Node> &nodes, QVector<Link> &links)
{
    nodeIndexById.reserve(static_cast<std::size_t>(nodes.size()));
    nodesById.reserve(static_cast<std::size_t>(nodes.size()));
    edgesByNodeId.reserve(static_cast<std::size_t>(nodes.size()));
    for (qsizetype index = 0; index < nodes.size(); ++index)
    {
        nodeIndexById.emplace(nodes[index].getId(), static_cast<std::size_t>(index));
        nodesById.emplace(nodes[index].getId(), &nodes[index]);
    }

    linksById.reserve(static_cast<std::size_t>(links.size()));
    for (Link &link : links)
    {
        const int fromNodeId = link.getFromNode().getId();
        const int toNodeId = link.getToNode().getId();
        const double distanceKilometers = link.getDistanceKilometers();
        linksById.emplace(link.getId(), &link);
        edgesByNodeId[fromNodeId].push_back({toNodeId, link.getId(), distanceKilometers});
        edgesByNodeId[toNodeId].push_back({fromNodeId, link.getId(), distanceKilometers});
    }

    distancesKilometers.reserve(static_cast<std::size_t>(nodes.size()));
    linkIdsByNodeIndex.reserve(static_cast<std::size_t>(nodes.size()));
    for (const Node &node : nodes)
    {
        const SearchResult result = findRoutesFrom(node.getId());
        distancesKilometers.push_back(result.distancesKilometers);

        std::vector<std::vector<int>> linkIdsFromNode;
        linkIdsFromNode.reserve(static_cast<std::size_t>(nodes.size()));
        for (const Node &targetNode : nodes)
            linkIdsFromNode.push_back(buildLinkPath(node.getId(), targetNode.getId(), result));
        linkIdsByNodeIndex.push_back(std::move(linkIdsFromNode));
    }
}

Route *StaticRouteCostMatrix::findRoute(int fromNodeId, int toNodeId, double departureTimeSeconds, double speedKilometersPerHour, double entrySeparationSeconds) const
{
    if (fromNodeId == toNodeId || speedKilometersPerHour <= 0.0 || entrySeparationSeconds < 0.0)
        return nullptr;

    const std::vector<int> linkIds = getLinkIds(fromNodeId, toNodeId);
    if (linkIds.empty())
        return nullptr;

    QVector<Node *> stations;
    QVector<Link *> links;
    QVector<Route::ContractStep> contract;
    stations.reserve(static_cast<qsizetype>(linkIds.size() + 1));
    links.reserve(static_cast<qsizetype>(linkIds.size()));
    contract.reserve(static_cast<qsizetype>(linkIds.size()));

    const auto startNode = nodesById.find(fromNodeId);
    if (startNode == nodesById.end())
        return nullptr;

    Node *currentStation = startNode->second;
    stations.append(currentStation);

    double arrivalTimeSeconds = departureTimeSeconds;
    for (const int linkId : linkIds)
    {
        const auto linkPosition = linksById.find(linkId);
        if (linkPosition == linksById.end())
            return nullptr;

        Link *link = linkPosition->second;
        const int currentStationId = currentStation->getId();
        const int fromLinkStationId = link->getFromNode().getId();
        const int toLinkStationId = link->getToNode().getId();
        const int nextStationId = currentStationId == fromLinkStationId ? toLinkStationId : fromLinkStationId;
        if (currentStationId != fromLinkStationId && currentStationId != toLinkStationId)
            return nullptr;

        const auto nextStation = nodesById.find(nextStationId);
        if (nextStation == nodesById.end())
            return nullptr;

        const double entryTimeSeconds = currentStation->getController().findEarliestEntryTime(link->getId(), arrivalTimeSeconds, entrySeparationSeconds);
        const double traversalSeconds = link->getDistanceKilometers() / speedKilometersPerHour * 3600.0;
        contract.append({entryTimeSeconds - arrivalTimeSeconds, traversalSeconds});
        arrivalTimeSeconds = entryTimeSeconds + traversalSeconds;

        currentStation = nextStation->second;
        links.append(link);
        stations.append(currentStation);
    }

    if (stations.back()->getId() != toNodeId)
        return nullptr;

    Route *route = new Route(std::move(stations), std::move(links), std::move(contract));
    if (!route->hasValidContract())
    {
        delete route;
        return nullptr;
    }
    return route;
}

double StaticRouteCostMatrix::getDistanceKilometers(int fromNodeId, int toNodeId) const
{
    const auto fromNode = nodeIndexById.find(fromNodeId);
    const auto toNode = nodeIndexById.find(toNodeId);
    if (fromNode == nodeIndexById.end() || toNode == nodeIndexById.end())
        return std::numeric_limits<double>::infinity();
    return distancesKilometers[fromNode->second][toNode->second];
}

std::vector<int> StaticRouteCostMatrix::getLinkIds(int fromNodeId, int toNodeId) const
{
    const auto fromNode = nodeIndexById.find(fromNodeId);
    const auto toNode = nodeIndexById.find(toNodeId);
    if (fromNode == nodeIndexById.end() || toNode == nodeIndexById.end())
        return {};
    return linkIdsByNodeIndex[fromNode->second][toNode->second];
}

StaticRouteCostMatrix::SearchResult StaticRouteCostMatrix::findRoutesFrom(int fromNodeId) const
{
    SearchResult result;
    result.distancesKilometers.assign(nodeIndexById.size(), std::numeric_limits<double>::infinity());
    result.predecessorNodeIds.assign(nodeIndexById.size(), -1);
    result.predecessorLinkIds.assign(nodeIndexById.size(), -1);
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, FartherDistance> queue;

    result.distancesKilometers[getNodeIndex(fromNodeId)] = 0.0;
    queue.push({0.0, fromNodeId});
    while (!queue.empty())
    {
        const QueueEntry current = queue.top();
        queue.pop();
        if (current.distanceKilometers > result.distancesKilometers[getNodeIndex(current.nodeId)])
            continue;

        const auto edges = edgesByNodeId.find(current.nodeId);
        if (edges == edgesByNodeId.end())
            continue;

        for (const Edge &edge : edges->second)
        {
            const double candidateDistance = current.distanceKilometers + edge.distanceKilometers;
            const std::size_t destinationIndex = getNodeIndex(edge.toNodeId);
            double &knownDistance = result.distancesKilometers[destinationIndex];
            if (candidateDistance >= knownDistance)
                continue;
            knownDistance = candidateDistance;
            result.predecessorNodeIds[destinationIndex] = current.nodeId;
            result.predecessorLinkIds[destinationIndex] = edge.linkId;
            queue.push({candidateDistance, edge.toNodeId});
        }
    }
    return result;
}

std::vector<int> StaticRouteCostMatrix::buildLinkPath(int fromNodeId, int toNodeId, const SearchResult &result) const
{
    if (fromNodeId == toNodeId || !std::isfinite(result.distancesKilometers[getNodeIndex(toNodeId)]))
        return {};

    std::vector<int> linkIds;
    int currentNodeId = toNodeId;
    while (currentNodeId != fromNodeId)
    {
        const std::size_t currentIndex = getNodeIndex(currentNodeId);
        const int predecessorNodeId = result.predecessorNodeIds[currentIndex];
        const int predecessorLinkId = result.predecessorLinkIds[currentIndex];
        if (predecessorNodeId < 0 || predecessorLinkId < 0)
            return {};
        linkIds.push_back(predecessorLinkId);
        currentNodeId = predecessorNodeId;
    }

    std::reverse(linkIds.begin(), linkIds.end());
    return linkIds;
}

std::size_t StaticRouteCostMatrix::getNodeIndex(int nodeId) const
{
    return nodeIndexById.at(nodeId);
}
