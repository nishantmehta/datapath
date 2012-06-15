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
#include "Profiler.h"
#include "Timer.h"
#include <unistd.h> 
#include <stdio.h>

Profiler :: Profiler(const char* fileName){
  evGen = new ProfilerImp(fileName);
}

ProfilerImp::ProfilerImp(const char* fileName)
  :out(fileName, fstream::trunc | fstream::out){
  
  printf("Started profile in fine %s\n", fileName);
 }


int ProfilerImp::ProduceMessage(){
  double start_time = global_clock.GetTime();

  // create the counters from the beginning since we want to make sure 
  // we measure the right thing

  PerfCounter cycles(PerfCounter::Cycles, true);
  PerfCounter  instructions(PerfCounter::Instructions, true);
  PerfCounter  branches(PerfCounter::Branch_Instructions, true);
  PerfCounter  branch_misses(PerfCounter::Branch_Misses, true);
  PerfCounter  cache_refs(PerfCounter::Cache_References, true);
  PerfCounter  cache_misses(PerfCounter::Cache_Misses, true);
  PerfCounter  contexts(PerfCounter::Context_Switches, true);
  PerfCounter  task_clock(PerfCounter::Task_Clock, true);
  
  
  sleep(1); // sleep for 1 second

  double c_time = global_clock.GetTime();

  double interval = c_time-start_time; // normalize by this

  out << c_time << "\t" << interval << "\t" << cycles.GetCount() << "\t" 
      << instructions.GetCount() << "\t" << branches.GetCount() << "\t"
      << branch_misses.GetCount() << "\t" << cache_refs.GetCount() << "\t"
      << cache_misses.GetCount() << "\t" << contexts.GetCount() << "\t"
      << task_clock.GetCount() << endl;

  return 0;
}

