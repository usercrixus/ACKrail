#include "RouteFinder.hpp"

#include <cassert>

int main()
{
    const Node stationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node stationB(2, QStringLiteral("B"), 0.0, 1.0);
    const Node stationC(3, QStringLiteral("C"), 0.0, 2.0);
    const Node stationD(4, QStringLiteral("D"), 1.0, 1.0);
    const Node stationE(5, QStringLiteral("E"), 5.0, 5.0);
    const QVector<Node> stations{stationA, stationB, stationC, stationD, stationE};
    const QVector<Link> links{
        Link(1, stationA, stationB, QStringLiteral("1"), QStringLiteral("#000000")),
        Link(2, stationB, stationC, QStringLiteral("1"), QStringLiteral("#000000")),
        Link(3, stationA, stationD, QStringLiteral("2"), QStringLiteral("#000000")),
        Link(4, stationD, stationC, QStringLiteral("2"), QStringLiteral("#000000"))};

    const RouteFinder routeFinder(stations, links);

    const auto route = routeFinder.findShortestRoute(1, 3);
    assert(route.has_value());
    assert(route->stations.size() == 3);
    assert(route->stations[0].getId() == 1);
    assert(route->stations[1].getId() == 2);
    assert(route->stations[2].getId() == 3);
    assert(route->totalDistanceKilometers > 0.0);

    const auto reverseRoute = routeFinder.findShortestRoute(3, 1);
    assert(reverseRoute.has_value());
    assert(reverseRoute->stations[0].getId() == 3);
    assert(reverseRoute->stations[2].getId() == 1);

    const auto sameStationRoute = routeFinder.findShortestRoute(2, 2);
    assert(sameStationRoute.has_value());
    assert(sameStationRoute->stations.size() == 1);
    assert(sameStationRoute->totalDistanceKilometers == 0.0);

    assert(!routeFinder.findShortestRoute(1, 5).has_value());
    assert(!routeFinder.findShortestRoute(1, 999).has_value());
}
