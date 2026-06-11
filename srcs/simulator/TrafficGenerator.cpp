#include "TrafficGenerator.hpp"

#include <chrono>

TrafficGenerator::TrafficGenerator(const Topology &topology, Garage &garage)
    : topology(topology),
      garage(garage),
      routeFinder(topology),
      randomGenerator(static_cast<std::mt19937::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
{
}

void TrafficGenerator::advance(double elapsedSeconds)
{
    if (elapsedSeconds <= 0.0)
        return;

    for (Biplace &engine : garage.getEngines())
        engine.advance(elapsedSeconds);

    secondsUntilDispatch -= elapsedSeconds;
    while (secondsUntilDispatch <= 0.0)
    {
        dispatchRandomEngines();
        secondsUntilDispatch += 1.0;
    }
}

std::optional<TrafficGenerator::EnginePosition>
TrafficGenerator::getEnginePosition(const Engine &engine) const
{
    const Route *trajectory = engine.getTrajectory();
    if (trajectory == nullptr)
        return std::nullopt;

    const double travelledDistance =
        engine.getAverageSpeedKilometersPerHour()
        * engine.getElapsedTrajectorySeconds() / 3600.0;
    double distanceBeforeLink = 0.0;
    const QVector<const Link *> &links = trajectory->getLinks();
    const QVector<Node> &stations = trajectory->getStations();

    for (qsizetype index = 0; index < links.size(); ++index)
    {
        const Link *link = links[index];
        const double linkDistance = link->getDistanceKilometers();
        if (travelledDistance < distanceBeforeLink + linkDistance)
        {
            return EnginePosition{
                &stations[index],
                &stations[index + 1],
                (travelledDistance - distanceBeforeLink) / linkDistance};
        }
        distanceBeforeLink += linkDistance;
    }

    return std::nullopt;
}

void TrafficGenerator::dispatchRandomEngines()
{
    std::uniform_int_distribution<int> dispatchCountDistribution(5, 20);
    int remainingDispatches = dispatchCountDistribution(randomGenerator);

    while (remainingDispatches > 0)
    {
        Biplace *engine = garage.getIdleEngine();
        if (engine == nullptr)
            break;
        if (dispatchEngine(*engine))
            --remainingDispatches;
        else
            break;
    }
}

bool TrafficGenerator::dispatchEngine(Biplace &engine)
{
    const QVector<Node> &stations = topology.getNodes();
    if (stations.size() < 2)
        return false;

    std::uniform_int_distribution<qsizetype> stationDistribution(0, stations.size() - 1);
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        const Node &fromStation = stations[stationDistribution(randomGenerator)];
        const Node &toStation = stations[stationDistribution(randomGenerator)];
        if (fromStation.getId() == toStation.getId())
            continue;

        const auto route = routeFinder.findShortestRoute(
            fromStation.getId(),
            toStation.getId());
        if (!route.has_value())
            continue;

        engine.setCurrentSpeedKilometersPerHour(
            engine.getMaximumSpeedKilometersPerHour());
        return engine.startTrajectory(*route);
    }

    return false;
}
