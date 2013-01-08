/* Headef file containing macros for capturing profiling information

 */

#include "SystemCounters.h"

// macro to increment value of counter(a string) by value(a long int)
#define PROFILING2(counter, value)			\
  SystemCounters::GetSystemCounters().Increment(counter, value);


// macro to flush profiling info to screen if the second is up
#define PROFILING2_FLUSH				\
  SystemCounters::GetSystemCounters().PrintIfTick();
