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
