////////////////////////////////////////////////////////////////////////////////
// Timer Interface
// Author: Christopher Celio
// Date  : 2011 May 13

// Abstract out the timer code from each micro-benchmark.
#include <sys/time.h>
#include "cctimer.h"
              
cccycles_t inline cc_get_cycles(double clk_freq)
{
   cccycles_t cycles;
   __asm__ __volatile__ ("rdcycle %0" : "=r" (cycles));
   return cycles;
}
        
