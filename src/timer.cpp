#include "../include/timer.hpp"

void Timer::startTimer() {
    start = std::chrono::steady_clock::now();
}

void Timer::stopTimer() {
    end = std::chrono::steady_clock::now();
}

long long Timer::getDuration() const {
    auto duration = end - start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

long long Timer::getFrameTime() const {
    Timer timer;
    timer.startTimer();
    timer.stopTimer();
    return timer.getDuration();
}