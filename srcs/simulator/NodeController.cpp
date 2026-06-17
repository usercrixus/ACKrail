#include "NodeController.hpp"

#include <algorithm>

NodeController::NodeController(int nodeId)
    : nodeId(nodeId)
{
}

int NodeController::getNodeId() const
{
    return nodeId;
}

double NodeController::findEarliestEntryTime(int linkId, double requestedEntryTime, double entrySeparationSeconds) const
{
    double candidate = requestedEntryTime;
    const auto schedulePosition = reservationsByLinkId.find(linkId);
    if (schedulePosition == reservationsByLinkId.end())
        return candidate;
    const std::vector<Reservation> &reservations = schedulePosition->second;
    for (const Reservation &reservation : reservations)
    {
        if (candidate + entrySeparationSeconds <= reservation.startSeconds)
            break;
        if (candidate < reservation.endSeconds)
            candidate = reservation.endSeconds;
    }
    return candidate;
}

void NodeController::reserveEntry(int linkId, int engineId, std::size_t contractStep, double entryTime, double entrySeparationSeconds)
{
    std::vector<Reservation> &reservations = reservationsByLinkId[linkId];
    const Reservation reservation{entryTime, entryTime + entrySeparationSeconds, engineId, contractStep};
    const auto insertionPoint = std::lower_bound(reservations.begin(), reservations.end(), reservation.startSeconds, [](const Reservation &existing, double startSeconds)
                                                 { return existing.startSeconds < startSeconds; });
    reservations.insert(insertionPoint, reservation);
}

void NodeController::cleanExpiredReservation(int linkId, int engineId, std::size_t contractStep)
{
    const auto schedulePosition = reservationsByLinkId.find(linkId);
    if (schedulePosition != reservationsByLinkId.end())
    {
        std::vector<Reservation> &reservations = schedulePosition->second;
        const auto reservation = std::find_if(reservations.begin(), reservations.end(), [engineId, contractStep](const Reservation &candidate)
                                              { return candidate.engineId == engineId && candidate.contractStep == contractStep; });
        if (reservation != reservations.end())
        {
            reservations.erase(reservation);
            if (reservations.empty())
                reservationsByLinkId.erase(schedulePosition);
        }
    }
}
