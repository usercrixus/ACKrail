#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

/**
 * Maintains the departure schedule for links controlled by one node.
 */
class NodeController
{
public:
    NodeController(int nodeId);

    int getNodeId() const;

    /**
     * Finds the first time at which a directional link can accept a departure.
     */
    double findEarliestEntryTime(int linkId, double requestedEntryTime, double entrySeparationSeconds) const;

    /**
     * Commits a departure slot previously selected by the route planner.
     */
    void reserveEntry(
        int linkId,
        int engineId,
        std::size_t contractStep,
        double entryTime,
        double entrySeparationSeconds);

    /**
     * Removes completed departure slots belonging to an engine.
     */
    void cleanExpiredReservation(int linkId, int engineId, std::size_t contractStep);

    void addExpectedArrival();
    void removeExpectedArrival();
    std::size_t getExpectedArrivalCount() const;

private:
    struct Reservation
    {
        double startSeconds;
        double endSeconds;
        int engineId;
        std::size_t contractStep;
    };
    int nodeId;
    std::size_t expectedArrivalCount = 0;
    std::unordered_map<int, std::vector<Reservation>> reservationsByLinkId;
};
