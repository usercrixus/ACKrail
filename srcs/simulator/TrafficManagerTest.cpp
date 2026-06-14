#include "TrafficManager.hpp"

#include <cassert>
#include <cmath>

int main()
{
    const Node headwayStationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node headwayStationB(2, QStringLiteral("B"), 0.0, 0.00001);
    Topology headwayTopology(
        {headwayStationA, headwayStationB},
        {Link(
            1,
            headwayStationA,
            headwayStationB,
            QStringLiteral("Direct"),
            QStringLiteral("#000000"))});
    Garage headwayGarage(2);
    TrafficManager headwayManager(headwayTopology, headwayGarage);
    Biplace &headwayLeader = headwayGarage.getEngines()[0];
    Biplace &headwayFollower = headwayGarage.getEngines()[1];
    assert(headwayManager.contractRoute(
        headwayLeader,
        headwayStationA.getId(),
        headwayStationB.getId()));
    assert(headwayManager.contractRoute(
        headwayFollower,
        headwayStationA.getId(),
        headwayStationB.getId()));
    const double expectedHeadwaySeconds =
        (headwayFollower.getLengthMeters() + headwayFollower.getSecurityDistanceMeters())
        / 1000.0
        / headwayFollower.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    assert(std::abs(
        headwayFollower.getPad().getTrajectory()->getContract().first().waitSeconds
        - expectedHeadwaySeconds) < 0.000001);
    const double headwayLinkTraversalSeconds =
        headwayTopology.getLinks().first().getDistanceKilometers()
        / headwayFollower.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    headwayFollower.getPad().advance(expectedHeadwaySeconds / 2.0);
    assert(headwayFollower.getPad().getCurrentSpeedKilometersPerHour() == 0.0);
    assert(headwayFollower.getPad().getTravelledDistanceKilometers() == 0.0);
    headwayFollower.getPad().advance(
        expectedHeadwaySeconds / 2.0 + headwayLinkTraversalSeconds / 2.0);
    assert(headwayFollower.getPad().getCurrentSpeedKilometersPerHour()
        == headwayFollower.getPad().getMaximumSpeedKilometersPerHour());
    assert(headwayFollower.getPad().getTravelledDistanceKilometers() > 0.0);

    const Node stationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node stationB(2, QStringLiteral("B"), 0.0, 0.00001);
    const Node stationC(3, QStringLiteral("C"), 0.000005, 0.000005);
    Topology topology(
        {stationA, stationB, stationC},
        {
        Link(1, stationA, stationB, QStringLiteral("Direct"), QStringLiteral("#000000")),
        Link(2, stationA, stationC, QStringLiteral("Alternative"), QStringLiteral("#000000")),
        Link(3, stationC, stationB, QStringLiteral("Alternative"), QStringLiteral("#000000"))
        });

    Garage garage(3);
    TrafficManager trafficManager(topology, garage);
    Biplace &firstEngine = garage.getEngines()[0];
    Biplace &secondEngine = garage.getEngines()[1];
    Biplace &reverseEngine = garage.getEngines()[2];

    assert(firstEngine.getLengthMeters() == 3.0);
    assert(firstEngine.getSecurityDistanceMeters() == 3.0);
    assert(trafficManager.contractRoute(firstEngine, stationA.getId(), stationB.getId()));
    assert(firstEngine.getPad().getTrajectory()->getLinks().first()->getId() == 1);
    assert(firstEngine.getPad().getTrajectory()->getContract().first().waitSeconds == 0.0);

    assert(trafficManager.contractRoute(secondEngine, stationA.getId(), stationB.getId()));
    assert(secondEngine.getPad().getTrajectory()->getLinks().first()->getId() == 2);

    assert(trafficManager.contractRoute(reverseEngine, stationB.getId(), stationA.getId()));
    assert(reverseEngine.getPad().getTrajectory()->getLinks().first()->getId() == 1);
    assert(reverseEngine.getPad().getTrajectory()->getContract().first().waitSeconds == 0.0);

    const double directTraversalSeconds =
        topology.getLinks().first().getDistanceKilometers()
        / firstEngine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    firstEngine.getPad().advance(directTraversalSeconds / 2.0);
    assert(firstEngine.getPad().isActive());
    assert(firstEngine.getPad().getCurrentSpeedKilometersPerHour()
        == firstEngine.getPad().getMaximumSpeedKilometersPerHour());
    assert(firstEngine.getPad().getTravelledDistanceKilometers() > 0.0);

    firstEngine.getPad().advance(directTraversalSeconds);
    assert(!firstEngine.getPad().isActive());
    assert(std::abs(
        firstEngine.getPad().getTravelledDistanceKilometers()
        - topology.getLinks().first().getDistanceKilometers()) < 0.000001);
}
