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
#ifndef _COORDINATOR_IMP_H_
#define _COORDINATOR_IMP_H_

#include "Message.h"
#include "ID.h"
#include "EventProcessorImp.h"
#include "DataPathGraph.h"
#include "ContainerTypes.h"
#include "Timer.h"
#include "Constants.h"

/* Simple coordintator to wait for queries to finish
*/

class CoordinatorImp : public EventProcessorImp {
private:
	// keep track of the execution engine
	EventProcessor execEngine;

	Timer clock;
	bool clockStarted;

	int qDone; // number of queries that finished

public:
	// constructor
	// metadataFile is the file containing metadata about the relation being read
	// graphFile is the file containing the metadata describing the graph and waypoints
	// waypointFile is a file containing table scan and waypoint detailed info
	CoordinatorImp(	int bogus );

	// destructor doing nothing
	virtual ~CoordinatorImp(void){ }

	// MESSAGE PROCESSING METHODS

	// the query ended
	// processes message QueryFinished
	MESSAGE_HANDLER_DECLARATION(QueryFinishedProc);

	// kill message; need to take everything down
	MESSAGE_HANDLER_DECLARATION(DieProc);

};


#endif // _COORDINATOR_H_
