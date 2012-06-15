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
/** Unit test program to test the NUMA memory allocation capablities */

#ifndef NUM_TASKS
#define NUM_TASKS 32
#endif

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "Generator.h"
#include "Adder.h"
#include "TwoWayList.cc"
#include "MmapAllocator.h"

int main(void){
	mmap_free(mmap_alloc(1000));

	DistributedCounter dCounter(0); // total number of blocks processed

	// start the processors
	TwoWayList <EventProcessor> myGens;
	for (int i=0; i < NUM_TASKS; i++) {
		Adder adder;
		adder.ForkAndSpin(i);

		Generator newGen(adder, dCounter, i);
		newGen.ForkAndSpin(i);

		myGens.Insert(adder);
		myGens.Insert(newGen);
	}
  
	sleep(60);
	printf("Total count = %ld\n", dCounter.Increment(0));
	sleep(1);
	return 0;
}
