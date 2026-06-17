#include "./Biplace.hpp"

#include <cassert>
#include <cmath>

int main()
{
    Node stationA(1, QStringLiteral("A"), 48.8566, 2.3522);
    Node stationB(2, QStringLiteral("B"), 48.8566, 2.3622);
    Node stationC(3, QStringLiteral("C"), 48.8566, 2.3722);
    Link link(1, stationA, stationB, QStringLiteral("Demo"), QStringLiteral("#ffffff"));
    Link secondLink(2, stationB, stationC, QStringLiteral("Demo"), QStringLiteral("#ffffff"));

    Biplace engine;
    assert(!engine.getPad().hasParkingStation());
    engine.getPad().setParkingStationId(stationA.getId());
    assert(engine.getPad().hasParkingStation());
    assert(engine.getPad().getParkingStationId() == stationA.getId());

    const double expectedEntrySeparationSeconds =
        (engine.getLengthMeters() + engine.getSecurityDistanceMeters())
        / 1000.0
        / engine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    assert(std::abs(
        engine.getEntrySeparationSeconds()
        - expectedEntrySeparationSeconds) < 0.000001);
    const double firstTraversalSeconds =
        link.getDistanceKilometers()
        / engine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    const double secondTraversalSeconds =
        secondLink.getDistanceKilometers()
        / engine.getPad().getMaximumSpeedKilometersPerHour()
        * 3600.0;
    Route *route = new Route(
        {&stationA, &stationB, &stationC},
        {&link, &secondLink},
        {
            {2.0, firstTraversalSeconds},
            {1.0, secondTraversalSeconds},
        });
    assert(route->hasValidContract());
    constexpr double StartTimeSeconds = 10.0;
    const double firstEntryTimeSeconds = StartTimeSeconds + 2.0;
    const double firstExitTimeSeconds = firstEntryTimeSeconds + firstTraversalSeconds;
    const double secondEntryTimeSeconds = firstExitTimeSeconds + 1.0;
    const double secondExitTimeSeconds = secondEntryTimeSeconds + secondTraversalSeconds;
    assert(engine.getPad().startContractedTrajectory(route, StartTimeSeconds));
    assert(engine.getPad().getTravelType() == EnginePad::TravelType::Passenger);
    engine.getPad().setTotalTrajectorySeconds(
        3.0 + firstTraversalSeconds + secondTraversalSeconds);
    assert(std::abs(
        engine.getPad().getTotalTrajectorySeconds()
        - 3.0
        - firstTraversalSeconds
        - secondTraversalSeconds) < 0.000001);

    assert(engine.getPad().getCurrentSpeedKilometersPerHour(StartTimeSeconds + 1.0) == 0.0);
    assert(engine.getPad().getTravelledDistanceKilometers(StartTimeSeconds + 1.0) == 0.0);
    assert(engine.getPad().getCurrentContractStep() == 0);
    assert(engine.getPad().getCurrentLinkProgress(StartTimeSeconds + 1.0) == 0.0);

    assert(engine.getPad().getCurrentSpeedKilometersPerHour(firstEntryTimeSeconds + 1.0)
        == engine.getPad().getMaximumSpeedKilometersPerHour());
    assert(engine.getPad().getTravelledDistanceKilometers(firstEntryTimeSeconds + 1.0) > 0.0);
    assert(engine.getPad().getCurrentContractStep() == 0);
    assert(engine.getPad().getCurrentLinkProgress(firstEntryTimeSeconds + 1.0) > 0.0);
    assert(engine.getPad().getCurrentLinkProgress(firstEntryTimeSeconds + 1.0) < 1.0);
    assert(engine.getPad().getTrajectory() != nullptr);
    assert(engine.getPad().getTrajectory()->getLinks()[0] == &link);

    engine.getPad().completeContractStep(0, firstExitTimeSeconds);
    assert(engine.getPad().isActive());
    assert(engine.getPad().getCurrentSpeedKilometersPerHour(firstExitTimeSeconds) == 0.0);

    engine.getPad().enterContractStep(1, secondEntryTimeSeconds, secondExitTimeSeconds);
    assert(engine.getPad().getCurrentSpeedKilometersPerHour(secondEntryTimeSeconds + 1.0)
        == engine.getPad().getMaximumSpeedKilometersPerHour());
    engine.getPad().finishContractedTrajectory(secondExitTimeSeconds);
    assert(!engine.getPad().isActive());
    assert(engine.getPad().getTravelType() == EnginePad::TravelType::Idle);
    assert(engine.getPad().getTrajectory() == nullptr);
    assert(std::abs(
        engine.getPad().getTravelledDistanceKilometers()
        - link.getDistanceKilometers()
        - secondLink.getDistanceKilometers()) < 0.000001);
    assert(engine.getPad().getParkingStationId() == stationC.getId());

    Route *wrongDepartureRoute = new Route(
        {&stationA, &stationB},
        {&link},
        {{0.0, firstTraversalSeconds}});
    assert(!engine.getPad().startContractedTrajectory(wrongDepartureRoute, secondExitTimeSeconds));
    delete wrongDepartureRoute;
}
