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
  CounterMap cMap; // map of counters

  clock_t lastCpu;
  double lastTick;

  void AddCounter(PCounter& cnt);
  void PrintCounters(const clock_t newCpu, const double newClock);

  void HumanizeNumber( IntType value, std::string& outVal );

 public:

  ProfilerImp();
  ~ProfilerImp();

  MESSAGE_HANDLER_DECLARATION(ProfileMessage_H);
  MESSAGE_HANDLER_DECLARATION(PCProfileMessage_H);

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
  int64_t value = cnt.get_value();
  std::string& counter = cnt.get_name();
  if (cMap.find(counter) == cMap.end()){ // not found
    cMap.insert(std::make_pair(counter, value));
  } else {
    cMap[counter]+=value;
  }
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
