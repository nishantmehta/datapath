#ifndef _PROFILER_H_
#define _PROFILER_H_

// include the base class definition
#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "Message.h"
#include "ProfMSG.h"
#include <map>
#include <iostream>
#include <iomanip>

class ProfilerImp : public EventProcessorImp {
    typedef int64_t IntType;
    typedef std::map<std::string, IntType> CounterMap;
    typedef std::map<std::string, CounterMap> GroupMap;
    GroupMap cMap; // the map of counters

    const int groupColWidth = 20;

    clock_t lastCpu;
    double lastTick;

    void AddCounter(PCounter& cnt);
    void PrintCounters(const clock_t newCpu, const double newClock);

    void HumanizeNumber( IntType value, std::string& outVal );

    public:

    ProfilerImp();
    ~ProfilerImp();

    MESSAGE_HANDLER_DECLARATION(ProfileMessage_H);
    MESSAGE_HANDLER_DECLARATION(ProfileSetMessage_H);
    MESSAGE_HANDLER_DECLARATION(ProfileIntervalMessage_H);
};


class Profiler : public EventProcessor {

    public:
        Profiler(void){
            evProc = new ProfilerImp;
        }

        virtual ~Profiler(){}

};

// Global variables
extern Profiler globalProfiler;

// Inline methods
inline void ProfilerImp::AddCounter(PCounter& cnt){
    IntType value = cnt.get_value();
    std::string& counter = cnt.get_name();
    std::string& group = cnt.get_group();
    cMap[group][counter] += value;
}

inline
void ProfilerImp :: HumanizeNumber( IntType value, std::string& outVal ) {
    const char * suffList = " KMGTPEZY";
    int index = 0;

    // Shift it down if the value is in danger of using more than 4 digits.
    while( value > 9999 ) {
        value >>= 10;
        ++index;
    }

    std::ostringstream str;
    str << std::setw(4) << value << suffList[index];
    outVal = str.str();
}


#endif // _PROFILER_H_
