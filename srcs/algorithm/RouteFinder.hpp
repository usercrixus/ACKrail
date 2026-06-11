#pragma once

#include "Route.hpp"
#include "../topology/Link.hpp"
#include "../topology/Node.hpp"
#include "../topology/Topology.hpp"

#include <QVector>
#include <boost/graph/adjacency_list.hpp>
#include <cstdint>
#include <optional>
#include <unordered_map>

class RouteFinder
{
public:
    explicit RouteFinder(const Topology &topology);
    RouteFinder(const QVector<Node> &nodes, const QVector<Link> &links);

    std::optional<Route> findShortestRoute(int fromStationId, int toStationId) const;

private:
    using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        boost::property<boost::vertex_name_t, int>,
        boost::property<boost::edge_weight_t, double>>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph graph;
    std::unordered_map<int, Vertex> verticesByStationId;
    std::unordered_map<int, Node> stationsById;
    std::unordered_map<std::uint64_t, const Link *> linksByStationPair;
};
