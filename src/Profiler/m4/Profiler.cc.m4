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
  RegisterMessageProcessor(PCProfileMessage::type, &PCProfileMessage_H, 2);
}

ProfilerImp :: ~ProfilerImp() {
}

MESSAGE_HANDLER_DEFINITION_BEGIN(ProfilerImp, ProfileMessage_H, ProfileMessage){
  evProc.AddCounter(msg.counter);
}MESSAGE_HANDLER_DEFINITION_END


MESSAGE_HANDLER_DEFINITION_BEGIN(ProfilerImp, PCProfileMessage_H, PCProfileMessage){

  FOREACH_TWL(el, msg.counters){
    evProc.AddCounter(el);
  }END_FOREACH

  evProc.PrintCounters(msg.cTime, msg.wallTime);

}MESSAGE_HANDLER_DEFINITION_END

void ProfilerImp::PrintCounters(const clock_t newCpu, const double newClock){
    std::ostringstream out;

    out << fixed << setprecision(2);
    out << "SC(" << setw(8) << newClock << ")";
    out << "\tCPU:" << setw(5) << (newCpu-lastCpu)/(newClock-lastTick)/CLOCKS_PER_SEC;
    lastCpu = newCpu;
    FOREACH_STL(el, cMap){
        std::string counter=el.first;
        int64_t& value=el.second;
        int64_t val = value;
        if (val>0){
            val/=(newClock-lastTick);
            out <<"\t";
            out << counter << ":";
            string outVal;
            HumanizeNumber( val, outVal );
            out << outVal;
        }
        value = 0; // reset value
    }END_FOREACH;

    lastTick = newClock;
    cout << out.str() << endl;
}
