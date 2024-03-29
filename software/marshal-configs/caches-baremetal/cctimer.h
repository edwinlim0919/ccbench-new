////////////////////////////////////////////////////////////////////////////////
// Timer Interface
// Author: Christopher Celio
// Date  : 2011 May 13

// Abstract out the timer code from each micro-benchmark.

#include <stdint.h>
#include <time.h>

// unit is "seconds"
typedef double cctime_t;

// unit is "cycles"
typedef unsigned long cccycles_t;

// converts from seconds to cycles, given the frequency
// if a cycle-accurate timer is available, then the clk_freq
// input is ignored.
cccycles_t cc_get_cycles(double clk_freq);
