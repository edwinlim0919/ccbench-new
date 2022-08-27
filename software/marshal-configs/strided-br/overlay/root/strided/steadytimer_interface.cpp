#include "steadytimer_interface.h"
#include "steadytimer.hpp"

extern "C" {
    CSteadyTimer * create_steadytimer() {
        return reinterpret_cast<CSteadyTimer*>(new SteadyTimer());
    }

    void free_steadytimer(CSteadyTimer * p) {
        delete reinterpret_cast<SteadyTimer*>(p);
    }

    double get_microseconds_steadytimer(CSteadyTimer * p) {
        return reinterpret_cast<SteadyTimer*>(p)->get_microseconds_since_start();
    }
}
