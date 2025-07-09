#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

class Timer {
    public:
        Timer();
        ~Timer() = default;
        
        void startTimer();
        
        void stopTimer();

        int64_t getDuration() const;

    protected:
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point end;
        
};

#endif // TIMER_HPP