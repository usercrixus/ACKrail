#pragma once

#include "../garage/Garage.hpp"
#include "TrafficManager.hpp"

#include <cstddef>
#include <deque>
#include <unordered_map>

class PassengerDispatcher
{
public:
    struct PassengerRequest
    {
        int fromStationId;
        int toStationId;
        double createdAtSeconds;
    };

    PassengerDispatcher(Garage &garage, TrafficManager &trafficManager);

    void enqueue(int fromStationId, int toStationId, double currentSimulationTimeSeconds);
    void dispatchWaitingPassengers(double currentSimulationTimeSeconds);

    std::size_t getQueueSizeAtStation(int stationId) const;
    std::size_t getTotalQueueSize() const;
    double getOldestWaitSecondsAtStation(int stationId, double currentSimulationTimeSeconds) const;

private:
    Garage &garage;
    TrafficManager &trafficManager;
    std::unordered_map<int, std::deque<PassengerRequest>> queuesByStationId;
    std::size_t totalQueueSize = 0;
};
