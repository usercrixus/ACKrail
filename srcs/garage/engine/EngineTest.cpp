#include "./EngineFactory.hpp"

#include <cassert>
#include <cmath>

int main()
{
    const Node stationA(1, QStringLiteral("A"), 48.8566, 2.3522);
    const Node stationB(2, QStringLiteral("B"), 48.8566, 2.3622);
    const Node stationC(3, QStringLiteral("C"), 48.8566, 2.3722);
    const Link link(1, stationA, stationB, QStringLiteral("Demo"), QStringLiteral("#ffffff"));
    const Link secondLink(2, stationB, stationC, QStringLiteral("Demo"), QStringLiteral("#ffffff"));

    Biplace engine = EngineFactory::createBiplace();
    const Route route(
        {stationA, stationB, stationC},
        {&link, &secondLink});
    assert(route.isValid());
    assert(engine.startTrajectory(route));
    engine.setCurrentSpeedKilometersPerHour(36.0);
    engine.advance(10.0);

    assert(engine.getElapsedTrajectorySeconds() == 10.0);
    assert(engine.getAverageSpeedKilometersPerHour() == 36.0);
    assert(engine.getTrajectory() != nullptr);
    assert(engine.getTrajectory()->getLinks()[0] == &link);

    const double secondsToFirstStation = (link.getDistanceKilometers() - 0.1) / 36.0 * 3600.0;
    engine.advance(secondsToFirstStation + 1.0);

    assert(engine.isActive());

    const double secondsToDestination = (secondLink.getDistanceKilometers() - 0.01) / 36.0 * 3600.0;
    engine.advance(secondsToDestination + 0.001);

    assert(!engine.isActive());
    assert(engine.getTrajectory() == nullptr);
    assert(std::abs(
        engine.getAverageSpeedKilometersPerHour()
            * engine.getElapsedTrajectorySeconds() / 3600.0
        - route.getTotalDistanceKilometers()) < 0.000001);

    const Route returnRoute(
        {stationB, stationC},
        {&secondLink});
    assert(engine.startTrajectory(returnRoute));
    assert(engine.getElapsedTrajectorySeconds() == 0.0);
    assert(engine.getAverageSpeedKilometersPerHour() == 0.0);
    assert(engine.getTrajectory() != nullptr);

    engine.setCurrentSpeedKilometersPerHour(36.0);
    engine.advance(10.0);
    engine.setCurrentSpeedKilometersPerHour(18.0);
    engine.advance(20.0);

    assert(engine.getElapsedTrajectorySeconds() == 30.0);
    assert(std::abs(engine.getAverageSpeedKilometersPerHour() - 24.0) < 0.000001);
    assert(std::abs(
        engine.getAverageSpeedKilometersPerHour()
            * engine.getElapsedTrajectorySeconds() / 3600.0
        - 0.2) < 0.000001);

    const double remainingSecondsAtReducedSpeed =
        (returnRoute.getTotalDistanceKilometers() - 0.2) / 18.0 * 3600.0;
    engine.advance(remainingSecondsAtReducedSpeed - 0.001);
    assert(engine.isActive());

    engine.advance(0.002);
    assert(!engine.isActive());
}
