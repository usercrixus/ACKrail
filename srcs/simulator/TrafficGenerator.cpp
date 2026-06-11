#include "TrafficGenerator.hpp"

#include <algorithm>
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

        const QVector<const Link *> routeLinks = linksForRoute(*route);
        if (routeLinks.size() != route->stations.size() - 1)
            continue;

        engine.setCurrentSpeedKilometersPerHour(
            engine.getMaximumSpeedKilometersPerHour());
        return engine.assignRoute(routeLinks, fromStation.getId());
    }

    return false;
}

QVector<const Link *> TrafficGenerator::linksForRoute(const RouteFinder::Route &route) const
{
    QVector<const Link *> routeLinks;
    routeLinks.reserve(std::max<qsizetype>(0, route.stations.size() - 1));

    for (qsizetype index = 1; index < route.stations.size(); ++index)
    {
        const int fromStationId = route.stations[index - 1].getId();
        const int toStationId = route.stations[index].getId();
        const auto link = std::find_if(
            topology.getLinks().begin(),
            topology.getLinks().end(),
            [fromStationId, toStationId](const Link &candidate) {
                const int candidateFromId = candidate.getFromNode().getId();
                const int candidateToId = candidate.getToNode().getId();
                return (candidateFromId == fromStationId && candidateToId == toStationId)
                    || (candidateFromId == toStationId && candidateToId == fromStationId);
            });
        if (link == topology.getLinks().end())
            return {};
        routeLinks.append(&*link);
    }

    return routeLinks;
}
