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
#include "datatypes/DSTRING.h"
#include "Timer.h"

#include <iostream>
#include <string.h>
#include <sstream>
#include <stdlib.h>

/** Benchmark for creation of DStrings */

#define NUM_INSERTIONS  (1UL<<24)
#define NUM_STRINGS 10

using namespace std;

int main(void){
	// start with an empty dictionary

	// form an arrays of DStrings (to simulate the real thing)
	DString* dArray = (DString*) malloc( sizeof(DString)*NUM_INSERTIONS );

	Timer clock;
	clock.Restart();
	char buffer[1000];
	for (unsigned long i=0; i<NUM_INSERTIONS; i++){
		sprintf(buffer, "String %ld", random()%NUM_STRINGS);
		dArray[i] = DString(buffer, true);
	}
	
	double insertionTime = clock.GetTime();
	printf("InsertionTotal=%5.4fs\tperTuple=%4.1fns\tTPS=%7.1f\n", 
				 insertionTime, insertionTime/NUM_INSERTIONS*1000000000, 
				 NUM_INSERTIONS/insertionTime);


	DString::SaveDictionary("NewDict.dta");


	return 0;
}

