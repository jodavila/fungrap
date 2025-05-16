#pragma once

#include <chrono>
#include <iostream>

class Timer {
    public:
        Timer();
        ~Timer() = default;
        
        void startTimer();
        
        void stopTimer();

        long long getDuration() const;

        long long Timer::getFrameTime() const;

    protected:
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point end;
        
};