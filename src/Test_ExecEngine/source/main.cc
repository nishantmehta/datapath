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
#include <unistd.h>

#include "BStringIterator.h" // has to be first since compilation
			 // breaks otherwise. God knows why
#include "ExecEngine.h"
#include "DataPathGraph.h"
#include "WayPointConfigureData.h"
#include "CPUWorkerPool.h"
#include "Coordinator.h"
#include "Constants.h"
#include "Logging.h"
#include "EmptyHashTableWorkFunc"
#include "WriterWorkFunc"
#include "TableScanWorkFunc"
#include "AggWorkFuncs"
#include "SelectionWorkFunc"
#include "PrintWorkFunc"
#include "JoinWorkFuncs"


// all of the event global event processor classes go here... note that since I
// just have a "pretend" disk setup (where chunks are created entirely using CPU)
// I use CPU workers to handle the "disk"... obviously, this will change when the
// "real" disk is added in
ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (16);

// global coordinator. Used by waypoints that produce external
// messages such as Print
/* 0 is a bogus parameter to avoid default constructor */
Coordinator globalCoordinator(0);

#include "History.h"

int main () {

	// this is used to count the number of tuples that get cleaned out of the hash table
	// and sent to a waypoint for writing
	for (int i = 0; i < 4; i++) {
		leftTots[i] = 0;
		rightTots[i] = 0;
	}
	counterMutex = new pthread_mutex_t;
	pthread_mutex_init (counterMutex, NULL);

  	// starting the logging
	StartLogging();

	LOG_ENTRY(1, "Test of execution engine started");

	// allocate the hash table
	HashTable centralHashTable;
	centralHashTable.Allocate (8);

	// fire up the execution engine
	executionEngine.ForkAndSpin ();

	// fire up the global coordinator
	globalCoordinator.ForkAndSpin ();

	// this is the configuration info to send into the execution engine
	DataPathGraph myGraph;
	WayPointConfigurationList myConfigs;

	// run a bunch of nast, hacked code to manually build a graph
	#include "BuildGraph"

	// now, configure the execution engine
	ConfigureExecEngineMessage_Factory (executionEngine, myGraph, myConfigs);

	/*while (1) {
		sleep (2);
		for (int i = 0; i < 4; i++) {
			cout << leftTots[i] << " left tuples cleaned from over-full hash table for query " << i << "\n";
			cout << rightTots[i] << " right tuples cleaned from over-full hash table for query " << i << "\n";
		}
	}*/

	// the thread running the main program is still available
	// we block it until the file writer dies
	globalCoordinator.WaitForProcessorDeath();
	cout << "All queries I loaded are done.\n";

	// kill the execution engine
	KillEvProc (executionEngine);
}
