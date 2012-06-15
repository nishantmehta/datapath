dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE
	
#include "DataPathGraph.h"
#include "Tokens.h"
#include "WayPointConfigureData.h"
#include "EEMessageTypes.h"

#ifndef _EXEC_ENGINE_MESSAGES_H_
#define _EXEC_ENGINE_MESSAGES_H_

// this has all of the message types that can be sent to the execution engine from outside...
// right now there are only three: a configuration message, a data message to send thru
// the system, and a message to just give a token back

// this is the message that is sent to the execution engine to modify/add the graph and/or 
// the waypoints that are currently active in the system
M4_CREATE_MESSAGE_TYPE(ConfigureExecEngineMessage,
 <//>,
 </(newGraph, DataPathGraph), (configs, WayPointConfigurationList)/>)

// this is the message that is sent to the execution engine by a worker to send a hopping
// data message through the graph.  The secod param ("token") is the work token that was used
// to authorize the actual work that was done.  The first param is the return value from the
// function that was called to produce the data... 
M4_CREATE_MESSAGE_TYPE(HoppingDataMsgMessage,
 </(returnVal, int)/>,
 </(token, GenericWorkToken), (message, HoppingDataMsg)/>)

//////////// QUERIES DONE MESSAGE /////////////

/** When the execution engine has determined that some queries have completed
		execution, it sends the following message to the controller, which lists all
		of the query/exit combos for which work has been completed.

		Arguments:
			completedQueries: the set of completed queries
*/
M4_CREATE_MESSAGE_TYPE(QueriesDoneMessage,
  <//>,
  </(completedQueries, QueryExitContainer)/>)


#endif
