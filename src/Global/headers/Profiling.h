/* Headef file containing macros for capturing profiling information

 */

#include <ctime>
#include "ProfMSG.h"
#include "Timer.h"
#include "Profiler.h"
#include "Logging.h"
#include "Pragma.h"

/*
 * General structure of code being profiled:
 *
 * <set up counters>
 * PROFILING2_START
 *
 *  <Important code to run>
 *
 * PROFILING2_END
 * <finalize counters>
 *
 * PROFILING2_SINGLE or PROFILING2_SET
 * depending on the number of counters
 */

// Saves the relevant start times for a profiling run
#define PROFILING2_START \
    double _dp_prof_wall_start_ = global_clock.GetTime(); \
    clock_t _dp_prof_cpu_start_ = std::clock();

// Saves the relevant end times for a profiling run
#define PROFILING2_END \
    double _dp_prof_wall_end_ = global_clock.GetTime(); \
    clock_t _dp_prof_cpu_end_ = std::clock();

// macro to increment value of counter(a string) by value(a long int)
#define PROFILING2_SINGLE(counter, value, group) { \
    PCounter cnt((counter), (value), (group)); \
    ProfileMessage_Factory(globalProfiler, _dp_prof_wall_start_, _dp_prof_cpu_start_, \
            _dp_prof_wall_end_, _dp_prof_cpu_end_, cnt); \
}

// macro to send a set of counters
#define PROFILING2_SET(counterSet) { \
    ProfileSetMessage_Factory(globalProfiler, _dp_prof_wall_start_, _dp_prof_cpu_start_, \
            _dp_prof_wall_end_, _dp_prof_cpu_end_, (counterSet)); \
}

// A counter that pertains to an instant in time instead of an interval
#define PROFILING2_INSTANT(counter, value, group) { \
    double wallClock = global_clock.GetTime();  \
    clock_t cpuTime = std::clock(); \
    PCounter cnt((counter), (value), (group)); \
    ProfileMessage_Factory(globalProfiler, wallClock, cpuTime, wallClock, cpuTime, cnt); \
}

#define PROFILING2(counter, value) \
    PRAGMA_MSG("The PROFILING2 macro has been deprecated.")

// macro to flush profiling info to screen if the second is up
#define PROFILING2_FLUSH \
    PRAGMA_MSG("The PROFILING2_FLUSH macro has been deprecated.")
