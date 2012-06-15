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
#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "DistributedCounter.h"
#include "Errors.h"
#include "Swap.h"
#include "Message.h"


EventProcessor::EventProcessor(void) {
	// we just make the pointer null to make sure
	// none of the functions have any effect
	evProc = NULL;
	numCopies = new DistributedCounter(1);
	noDelete = false;
}

EventProcessor::EventProcessor(EventProcessorImp* obj) {
	evProc = obj;
	numCopies = new DistributedCounter(1);
	noDelete=true;
}

void EventProcessor::ProcessMessage(Message& msg) {
	if (evProc != NULL) {
		evProc->ProcessMessage(msg);
	}
	else {
		FATAL("Message sent to an unititialized EventProcessor.");
		assert(1==2);
	}
}

bool EventProcessor::ForkAndSpin(int node) {
	if (evProc != NULL) {
		return evProc->ForkAndSpin(node);
	}

	return false;
}

void EventProcessor::Seppuku(void) {
	if (evProc != NULL) {
		evProc->Seppuku();
	}
}

void EventProcessor::WaitForProcessorDeath(void) {
	if (evProc != NULL) {
		evProc->WaitForProcessorDeath();
	}
}

EventProcessor::~EventProcessor() {
	// is this the last copy?
	if (numCopies->Decrement(1) == 0) {
		// distroy the event processor and the counter
		if (evProc != NULL && !noDelete)
			delete evProc;

		delete numCopies;
	}
}

void EventProcessor::swap(EventProcessor& other) {
	SWAP(evProc, other.evProc);
	SWAP(numCopies, other.numCopies);
	SWAP(noDelete, other.noDelete);
}

void EventProcessor::copy(EventProcessor& other) {
	// same code as the destructor
	if (numCopies->Decrement(1) == 0) {
		// distroy the event processor and the counter
		if (evProc != NULL && !noDelete)
			delete evProc;

		delete numCopies;
	}

	// put in the new content
	evProc = other.evProc;
	numCopies = other.numCopies;
	noDelete = other.noDelete;
	numCopies->Increment(1);
}
