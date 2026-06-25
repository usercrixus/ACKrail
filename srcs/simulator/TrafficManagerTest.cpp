#include "TrafficManager.hpp"
#include "TrafficBalancer.hpp"
#include "TrafficPassenger.hpp"
#include "../statistics/SimulationStatisticsCollector.hpp"

#include <QTemporaryFile>
#include <cassert>
#include <cmath>

int main()
{
    StationStatistics stationStatistics;
    stationStatistics.setTargetIdleEngines(1, 3.0);
    stationStatistics.recordSnapshot(1, 2.0, 0.0);
    stationStatistics.recordSnapshot(1, 4.0, 10.0);
    stationStatistics.recordDeparture(1);
    stationStatistics.recordArrival(1);
    const StationReport *stationReport =
        stationStatistics.findStationReport(1);
    assert(stationReport != nullptr);
    assert(stationReport->currentIdleEngines == 4.0);
    const StationsGlobalReport globalStationReport =
        stationStatistics.getGlobalReport();
    assert(globalStationReport.stationCount == 1);
    assert(globalStationReport.currentIdleEngines == 4.0);
    assert(globalStationReport.targetIdleEngines == 3.0);
    assert(globalStationReport.departureCount == 1);
    assert(globalStationReport.arrivalCount == 1);

    const Node queueStationA(1, QStringLiteral("Queue A"), 0.0, 0.0);
    const Node queueStationB(2, QStringLiteral("Queue B"), 0.0, 0.00001);
    Topology queueTopology(
        {queueStationA, queueStationB},
        {Link(1,
              queueStationA,
              queueStationB,
              QStringLiteral("Queue"),
              QStringLiteral("#000000"))});
    Garage queueGarage(1);
    TrafficManager queueManager(queueTopology, queueGarage);
    TrafficPassenger queueDispatcher(queueGarage, queueManager);
    Engine &queuedEngine = *queueGarage.getIdleEngine();
    queueGarage.setIdleEngineParkingStation(queuedEngine, queueStationB.getId());
    queueDispatcher.enqueue(queueStationA.getId(), queueStationB.getId(), 2.0);
    queueDispatcher.dispatchWaitingPassengers(5.0);
    assert(queueDispatcher.getQueueSizeAtStation(queueStationA.getId()) == 1);
    assert(queueDispatcher.getTotalQueueSize() == 1);
    assert(queueDispatcher.getOldestWaitSecondsAtStation(queueStationA.getId(), 5.0) == 3.0);
    queueGarage.setIdleEngineParkingStation(queuedEngine, queueStationA.getId());
    queueDispatcher.dispatchWaitingPassengers(6.0);
    assert(queueDispatcher.getQueueSizeAtStation(queueStationA.getId()) == 0);
    assert(queueDispatcher.getTotalQueueSize() == 0);
    assert(queueGarage.getActiveEngineCount() == 1);

    const Node queueBalanceStationA(1, QStringLiteral("Queue balance A"), 0.0, 0.0);
    const Node queueBalanceStationB(2, QStringLiteral("Queue balance B"), 0.0, 0.00001);
    Topology queueBalanceTopology(
        {queueBalanceStationA, queueBalanceStationB},
        {Link(1,
              queueBalanceStationA,
              queueBalanceStationB,
              QStringLiteral("Queue balance"),
              QStringLiteral("#000000"))});
    Garage queueBalanceGarage(4);
    TrafficManager queueBalanceManager(queueBalanceTopology, queueBalanceGarage);
    TrafficPassenger queueBalanceDispatcher(queueBalanceGarage, queueBalanceManager);
    TrafficBalancer queueBalanceBalancer(
        queueBalanceTopology,
        queueBalanceGarage,
        queueBalanceManager,
        queueBalanceDispatcher);
    for (Engine *engine : queueBalanceGarage.getIdleEngines())
        queueBalanceGarage.setIdleEngineParkingStation(
            *engine, queueBalanceStationB.getId());
    queueBalanceDispatcher.enqueue(
        queueBalanceStationA.getId(), queueBalanceStationB.getId(), 0.0);
    queueBalanceDispatcher.enqueue(
        queueBalanceStationA.getId(), queueBalanceStationB.getId(), 0.0);
    queueBalanceBalancer.tryRebalance(5.0, 5.0);
    assert(queueBalanceGarage.getActiveEngineCount() == 3);
    assert(queueBalanceGarage.getIdleEnginesByStation()
               .at(queueBalanceStationB.getId())
               .size()
           == 1);

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
    assert(headwayManager.contractOptimizedRoute(
        headwayLeader,
        headwayStationA.getId(),
        headwayStationB.getId(),
        headwaySimulationTimeSeconds,
        EnginePad::TravelType::Passenger));
    assert(headwayGarage.getActiveEngines().at(headwayLeader.getId()) == &headwayLeader);
    Engine &headwayFollower = *headwayGarage.getIdleEngine();
    assert(headwayManager.contractOptimizedRoute(
        headwayFollower,
        headwayStationA.getId(),
        headwayStationB.getId(),
        headwaySimulationTimeSeconds,
        EnginePad::TravelType::Passenger));
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
    assert(trafficManager.contractOptimizedRoute(firstEngine, stationA.getId(), stationB.getId(), simulationTimeSeconds, EnginePad::TravelType::Passenger));
    assert(firstEngine.getPad().getTrajectory()->getLinks().first()->getId() == 1);
    assert(firstEngine.getPad().getTrajectory()->getContract().first().waitSeconds == 0.0);

    Engine &secondEngine = *garage.getIdleEngine();
    assert(trafficManager.contractOptimizedRoute(secondEngine, stationA.getId(), stationB.getId(), simulationTimeSeconds, EnginePad::TravelType::Passenger));
    assert(secondEngine.getPad().getTrajectory()->getLinks().first()->getId() == 2);

    Engine &reverseEngine = *garage.getIdleEngine();
    assert(trafficManager.contractOptimizedRoute(reverseEngine, stationB.getId(), stationA.getId(), simulationTimeSeconds, EnginePad::TravelType::Passenger));
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
    SimulationStatisticsCollector completionStatisticsCollector(
        completionTopology,
        completionGarage,
        completionManager);
    double completionSimulationTimeSeconds = 0.0;
    Engine &completionEngine = *completionGarage.getIdleEngine();
    completionGarage.setIdleEngineParkingStation(completionEngine, completionStationA.getId());
    assert(completionGarage.getIdleEnginesByStation().at(completionStationA.getId()).size() == 1);
    assert(completionManager.contractOptimizedRoute(completionEngine, completionStationA.getId(), completionStationB.getId(), completionSimulationTimeSeconds, EnginePad::TravelType::Passenger));
    completionStatisticsCollector.update(completionSimulationTimeSeconds);
    const SimulationStatistics &completionStatistics =
        completionStatisticsCollector.getStatistics();
    const StationReport *departureReport =
        completionStatistics.getStationStatistics().findStationReport(completionStationA.getId());
    assert(departureReport != nullptr);
    assert(departureReport->departureCount == 1);
    assert(departureReport->arrivalCount == 0);
    assert(completionTopology.getNodes()[1].getController().getExpectedArrivalCount() == 1);
    assert(completionGarage.getIdleEnginesByStation().find(completionStationA.getId()) == completionGarage.getIdleEnginesByStation().end());
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
    completionStatisticsCollector.update(completionSimulationTimeSeconds);
    completionStatisticsCollector.update(completionSimulationTimeSeconds);
    assert(!completionEngine.getPad().isActive());
    assert(completionTopology.getNodes()[1].getController().getExpectedArrivalCount() == 0);
    assert(completionGarage.getActiveEngineCount() == 0);
    assert(completionGarage.getIdleEnginesByStation().at(completionStationB.getId()).size() == 1);
    assert(completionStatistics.getEngineStatistics().getPassengerTripCount() == 1);
    const StationReport *arrivalReport =
        completionStatistics.getStationStatistics().findStationReport(completionStationB.getId());
    assert(arrivalReport != nullptr);
    assert(arrivalReport->departureCount == 0);
    assert(arrivalReport->arrivalCount == 1);
    assert(completionStatistics.getStationStatistics().getTotalMovementCount() == 2);
    assert(completionStatistics.getEngineStatistics().getTripCount() == 1);
    assert(std::abs(completionStatistics.getEngineStatistics().getPassengerDistanceKilometers()
                    - completionTopology.getLinks().first().getDistanceKilometers()) < 0.000001);

    const Node balanceStationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node balanceStationB(2, QStringLiteral("B"), 0.0, 0.00001);
    Topology balanceTopology(
        {balanceStationA, balanceStationB},
        {Link(1, balanceStationA, balanceStationB, QStringLiteral("Direct"), QStringLiteral("#000000"))});
    QTemporaryFile balanceFile;
    assert(balanceFile.open());
    assert(balanceFile.write(R"({
        "stations": [
            { "stationId": 1, "arrivalWeight": 1.0, "departureWeight": 0.0 },
            { "stationId": 2, "arrivalWeight": 0.0, "departureWeight": 1.0 }
        ]
    })") > 0);
    balanceFile.flush();
    assert(balanceTopology.loadWeights(balanceFile.fileName()));
    Garage balanceGarage(8);
    TrafficManager balanceManager(balanceTopology, balanceGarage);
    TrafficPassenger balancePassenger(balanceGarage, balanceManager);
    TrafficBalancer balanceBalancer(balanceTopology, balanceGarage, balanceManager, balancePassenger);
    for (Engine *engine : balanceGarage.getIdleEngines())
        balanceGarage.setIdleEngineParkingStation(*engine, balanceStationA.getId());
    assert(balanceGarage.getIdleEnginesByStation().at(balanceStationA.getId()).size() == 8);
    assert(std::abs(balanceBalancer.getNetworkBalancePercent() - 12.5) < 0.000001);
    balanceBalancer.tryRebalance(0.0, 30.0);
    assert(balanceGarage.getActiveEngineCount() == 7);
    assert(balanceGarage.getActiveEngines().back()->getPad().getTravelType() == EnginePad::TravelType::Rebalancing);
    assert(balanceGarage.getIdleEnginesByStation().at(balanceStationA.getId()).size() == 1);
    assert(std::abs(balanceBalancer.getNetworkBalancePercent() - 100.0) < 0.000001);

    const Node weightedStationA(1, QStringLiteral("A"), 0.0, 0.0);
    const Node weightedStationB(2, QStringLiteral("B"), 0.0, 0.00001);
    const Node weightedStationC(3, QStringLiteral("C"), 0.0, 0.00002);
    Topology weightedTopology(
        {weightedStationA, weightedStationB, weightedStationC},
        {
            Link(1, weightedStationA, weightedStationB, QStringLiteral("Direct"), QStringLiteral("#000000")),
            Link(2, weightedStationB, weightedStationC, QStringLiteral("Direct"), QStringLiteral("#000000")),
        });
    QTemporaryFile weightedFile;
    assert(weightedFile.open());
    assert(weightedFile.write(R"({
        "stations": [
            { "stationId": 1, "arrivalWeight": 0.0, "departureWeight": 1.0 },
            { "stationId": 2, "arrivalWeight": 1.0, "departureWeight": 0.0 },
            { "stationId": 3, "arrivalWeight": 1.0, "departureWeight": 0.0 }
        ]
    })") > 0);
    weightedFile.flush();
    assert(weightedTopology.loadWeights(weightedFile.fileName()));
    Garage weightedGarage(70);
    TrafficManager weightedManager(weightedTopology, weightedGarage);
    TrafficPassenger weightedPassenger(weightedGarage, weightedManager);
    TrafficBalancer weightedBalancer(weightedTopology, weightedGarage, weightedManager, weightedPassenger);
    int weightedIndex = 0;
    for (Engine *engine : weightedGarage.getIdleEngines())
    {
        const int stationId = weightedIndex < 35 ? weightedStationB.getId() : weightedStationC.getId();
        weightedGarage.setIdleEngineParkingStation(*engine, stationId);
        ++weightedIndex;
    }
    assert(weightedGarage.getIdleEnginesByStation().find(weightedStationA.getId()) == weightedGarage.getIdleEnginesByStation().end());
    weightedBalancer.tryRebalance(0.0, 30.0);
    assert(weightedGarage.getActiveEngineCount() > 0);
    assert(weightedGarage.getActiveEngines().back()->getPad().getTrajectory()->getStations().back()->getId() == weightedStationA.getId());
}
