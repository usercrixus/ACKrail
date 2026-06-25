#include "PassengerDispatcher.hpp"

#include <algorithm>

PassengerDispatcher::PassengerDispatcher(Garage &garage, TrafficManager &trafficManager)
    : garage(garage),
      trafficManager(trafficManager)
{
}

void PassengerDispatcher::enqueue(int fromStationId, int toStationId, double currentSimulationTimeSeconds)
{
    if (fromStationId == toStationId)
        return;
    queuesByStationId[fromStationId].push_back({fromStationId, toStationId, currentSimulationTimeSeconds});
    ++totalQueueSize;
}

void PassengerDispatcher::dispatchWaitingPassengers(double currentSimulationTimeSeconds)
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
            if (!trafficManager.contractOptimizedRoute(*engine, request.fromStationId, request.toStationId, currentSimulationTimeSeconds, EnginePad::TravelType::Passenger))
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

std::size_t PassengerDispatcher::getQueueSizeAtStation(int stationId) const
{
    const auto queue = queuesByStationId.find(stationId);
    return queue == queuesByStationId.end() ? 0 : queue->second.size();
}

std::size_t PassengerDispatcher::getTotalQueueSize() const
{
    return totalQueueSize;
}

double PassengerDispatcher::getOldestWaitSecondsAtStation(int stationId, double currentSimulationTimeSeconds) const
{
    const auto queue = queuesByStationId.find(stationId);
    if (queue == queuesByStationId.end() || queue->second.empty())
        return 0.0;
    return std::max(0.0, currentSimulationTimeSeconds - queue->second.front().createdAtSeconds);
}
