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
    assert(engine.getPad().startContractedTrajectory(route));
    assert(std::abs(
        engine.getPad().getTotalTrajectorySeconds()
        - 3.0
        - firstTraversalSeconds
        - secondTraversalSeconds) < 0.000001);

    engine.getPad().advance(1.0);
    assert(engine.getPad().getCurrentSpeedKilometersPerHour() == 0.0);
    assert(engine.getPad().getTravelledDistanceKilometers() == 0.0);
    assert(engine.getPad().getCurrentContractStep() == 0);
    assert(engine.getPad().getCurrentLinkProgress() == 0.0);

    engine.getPad().advance(2.0);
    assert(engine.getPad().getCurrentSpeedKilometersPerHour()
        == engine.getPad().getMaximumSpeedKilometersPerHour());
    assert(engine.getPad().getTravelledDistanceKilometers() > 0.0);
    assert(engine.getPad().getCurrentContractStep() == 0);
    assert(engine.getPad().getCurrentLinkProgress() > 0.0);
    assert(engine.getPad().getCurrentLinkProgress() < 1.0);
    assert(engine.getPad().getTrajectory() != nullptr);
    assert(engine.getPad().getTrajectory()->getLinks()[0] == &link);

    engine.getPad().advance(firstTraversalSeconds);
    assert(engine.getPad().isActive());
    assert(engine.getPad().getCurrentSpeedKilometersPerHour() == 0.0);

    engine.getPad().advance(1.0 + secondTraversalSeconds);
    assert(!engine.getPad().isActive());
    assert(engine.getPad().getTrajectory() == nullptr);
    assert(std::abs(
        engine.getPad().getTravelledDistanceKilometers()
        - link.getDistanceKilometers()
        - secondLink.getDistanceKilometers()) < 0.000001);
}
