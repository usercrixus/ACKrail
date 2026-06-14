#pragma once

#include <unordered_map>
#include <vector>

class Engine;

/**
 * Maintains the departure schedule for links controlled by one node.
 */
class NodeController
{
public:
    explicit NodeController(int nodeId);

    int getNodeId() const;

    /**
     * Finds the first time at which a directional link can accept a departure.
     */
    double findEarliestEntryTime(int linkId, double requestedEntryTime, double entrySeparationSeconds) const;

    /**
     * Commits a departure slot previously selected by the route planner.
     */
    void reserveEntry(int linkId, const Engine &engine, double entryTime, double entrySeparationSeconds);

private:
    struct Reservation
    {
        double startSeconds;
        double endSeconds;
        const Engine *engine;
    };

    int nodeId;
    std::unordered_map<int, std::vector<Reservation>> reservationsByLinkId;
};
