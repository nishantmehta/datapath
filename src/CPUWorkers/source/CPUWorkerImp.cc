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

#include "CPUWorkerImp.h"
#include "CPUWorkerPool.h"
#include "EEExternMessages.h"
#include "ExecEngine.h"
#include "ExecEngineImp.h"
#include "Logging.h"
#include "Diagnose.h"

/** How oftern the system should have context swithes? Need this to determine if we have
    too many context switches thus we should be worried */
#ifndef HZ // linux defines it in asm/param.h
#define HZ 100
#endif

///////////////////// NO SYSTEM HEADERS SHOULD BE INCLUDED BEYOND THIS POINT ////////////////////

void CPUWorkerImp :: GetCopyOf (EventProcessor &myParent){
	me.copy (myParent);
}

CPUWorkerImp :: CPUWorkerImp ()
#ifdef PER_CPU_PROFILE
 :
  cycles(PerfCounter::Cycles),
  instructions(PerfCounter::Instructions),
  branches(PerfCounter::Branch_Instructions),
  branch_misses(PerfCounter::Branch_Misses),
  cache_refs(PerfCounter::Cache_References),
  cache_misses(PerfCounter::Cache_Misses),
  contexts(PerfCounter::Context_Switches)
#endif
{
  
	// register the DoSomeWork method
	RegisterMessageProcessor (WorkRequestMsg :: type, &DoSomeWork, 1);
}

CPUWorkerImp :: ~CPUWorkerImp () {}

MESSAGE_HANDLER_DEFINITION_BEGIN(CPUWorkerImp, DoSomeWork, WorkRequestMsg) {

	// this is where the result of the computation will go
	ExecEngineData computationResult;

	LOG_ENTRY_P(1, " Function of waypoint %s started\n", msg.currentPos.getName().c_str());
	DIAG_ID dID = DIAGNOSE_ENTRY("CPUWORKER", msg.currentPos.getName().c_str(), "CPUWORK");
	/* Reset performance counters */
	evProc.ResetAllCounters();
	uint64_t effort = PREFERED_TUPLES_PER_CHUNK; // function fills in the effort
	// now, call the work function to actually produce the output data
	int returnVal = msg.myFunc (msg.workDescription, computationResult);
	/* Stop the counters */
	evProc.ReadAllCounters();
	DIAGNOSE_EXIT(dID);
	
	/** Statistics we compute */
	// instructions per cycle. Below 1.0 means big problems with stalls */
	float instPerCycle = 1.0*evProc.instructions_C/evProc.cycles_C;
	/** percentage branch misses */
	float perBranchMiss = 100.0*evProc.branch_misses_C/evProc.branches_C;
	/** percentage cache misses */
	float perCacheMiss = 100.0*evProc.cache_misses_C/evProc.cache_refs_C;
	/** number of cycles/tuple */
	float cyclesTuple = 1.0*evProc.cycles_C/effort;
	/** number of cache misses/tuple */
	float bMissesTuple = 1.0*evProc.branch_misses_C/effort;
	/** number of context switches per system clock tick. A lot more then 1.0
	    indicates problems with locking (which would cause a lot of switches) */
	float cxPerSlice = 1.0*evProc.contexts_C/evProc.clock_C/HZ;

	LOG_ENTRY_P(1, " Function of waypoint %s fihished."
		    " Tm: %4.5f I/Cy:%1.1f BMs:%2.3f%% CMs:%2.3f%% Cy/T:%5.1f Bms/T:%3.2f CX:%2.1f\n",
		    msg.currentPos.getName().c_str(), 
		    evProc.clock_C, instPerCycle, perBranchMiss, perCacheMiss, cyclesTuple, 
		    bMissesTuple, cxPerSlice);

	// and finally, store outselves in the queue for future use
	CPUWorker me;
	me.copy(evProc.me);
	if (CHECK_DATA_TYPE(msg.token, CPUWorkToken)) {
		myCPUWorkers.AddWorker (me);	
	} else if (CHECK_DATA_TYPE(msg.token, DiskWorkToken)) {
		myDiskWorkers.AddWorker (me);	
	} else
		FATAL ("Strange work token type!\n");
	
	// now, send the result back
	// first, create the object that will have the result
	HoppingDataMsg result (msg.currentPos, msg.dest, msg.lineage, computationResult);

	//printf("\nData msg from MESSAGE_HANDLER_DEFINITION_BEGIN(CPUWorkerImp, DoSomeWork, WorkRequestMsg) sent");
	// and send it
	HoppingDataMsgMessage_Factory (executionEngine, returnVal, msg.token, result);

	
}MESSAGE_HANDLER_DEFINITION_END
