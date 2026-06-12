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
#include <vector>

/**
 * Finds shortest routes between stations in a rail topology.
 *
 * The topology is represented as a weighted Boost graph where stations are
 * vertices, links are edges, and link distances are edge weights.
 */
class RouteFinder
{
public:
    /**
     * Builds a route finder from a complete topology.
     *
     * @param topology Topology containing the stations and links.
     */
    explicit RouteFinder(const Topology &topology);

    /**
     * Builds a route finder from station and link collections.
     *
     * @param nodes Stations to add as graph vertices.
     * @param links Connections to add as weighted graph edges.
     */
    RouteFinder(const QVector<Node> &nodes, const QVector<Link> &links);

    /**
     * Finds the shortest route between two stations by distance.
     *
     * @param fromStationId Identifier of the departure station.
     * @param toStationId Identifier of the destination station.
     * @return The shortest route, or std::nullopt if either station does not
     * exist or no route connects them.
     */
    std::optional<Route> findShortestRoute(int fromStationId, int toStationId) const;

private:
    /**
     * Undirected graph storing a station ID on each vertex and a distance on
     * each edge.
     */
    using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        boost::property<boost::vertex_name_t, int>,
        boost::property<boost::edge_weight_t, double>>;

    /** Descriptor used by Boost to identify a vertex in the graph. */
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    /**
     * Runs Dijkstra's algorithm and returns the shortest vertex path.
     *
     * @param from Departure vertex.
     * @param to Destination vertex.
     * @return Vertices in travel order, or std::nullopt when no path exists.
     */
    std::optional<std::vector<Vertex>> findShortestVertexPath(Vertex from, Vertex to) const;

    /**
     * Reconstructs a path from the predecessor table produced by Dijkstra's
     * algorithm.
     *
     * @param from Departure vertex.
     * @param to Destination vertex.
     * @param predecessors Predecessor vertex associated with each vertex.
     * @return Vertices in travel order, or std::nullopt for an invalid table.
     */
    std::optional<std::vector<Vertex>> reconstructPath(Vertex from, Vertex to, const std::vector<Vertex> &predecessors) const;

    /**
     * Converts a Boost vertex path into the application's route type.
     *
     * @param vertexPath Vertices ordered from departure to destination.
     * @return The corresponding route, or std::nullopt if a link is missing.
     */
    std::optional<Route> createRoute(const std::vector<Vertex> &vertexPath) const;

    /** Weighted graph used by the shortest-path algorithm. */
    Graph graph;

    /** Maps station IDs to their corresponding graph vertices. */
    std::unordered_map<int, Vertex> verticesByStationId;

    /** Stores the station data associated with each station ID. */
    std::unordered_map<int, Node> stationsById;

    /** Maps an unordered pair of station IDs to its shortest direct link. */
    std::unordered_map<std::uint64_t, const Link *> linksByStationPair;
};
