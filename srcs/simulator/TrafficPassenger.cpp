#include "TrafficPassenger.hpp"
#include "TrafficManager.hpp"

#include <algorithm>

TrafficPassenger::TrafficPassenger(Garage &garage, TrafficOperations &trafficOperations)
    : garage(garage),
      trafficOperations(trafficOperations)
{
}

TrafficPassenger::TrafficPassenger(Garage &garage, TrafficManager &trafficManager)
    : TrafficPassenger(garage, trafficManager.getTrafficOperations())
{
}

void TrafficPassenger::enqueue(int fromStationId, int toStationId, double currentSimulationTimeSeconds)
{
    if (fromStationId == toStationId)
        return;
    queuesByStationId[fromStationId].push_back({fromStationId, toStationId, currentSimulationTimeSeconds});
    ++totalQueueSize;
}

void TrafficPassenger::dispatchWaitingPassengers(double currentSimulationTimeSeconds)
{
    for (auto queuePosition = queuesByStationId.begin(); queuePosition != queuesByStationId.end();)
    {
        std::deque<PassengerRequest> &queue = queuePosition->second;
        const int stationId = queuePosition->first;

        while (!queue.empty())
        {
            const auto stationPool = garage.getIdleEnginesByStation().find(stationId);
            if (stationPool == garage.getIdleEnginesByStation().end() || stationPool->second.empty())
                break;
            Engine *engine = stationPool->second[0];
            const PassengerRequest &request = queue.front();
            if (!trafficOperations.contractOptimizedRoute(*engine, request.fromStationId, request.toStationId, currentSimulationTimeSeconds, EnginePad::TravelType::Passenger))
                break;
            queue.pop_front();
            --totalQueueSize;
        }
        if (queue.empty())
            queuePosition = queuesByStationId.erase(queuePosition);
        else
            ++queuePosition;
    }
}

std::size_t TrafficPassenger::getQueueSizeAtStation(int stationId) const
{
    const auto queue = queuesByStationId.find(stationId);
    return queue == queuesByStationId.end() ? 0 : queue->second.size();
}

std::size_t TrafficPassenger::getTotalQueueSize() const
{
    return totalQueueSize;
}

double TrafficPassenger::getOldestWaitSecondsAtStation(int stationId, double currentSimulationTimeSeconds) const
{
    const auto queue = queuesByStationId.find(stationId);
    if (queue == queuesByStationId.end() || queue->second.empty())
        return 0.0;
    return std::max(0.0, currentSimulationTimeSeconds - queue->second.front().createdAtSeconds);
}
