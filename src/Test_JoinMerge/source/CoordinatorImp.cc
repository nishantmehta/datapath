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
#include "CoordinatorImp.h"
#include "ExecEngine.h"
#include "EEExternMessages.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <libgen.h>
#include <string>

CoordinatorImp::CoordinatorImp(int bogus)
#ifdef DEBUG_EVPROC
	: EventProcessorImp(true, "Coordinator") // comment to remove debug
#endif

{
	clockStarted=false;
	qDone = 0; 

	execEngine.copy(executionEngine /* global var; how ugly is that*/);

	// we register the messges since we need to receive some
	RegisterMessageProcessor(DieMessage::type, &DieProc, 1);
	RegisterMessageProcessor(QueriesDoneMessage::type, &QueryFinishedProc, 1);
}


MESSAGE_HANDLER_DEFINITION_BEGIN(CoordinatorImp, DieProc, DieMessage){
	cerr << "============= TAKING THE SHOW DOWN ================" << endl;

	// killing the execution engine
	KillEvProc(evProc.execEngine);

	// killing ourselves
	evProc.Seppuku();

}MESSAGE_HANDLER_DEFINITION_END


MESSAGE_HANDLER_DEFINITION_BEGIN(CoordinatorImp, QueryFinishedProc, QueriesDoneMessage){
	// the exec engine finished, we take the show down

	msg.completedQueries.MoveToStart ();
	while (msg.completedQueries.RightLength ()) {
		msg.completedQueries.Current ().exit.Print ();
		msg.completedQueries.Current ().query.Print ();
		msg.completedQueries.Advance ();
		evProc.qDone++; // update the done counter so we know when we are done
	}

	if ( evProc.qDone==4)
		evProc.Seppuku();

}MESSAGE_HANDLER_DEFINITION_END
