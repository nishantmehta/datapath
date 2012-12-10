/* Headef file containing macros for capturing profiling information

 */

#include "ProfMSG.h"
#include "Timer.h"
#include "Profiler.h"
#include <ctime>
#include "Logging.h"

// macro to increment value of counter(a string) by value(a long int)
#define PROFILING2(counter, value) { \
    double wallTime = global_clock.GetTime(); \
    clock_t cTime = std::clock(); \
    PCounter cnt(counter, value); \
    ProfileMessage_Factory(globalProfiler, wallTime, cTime, cnt); \
}


// macro to flush profiling info to screen if the second is up
#define PROFILING2_FLUSH ;
