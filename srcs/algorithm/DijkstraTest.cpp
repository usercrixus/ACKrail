#include "Dijkstra.hpp"

#include <cassert>

int main()
{
    const Node stationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node stationB(2, QStringLiteral("B"), 0.0, 1.0);
    const Node stationC(3, QStringLiteral("C"), 0.0, 2.0);
    const Node stationD(4, QStringLiteral("D"), 1.0, 1.0);
    const Node stationE(5, QStringLiteral("E"), 5.0, 5.0);
    QVector<Node> stations{
        stationA,
        stationB,
        stationC,
        stationD,
        stationE};
    QVector<Link> links{
        Link(1, stationA, stationB, QStringLiteral("1"), QStringLiteral("#000000")),
        Link(2, stationB, stationC, QStringLiteral("1"), QStringLiteral("#000000")),
        Link(3, stationA, stationD, QStringLiteral("2"), QStringLiteral("#000000")),
        Link(4, stationD, stationC, QStringLiteral("2"), QStringLiteral("#000000"))};

    Dijkstra dijkstra(stations, links);

    Route *route = dijkstra.findRoute(1, 3, 0.0, 100.0, 0.0);
    assert(route != nullptr);
    assert(route->hasValidContract());
    assert(route->getStations().size() == 3);
    assert(route->getStations()[0]->getId() == 1);
    assert(route->getStations()[1]->getId() == 2);
    assert(route->getStations()[2]->getId() == 3);
    assert(route->getLinks()[0]->getId() == 1);
    assert(route->getLinks()[1]->getId() == 2);
    assert(route->getContract()[0].waitSeconds == 0.0);

    Route *reverseRoute = dijkstra.findRoute(3, 1, 0.0, 100.0, 0.0);
    assert(reverseRoute != nullptr);
    assert(reverseRoute->getLinks()[0]->getId() == 2);
    assert(reverseRoute->getLinks()[1]->getId() == 1);

    delete route;
    delete reverseRoute;

    assert(dijkstra.findRoute(2, 2, 0.0, 100.0, 0.0) == nullptr);
    assert(dijkstra.findRoute(1, 5, 0.0, 100.0, 0.0) == nullptr);
    assert(dijkstra.findRoute(1, 999, 0.0, 100.0, 0.0) == nullptr);
    assert(dijkstra.findRoute(1, 3, 0.0, 0.0, 0.0) == nullptr);
}
