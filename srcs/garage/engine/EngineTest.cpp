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
    assert(engine.assignRoute({&link, &secondLink}, stationA.getId()));
    engine.setCurrentSpeedKilometersPerHour(36.0);
    engine.advance(10.0);

    assert(engine.getCurrentLink() == &link);
    assert(engine.getFromNode()->getId() == stationA.getId());
    assert(engine.getToNode()->getId() == stationB.getId());
    assert(std::abs(engine.getDistanceOnCurrentLinkKilometers() - 0.1) < 0.000001);

    const double secondsToFirstStation = (link.getDistanceKilometers() - 0.1) / 36.0 * 3600.0;
    engine.advance(secondsToFirstStation + 1.0);

    assert(engine.isActive());
    assert(engine.getCurrentLink() == &secondLink);
    assert(engine.getFromNode()->getId() == stationB.getId());
    assert(engine.getToNode()->getId() == stationC.getId());
    assert(std::abs(engine.getDistanceOnCurrentLinkKilometers() - 0.01) < 0.000001);

    const double secondsToDestination = (secondLink.getDistanceKilometers() - 0.01) / 36.0 * 3600.0;
    engine.advance(secondsToDestination + 0.001);

    assert(!engine.isActive());
    assert(engine.getCurrentLink() == nullptr);
}
