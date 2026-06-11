#include "RouteFinder.hpp"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <cmath>
#include <limits>
#include <vector>

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
        const auto from = verticesByStationId.find(link.getFromNode().getId());
        const auto to = verticesByStationId.find(link.getToNode().getId());
        if (from == verticesByStationId.end() || to == verticesByStationId.end())
            continue;

        boost::add_edge(
            from->second,
            to->second,
            boost::property<boost::edge_weight_t, double>(link.getDistanceKilometers()),
            graph);
    }
}

std::optional<RouteFinder::Route> RouteFinder::findShortestRoute(
    int fromStationId,
    int toStationId) const
{
    const auto from = verticesByStationId.find(fromStationId);
    const auto to = verticesByStationId.find(toStationId);
    if (from == verticesByStationId.end() || to == verticesByStationId.end())
        return std::nullopt;

    const std::size_t vertexCount = boost::num_vertices(graph);
    std::vector<Vertex> predecessors(vertexCount);
    std::vector<double> distances(vertexCount, std::numeric_limits<double>::infinity());
    const auto vertexIndexMap = boost::get(boost::vertex_index, graph);

    boost::dijkstra_shortest_paths(
        graph,
        from->second,
        boost::predecessor_map(
            boost::make_iterator_property_map(predecessors.begin(), vertexIndexMap))
            .distance_map(
                boost::make_iterator_property_map(distances.begin(), vertexIndexMap)));

    const double totalDistance = distances[vertexIndexMap[to->second]];
    if (!std::isfinite(totalDistance))
        return std::nullopt;

    std::vector<Vertex> reversedPath;
    Vertex current = to->second;
    while (current != from->second)
    {
        reversedPath.push_back(current);
        const Vertex predecessor = predecessors[vertexIndexMap[current]];
        if (predecessor == current || reversedPath.size() > vertexCount)
            return std::nullopt;
        current = predecessor;
    }
    reversedPath.push_back(from->second);

    Route route{{}, totalDistance};
    route.stations.reserve(static_cast<qsizetype>(reversedPath.size()));
    const auto stationIdMap = boost::get(boost::vertex_name, graph);
    for (auto vertex = reversedPath.rbegin(); vertex != reversedPath.rend(); ++vertex)
        route.stations.append(stationsById.at(stationIdMap[*vertex]));

    return route;
}
