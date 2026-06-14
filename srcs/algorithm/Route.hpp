#pragma once

#include "../topology/Link.hpp"
#include "../topology/Node.hpp"
#include <QVector>

class Route
{
public:
    /**
     * Describes the timing contract for traversing one route link.
     */
    struct ContractStep
    {
        /** Time spent waiting at the departure node before entering the link. */
        double waitSeconds;

        /** Expected time required to traverse the link. */
        double traversalSeconds;
    };

    /**
     * Creates a route without a timing contract.
     *
     * @param stations Stations in travel order.
     * @param links Links connecting consecutive stations.
     */
    Route(QVector<Node *> stations, QVector<Link *> links);

    /**
     * Creates a route with one timing contract step per link.
     *
     * @param stations Stations in travel order.
     * @param links Links connecting consecutive stations.
     * @param contract Ordered wait and traversal steps.
     */
    Route(QVector<Node *> stations, QVector<Link *> links, QVector<ContractStep> contract);

    /**
     * Checks whether the stations and links form a valid route.
     *
     * @return true when the route topology is valid; otherwise false.
     */
    bool isValid() const;

    /**
     * Checks whether the route has a complete valid timing contract.
     *
     * @return true when there is one valid contract step per link; otherwise
     * false.
     */
    bool hasValidContract() const;

    /**
     * Returns the stations in travel order.
     *
     * @return Route stations.
     */
    const QVector<Node *> &getStations() const;

    /**
     * Returns the links in travel order.
     *
     * @return Route links.
     */
    const QVector<Link *> &getLinks() const;

    /**
     * Returns the route timing contract.
     *
     * @return Ordered contract steps, or an empty collection for an
     * uncontracted route.
     */
    const QVector<ContractStep> &getContract() const;

    /**
     * Returns the total route distance.
     *
     * @return Total distance in kilometers.
     */
    double getTotalDistanceKilometers() const;

private:
    QVector<Node *> stations;
    QVector<Link *> links;
    QVector<ContractStep> contract;
    double totalDistanceKilometers = 0.0;
};
