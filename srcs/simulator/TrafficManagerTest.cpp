#include "TrafficManager.hpp"

#include <cassert>
#include <cmath>

int main()
{
    NodeController controller(1);
    controller.reserveEntry(1, 7, 0, 2.0, 3.0);
    controller.reserveEntry(2, 7, 1, 4.0, 3.0);
    controller.cleanExpiredReservation(1, 7, 0);
    assert(controller.findEarliestEntryTime(1, 2.0, 1.0) == 2.0);
    assert(controller.findEarliestEntryTime(2, 4.0, 1.0) == 7.0);

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
    double headwaySimulationTimeSeconds = 0.0;
    Engine &headwayLeader = *headwayGarage.getIdleEngine();
    assert(headwayGarage.getIdleEngines().at(headwayLeader.getId()) == &headwayLeader);
    assert(headwayManager.contractRoute(
        headwayLeader,
        headwayStationA.getId(),
        headwayStationB.getId(),
        headwaySimulationTimeSeconds));
    assert(headwayGarage.getActiveEngines().at(headwayLeader.getId()) == &headwayLeader);
    Engine &headwayFollower = *headwayGarage.getIdleEngine();
    assert(headwayManager.contractRoute(
        headwayFollower,
        headwayStationA.getId(),
        headwayStationB.getId(),
        headwaySimulationTimeSeconds));
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
    headwaySimulationTimeSeconds += expectedHeadwaySeconds / 2.0;
    headwayManager.processCurrentEvents(headwaySimulationTimeSeconds);
    assert(headwayFollower.getPad().getCurrentSpeedKilometersPerHour(headwaySimulationTimeSeconds) == 0.0);
    assert(headwayFollower.getPad().getTravelledDistanceKilometers(headwaySimulationTimeSeconds) == 0.0);
    headwaySimulationTimeSeconds += expectedHeadwaySeconds / 2.0 + headwayLinkTraversalSeconds / 2.0;
    headwayManager.processCurrentEvents(headwaySimulationTimeSeconds);
    assert(headwayFollower.getPad().getCurrentSpeedKilometersPerHour(headwaySimulationTimeSeconds)
        == headwayFollower.getPad().getMaximumSpeedKilometersPerHour());
    assert(headwayFollower.getPad().getTravelledDistanceKilometers(headwaySimulationTimeSeconds) > 0.0);

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
    double simulationTimeSeconds = 0.0;
    Engine &firstEngine = *garage.getIdleEngine();

    assert(firstEngine.getLengthMeters() == 3.0);
    assert(firstEngine.getSecurityDistanceMeters() == 3.0);
    assert(trafficManager.contractRoute(firstEngine, stationA.getId(), stationB.getId(), simulationTimeSeconds));
    assert(firstEngine.getPad().getTrajectory()->getLinks().first()->getId() == 1);
    assert(firstEngine.getPad().getTrajectory()->getContract().first().waitSeconds == 0.0);

    Engine &secondEngine = *garage.getIdleEngine();
    assert(trafficManager.contractRoute(secondEngine, stationA.getId(), stationB.getId(), simulationTimeSeconds));
    assert(secondEngine.getPad().getTrajectory()->getLinks().first()->getId() == 2);

    Engine &reverseEngine = *garage.getIdleEngine();
    assert(trafficManager.contractRoute(reverseEngine, stationB.getId(), stationA.getId(), simulationTimeSeconds));
    assert(reverseEngine.getPad().getTrajectory()->getLinks().first()->getId() == 1);
    assert(reverseEngine.getPad().getTrajectory()->getContract().first().waitSeconds == 0.0);

    const double directTraversalSeconds =
        topology.getLinks().first().getDistanceKilometers()
        / firstEngine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    simulationTimeSeconds += directTraversalSeconds / 2.0;
    trafficManager.processCurrentEvents(simulationTimeSeconds);
    assert(firstEngine.getPad().isActive());
    assert(firstEngine.getPad().getCurrentSpeedKilometersPerHour(simulationTimeSeconds)
        == firstEngine.getPad().getMaximumSpeedKilometersPerHour());
    assert(firstEngine.getPad().getTravelledDistanceKilometers(simulationTimeSeconds) > 0.0);

    simulationTimeSeconds += directTraversalSeconds;
    trafficManager.processCurrentEvents(simulationTimeSeconds);
    assert(!firstEngine.getPad().isActive());
    assert(firstEngine.getPad().getCurrentContractStep() == 1);
    assert(std::abs(
        firstEngine.getPad().getTravelledDistanceKilometers()
        - topology.getLinks().first().getDistanceKilometers()) < 0.000001);

    const Node completionStationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node completionStationB(2, QStringLiteral("B"), 0.0, 0.00001);
    Topology completionTopology(
        {completionStationA, completionStationB},
        {Link(1, completionStationA, completionStationB, QStringLiteral("Direct"), QStringLiteral("#000000"))});
    Garage completionGarage(1);
    TrafficManager completionManager(completionTopology, completionGarage);
    double completionSimulationTimeSeconds = 0.0;
    Engine &completionEngine = *completionGarage.getIdleEngine();
    assert(completionManager.contractRoute(completionEngine, completionStationA.getId(), completionStationB.getId(), completionSimulationTimeSeconds));
    const double completionTraversalSeconds =
        completionTopology.getLinks().first().getDistanceKilometers()
        / completionEngine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    completionSimulationTimeSeconds += completionTraversalSeconds / 2.0;
    completionManager.processCurrentEvents(completionSimulationTimeSeconds);
    assert(completionEngine.getPad().isActive());
    assert(completionGarage.getActiveEngineCount() == 1);
    assert(completionEngine.getPad().getCurrentLinkProgress(completionSimulationTimeSeconds) > 0.0);
    completionSimulationTimeSeconds += completionTraversalSeconds + 1.0;
    completionManager.processCurrentEvents(completionSimulationTimeSeconds);
    assert(!completionEngine.getPad().isActive());
    assert(completionGarage.getActiveEngineCount() == 0);
}
