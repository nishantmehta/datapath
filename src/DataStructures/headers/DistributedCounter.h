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
#ifndef _DISTRIBUTED_COUNTER_H
#define _DISTRIBUTED_COUNTER_H

#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include "Errors.h"


/**
	Class to implement a distributed counter that can safely be incremented and decreemnted
	by multiple threads.
*/
class DistributedCounter {
private:
	pthread_mutex_t mutex; // the mutex that protects the counter
	off_t counter; // the counter (off_t so that we can count a lot)

	DistributedCounter(DistributedCounter&); // disable copy constructor
	DistributedCounter& operator= (const DistributedCounter&); // same for assignment

public:
	// constructor. Parameter is initial value
	DistributedCounter(off_t _initial = 0);

	// destructor
	virtual ~DistributedCounter(void);

	// increment and return the count atomicaly
	off_t Increment(off_t _increment);

	// look at the current value
	off_t GetCounter(void);

	// decrement and return the count atomicaly
	off_t Decrement(off_t _decrement);

	// reuse of the mutex to provide mutual exclusion
	void Lock(void);
	void Unlock(void);
};


inline  DistributedCounter::DistributedCounter(off_t _initial):
	counter(_initial){
	pthread_mutex_init(&mutex,NULL);
}

inline DistributedCounter::~DistributedCounter(void){
	pthread_mutex_destroy(&mutex);
}

inline off_t DistributedCounter::Increment(off_t _increment){
	pthread_mutex_lock(&mutex);

	counter+=_increment;
	off_t ret=counter;

	pthread_mutex_unlock(&mutex);

	return ret;
}

inline off_t DistributedCounter::GetCounter(void){
	pthread_mutex_lock(&mutex);
	off_t ret=counter;
	pthread_mutex_unlock(&mutex);

	return ret;
}


inline off_t DistributedCounter::Decrement(off_t _decrement){
	pthread_mutex_lock(&mutex);

	counter-=_decrement;
	off_t ret=counter;

	pthread_mutex_unlock(&mutex);

	FATALIF(ret<0, "A distributed counter got below 0");

	return ret;
}

inline void DistributedCounter::Lock(void){
  pthread_mutex_lock(&mutex);
}

inline void DistributedCounter::Unlock(void){
  pthread_mutex_unlock(&mutex);
}

#endif // _DISTRIBUTED_COUNTER_H
