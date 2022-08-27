#include <chrono>


class SteadyTimer {
    public:
        std::chrono::time_point<std::chrono::steady_clock> t0;
        SteadyTimer();
        double get_microseconds_since_start();
};

