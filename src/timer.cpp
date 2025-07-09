#include "../include/timer.hpp"


Timer::Timer() {
    start = std::chrono::steady_clock::now();
    end = std::chrono::steady_clock::now();
}

void Timer::startTimer() {
    start = std::chrono::steady_clock::now();
}

void Timer::stopTimer() {
    end = std::chrono::steady_clock::now();
}

int64_t Timer::getDuration() const {
    auto duration = end - start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
