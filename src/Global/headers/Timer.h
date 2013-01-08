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
#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>
#include <time.h>

/** This class implements a timer that, on Linux, has high
	* precision. The timer is based on gettimeofday().
	*/

class Timer {
private:
	double startTime; // the start time in seconds (high precision)

	// current time as a double
	double ctimeDBL(void){
		struct timeval time;
		gettimeofday(&time, NULL);
		return time.tv_sec+1.0e-6*time.tv_usec;
	}

public:
	Timer(void){Restart();}

	// call this to start the timer
	void Restart(void){ startTime=ctimeDBL(); }

	// call this to get the current time
	double GetTime(void){ return ctimeDBL()-startTime; }
};

#endif // _TIMER_H_
