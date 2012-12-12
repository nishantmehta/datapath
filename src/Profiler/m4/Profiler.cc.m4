#include "Profiler.h"
#include "Timer.h"
#include "Logging.h"
#include "Stl.h""
#include <inttypes.h>

dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

using namespace std;

// Instantiate global profiler
Profiler globalProfiler;

ProfilerImp::ProfilerImp(): lastCpu(0), lastTick(0)
#ifdef  DEBUG_EVPROC
  ,EventProcessorImp(true, "Profiler")
#endif
{
    lastCpu = std::clock();
    lastTick = global_clock.GetTime();
    RegisterMessageProcessor(ProfileMessage::type, &ProfileMessage_H, 2);
    RegisterMessageProcessor(ProfileSetMessage::type, &ProfileSetMessage_H, 2);
    RegisterMessageProcessor(ProfileIntervalMessage::type, &ProfileIntervalMessage_H, 2);
}

ProfilerImp :: ~ProfilerImp() {
}

MESSAGE_HANDLER_DEFINITION_BEGIN(ProfilerImp, ProfileMessage_H, ProfileMessage){

    evProc.AddCounter(msg.counter);

}MESSAGE_HANDLER_DEFINITION_END


MESSAGE_HANDLER_DEFINITION_BEGIN(ProfilerImp, ProfileSetMessage_H, ProfileSetMessage){

    FOREACH_TWL(el, msg.counters){
        evProc.AddCounter(el);
    }END_FOREACH

}MESSAGE_HANDLER_DEFINITION_END

MESSAGE_HANDLER_DEFINITION_BEGIN(ProfilerImp, ProfileIntervalMessage_H, ProfileIntervalMessage) {
    evProc.PrintCounters(msg.cTime, msg.wallTime);
} MESSAGE_HANDLER_DEFINITION_END

void ProfilerImp::PrintCounters(const clock_t newCpu, const double newClock){
    std::ostringstream out;

    out << fixed << setprecision(2);
    out << "SC(" << setw(8) << newClock << ")";
    out << "\tCPU:" << setw(5) << (newCpu-lastCpu)/(newClock-lastTick)/CLOCKS_PER_SEC;
    lastCpu = newCpu;
    FOREACH_STL(group, cMap){
        std::ostringstream gOut;
        gOut << endl << "[" << group.first << "]";
        if( group.first.length() < groupColWidth - 2 ) {
            gOut << std::string(groupColWidth - 2 - group.first.length(), ' ');
        }
        bool gotOne = false;
        FOREACH_STL(el, group.second) {
            std::string counter=el.first;
            int64_t& value=el.second;
            int64_t val = value;
            if (val>0){
                gotOne = true;
                val/=(newClock-lastTick);
                gOut <<"\t";
                gOut << counter << ":";
                string outVal;
                HumanizeNumber( val, outVal );
                gOut << outVal;
            }
            value = 0; // reset value
        }END_FOREACH;
        if( gotOne ) {
            out << gOut.str();
        }
    }END_FOREACH;

    lastTick = newClock;
    cout << out.str() << endl;
}
