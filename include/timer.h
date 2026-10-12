#ifndef SP_TIMER_H
#define SP_TIMER_H

#include <chrono>

namespace sp {

/// Small wall-clock stopwatch built on <chrono>.
/// Used to measure every stage of the pipeline separately.
class Timer {
public:
    Timer() : start_(Clock::now()) {}

    void reset() { start_ = Clock::now(); }

    /// Elapsed time since construction / last reset, in seconds.
    double seconds() const {
        return std::chrono::duration<double>(Clock::now() - start_).count();
    }

    /// Elapsed time in milliseconds.
    double milliseconds() const { return seconds() * 1000.0; }

private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
};

} // namespace sp

#endif // SP_TIMER_H
