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
/** Unit test program to test the EventProcessor and Messagign capablities */

// how many generators are started
#define NUM_GENERATORS 20

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "Generator.h"
#include "Adder.h"
#include "DistributedCounter.h"
#include "TwoWayList.cc"

int main(void){
  // create and start the adder
  Adder adder;
  adder.ForkAndSpin(); // starts one independent thread for the adder

  // the counter to detect when all the generators finished
  DistributedCounter dCounter(NUM_GENERATORS);

  // start the generators
  TwoWayList <EventProcessor> myGens;
  for (int i=0; i<NUM_GENERATORS; i++) {
    Generator newGen(adder, dCounter);
    newGen.ForkAndSpin();
    myGens.Insert(newGen);
  }
  
  // the thread running the main program is still available
  // we block it until the adder dies
  adder.WaitForProcessorDeath();  

  printf("The correct result is: %d\n", REPETITIONS*(REPETITIONS-1)/2*NUM_GENERATORS);
  
  // adder and all the generator automatically destroyed

  return 0;
}
