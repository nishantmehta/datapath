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



PCProfiler :: PCProfiler(EventProcessor& profiler){
  evGen = new PCProfilerImp(profiler);
}

PCProfilerImp::PCProfilerImp(EventProcessor& profiler) {
    myProfiler.copy( profiler );
}


int PCProfilerImp::ProduceMessage(){
    // create the counters from the beginning since we want to make sure
    // we measure the right thing

    timespec sleepTime = {1, 0};
    timespec remainingTime;

    while( nanosleep( &sleepTime, &remainingTime ) < 0 ) {
        sleepTime = remainingTime;
    }

    double wallTime = global_clock.GetTime();
    clock_t cTime = std::clock();

    ProfileIntervalMessage_Factory(myProfiler, wallTime, cTime );

    return 0;
}

