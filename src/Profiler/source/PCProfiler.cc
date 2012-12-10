//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <ctime>
#include "PCProfiler.h"
#include "PerfCounter.h"
#include "Profiler.h"
#include "Timer.h"
#include <unistd.h>
#include <stdio.h>
#include "ProfMSG.h"


/** If the list of performance counters to watch changes, mofify this

 Make sure the list size is set correctly as well
*/

static const PerfCounter::EventType eventsPC[] = {
  PerfCounter::Cycles,
  PerfCounter::Instructions,
  PerfCounter::Branch_Instructions,
  PerfCounter::Branch_Misses,
  PerfCounter::Cache_References,
  PerfCounter::Cache_Misses,
  PerfCounter::Context_Switches,
  PerfCounter::Task_Clock
};

static const size_t eventsPC_size = 8;

PCProfiler :: PCProfiler(EventProcessor& profiler){
  evGen = new PCProfilerImp(profiler);
}

PCProfilerImp::PCProfilerImp(EventProcessor& profiler) : createdCounters(false), counters(eventsPC_size) {
    myProfiler.copy( profiler );
}


int PCProfilerImp::ProduceMessage(){
    // create the counters from the beginning since we want to make sure
    // we measure the right thing

    if( !createdCounters ) {
        for (size_t i=0;  i<eventsPC_size; i++){
            PerfCounter cnt(eventsPC[i], true);
            counters[i].Swap(cnt);
        }
        createdCounters = true;
    }

    timespec sleepTime = {1, 0};
    timespec remainingTime;

    while( nanosleep( &sleepTime, &remainingTime ) < 0 ) {
        sleepTime = remainingTime;
    }

    double wallTime = global_clock.GetTime();
    clock_t cTime = std::clock();

    PCounterList counterList;

    for (size_t i=0;  i<eventsPC_size; i++){
        PCounter cnt(PerfCounter::names[eventsPC[i]],
                counters[i].GetCount());
        counterList.Append(cnt);
    }


    PCProfileMessage_Factory(myProfiler, wallTime, cTime, counterList);

    return 0;
}

