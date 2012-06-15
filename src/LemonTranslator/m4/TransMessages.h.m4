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
/**
	This header file contains the messages used by DiskIO.
*/

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _TRANS_MESSAGES_H_
#define _TRANS_MESSAGES_H_

#include "Message.h"
#include "DataPathGraph.h"
#include "WayPointConfigureData.h"

#include <string>

using namespace std;


////////// SYMBOLIC QUERY DESCRIPTIONS /////////

/** Message sent by the Translator to the Coordinator to give it the symbolic
		description. This description contains both information for
		the CodeGenerator and for the ExecutionEngine.
		The info for the FileScanners is sent directly.

		Arguments:
			newGraph: the new graph for the execution engine
			wpDesc: symbolic configuration for the WayPoints
*/
M4_CREATE_MESSAGE_TYPE(SymbolicQueryDescriptions,
	<//>,
	</(newQueries, QueryExitContainer), (newGraph, DataPathGraph), (wpDesc, WayPointConfigurationList)/>)


///// LEMON TRANSLATOR MESSAGES //////

/** Message is only used to implement the SimpleTranslator.
		It is going to be changed for the full translator.
		The message is sent by the Coordinator to the SimpleTranslator.

		Arguments:
			file: full path to the file with instructions for translator. dp file
*/
M4_CREATE_MESSAGE_TYPE(TranslationMessage,
	</(confFile, string)/>,
	<//>)

/** Message to specify that a set of queries have finished 

		Arguments:
				queries: the set of queries that finished
*/
M4_CREATE_MESSAGE_TYPE(DeleteQueriesMessage,
	</(queries, QueryIDSet)/>,
	<//>)


#endif // _TRANS_MESSAGES_H_
