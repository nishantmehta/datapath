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
/** Unit test program to test the loader and file writer communication */

#ifndef NUM_TASKS
#define NUM_TASKS 32
#endif

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Loader.h"
#include "Handler.h"
#include "TwoWayList.cc"
#include "Config.h"

int main(void){

	FILE* fd = fopen(FILENAME, "w+");
	if (fd == NULL) {
		printf("\nFile %s failed to open for writing", FILENAME);
		return 1;
	}
	//put string in separate line in the file
	for (int i = 0; i < CHUNK_SIZE; i++) {
		fprintf(fd, "%s\n", STR);
	}
	fclose(fd);
	
	DistributedCounter dCounter(0); // total number of chunks processed

	// start the processors
	TwoWayList <EventProcessor> myList;
	for (int i=0; i < NUM_TASKS; i++) {
		Handler h;
		h.ForkAndSpin(i);

		Loader l(h, dCounter, i);
		l.ForkAndSpin(i);

		myList.Insert(h);
		myList.Insert(l);
	}
  
	sleep(60);
	printf("Total number of chunks processed = %ld\n", dCounter.Increment(0));
	sleep(1);
	std::string deleteCmd;
	deleteCmd += "rm -f ";
	deleteCmd += FILENAME;
	system(deleteCmd.c_str());
	return 0;
}
