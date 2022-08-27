#include <chrono>
#include <ctime>
#include <ratio>
#include "steadytimer.hpp"


SteadyTimer::SteadyTimer() {
    t0 = std::chrono::steady_clock::now();
}

double SteadyTimer::get_microseconds_since_start() {
    std::chrono::time_point<std::chrono::steady_clock> t1 = std::chrono::steady_clock::now();
    return (double) std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

