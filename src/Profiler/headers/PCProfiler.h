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
#ifndef _PC_PROFILER_H_
#define _PC_PROFILER_H_

/** Class that measures performance of the system/second

    Result is logged in a file

*/

#include "EventGenerator.h"
#include "EventProcessor.h"
#include <iostream>
#include <vector>
#include "Logging.h"
#include "PerfCounter.h"

class PCProfilerImp; // forward definition

class PCProfiler : public EventGenerator{

 public:
  PCProfiler(void):EventGenerator(){}

  PCProfiler(EventProcessor& _profiler);
};

/** File produced has the format:

    time time_interval cycles instructions branches branch_misses cache_refs cache_misses contexts task_clock

*/

class PCProfilerImp : public EventGeneratorImp {
  EventProcessor myProfiler; // the profiler we send messages to

  std::vector<PerfCounter> counters;
  bool createdCounters;

public:
  PCProfilerImp(EventProcessor& profiler);

  virtual int ProduceMessage();

  ~PCProfilerImp(){ }
};

#endif // _PC_PROFILER_H_
