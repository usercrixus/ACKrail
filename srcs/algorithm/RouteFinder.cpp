#include "RouteFinder.hpp"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

static std::uint64_t stationPairKey(int firstStationId, int secondStationId)
{
    const auto first = static_cast<std::uint32_t>(std::min(firstStationId, secondStationId));
    const auto second = static_cast<std::uint32_t>(std::max(firstStationId, secondStationId));
    return (static_cast<std::uint64_t>(first) << 32) | second;
}

RouteFinder::RouteFinder(const Topology &topology)
    : RouteFinder(topology.getNodes(), topology.getLinks())
{
}

RouteFinder::RouteFinder(const QVector<Node> &nodes, const QVector<Link> &links)
{
    auto stationIdMap = boost::get(boost::vertex_name, graph);
    for (const Node &node : nodes)
    {
        const Vertex vertex = boost::add_vertex(graph);
        stationIdMap[vertex] = node.getId();
        verticesByStationId.emplace(node.getId(), vertex);
        stationsById.emplace(node.getId(), node);
    }
    for (const Link &link : links)
    {
        const std::unordered_map<int, Vertex>::iterator from = verticesByStationId.find(link.getFromNode().getId());
        const std::unordered_map<int, Vertex>::iterator to = verticesByStationId.find(link.getToNode().getId());
        if (from != verticesByStationId.end() && to != verticesByStationId.end())
        {
            boost::add_edge(from->second, to->second, boost::property<boost::edge_weight_t, double>(link.getDistanceKilometers()), graph);
            const std::uint64_t key = stationPairKey(link.getFromNode().getId(), link.getToNode().getId());
            const std::unordered_map<uint64_t, const Link *>::iterator existingLink = linksByStationPair.find(key);
            if (existingLink == linksByStationPair.end() || link.getDistanceKilometers() < existingLink->second->getDistanceKilometers())
                linksByStationPair[key] = &link;
        }
    }
}

std::optional<Route> RouteFinder::findShortestRoute(int fromStationId, int toStationId) const
{
    const auto from = verticesByStationId.find(fromStationId);
    const auto to = verticesByStationId.find(toStationId);
    if (from == verticesByStationId.end() || to == verticesByStationId.end())
        return std::nullopt;

    const std::optional<std::vector<Vertex>> vertexPath = findShortestVertexPath(from->second, to->second);
    if (!vertexPath.has_value())
        return std::nullopt;

    return createRoute(*vertexPath);
}

std::optional<std::vector<RouteFinder::Vertex>> RouteFinder::findShortestVertexPath(Vertex from, Vertex to) const
{
    const std::size_t vertexCount = boost::num_vertices(graph);
    std::vector<Vertex> predecessors(vertexCount);
    std::vector<double> distances(vertexCount, std::numeric_limits<double>::infinity());
    const auto vertexIndexMap = boost::get(boost::vertex_index, graph);

    boost::dijkstra_shortest_paths(graph, from, boost::predecessor_map(boost::make_iterator_property_map(predecessors.begin(), vertexIndexMap))
            .distance_map(boost::make_iterator_property_map(distances.begin(), vertexIndexMap)));

    if (!std::isfinite(distances[vertexIndexMap[to]]))
        return std::nullopt;

    return reconstructPath(from, to, predecessors);
}

std::optional<std::vector<RouteFinder::Vertex>> RouteFinder::reconstructPath(Vertex from, Vertex to, const std::vector<Vertex> &predecessors) const
{
    const std::size_t vertexCount = boost::num_vertices(graph);
    const auto vertexIndexMap = boost::get(boost::vertex_index, graph);
    std::vector<Vertex> reversedPath;
    Vertex current = to;
    while (current != from)
    {
        reversedPath.push_back(current);
        const Vertex predecessor = predecessors[vertexIndexMap[current]];
        if (predecessor == current || reversedPath.size() > vertexCount)
            return std::nullopt;
        current = predecessor;
    }
    reversedPath.push_back(from);
    return std::vector<Vertex>(reversedPath.rbegin(), reversedPath.rend());
}

std::optional<Route> RouteFinder::createRoute(const std::vector<Vertex> &vertexPath) const
{
    QVector<Node> routeStations;
    routeStations.reserve(static_cast<qsizetype>(vertexPath.size()));
    const auto stationIdMap = boost::get(boost::vertex_name, graph);
    for (const Vertex vertex : vertexPath)
        routeStations.append(stationsById.at(stationIdMap[vertex]));
    QVector<const Link *> routeLinks;
    routeLinks.reserve(std::max<qsizetype>(0, routeStations.size() - 1));
    for (qsizetype index = 1; index < routeStations.size(); ++index)
    {
        const std::uint64_t key = stationPairKey(routeStations[index - 1].getId(), routeStations[index].getId());
        const auto link = linksByStationPair.find(key);
        if (link == linksByStationPair.end())
            return std::nullopt;
        routeLinks.append(link->second);
    }
    return Route(std::move(routeStations), std::move(routeLinks));
}
