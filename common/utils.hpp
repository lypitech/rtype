#pragma once
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

void enableHighPrecisionTimer() { timeBeginPeriod(1); }
#endif

#define SPIN_THRESHOLD std::chrono::milliseconds(2)

namespace utils {

using namespace std::chrono;

inline double getElapsedTime()
{
    static time_point<steady_clock> lastCallTime = steady_clock::now();
    const duration<double> elapsed_seconds = steady_clock::now() - lastCallTime;
    const double seconds = elapsed_seconds.count();
    lastCallTime = steady_clock::now();
    return seconds;
}

class LoopTimer
{
public:
    explicit LoopTimer(const double tps)
        : _interval(duration_cast<steady_clock::duration>(duration<double>(1.0 / tps)))
    {
        _nextTick = steady_clock::now();
    }

    void waitForNextTick()
    {
        _nextTick += _interval;
        const time_point<steady_clock> now = steady_clock::now();
        if (now >= _nextTick) {
            return;
        }
        const auto remaining = duration<double>(_nextTick - now);
        if (remaining > SPIN_THRESHOLD) {
            std::this_thread::sleep_for(remaining - SPIN_THRESHOLD);
        }
        while (steady_clock::now() < _nextTick) {
            std::this_thread::yield();
        }
    }

private:
    steady_clock::duration _interval;
    steady_clock::time_point _nextTick;
};

}  // namespace utils
