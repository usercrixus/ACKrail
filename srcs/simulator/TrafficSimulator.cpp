#include "TrafficSimulator.hpp"

#include <chrono>

TrafficSimulator::TrafficSimulator(TrafficManager &trafficManager, TrafficGenerator &trafficGenerator)
    : trafficManager(trafficManager),
      trafficGenerator(trafficGenerator)
{
}

TrafficSimulator::~TrafficSimulator()
{
    stop();
}

void TrafficSimulator::start()
{
    if (!workerThread.joinable())
    {
        workerThread = std::jthread([this](std::stop_token stopToken)
                                    { run(stopToken); });
    }
}

void TrafficSimulator::stop()
{
    if (workerThread.joinable())
    {
        workerThread.request_stop();
        workerThread.join();
    }
}

void TrafficSimulator::run(std::stop_token stopToken)
{
    using Clock = std::chrono::steady_clock;
    constexpr auto TickInterval = std::chrono::milliseconds(16);
    auto previousTick = Clock::now();
    auto nextTick = previousTick + TickInterval;

    while (!stopToken.stop_requested())
    {
        std::this_thread::sleep_until(nextTick);
        const auto currentTick = Clock::now();
        const double elapsedSeconds = std::chrono::duration<double>(currentTick - previousTick).count();
        previousTick = currentTick;
        nextTick = currentTick + TickInterval;
        trafficManager.advance(elapsedSeconds);
        trafficGenerator.tryGenerate(elapsedSeconds);
    }
}
