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
	This header file contains the messages used by Workers.
*/

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _WORK_MESSAGES_H_
#define _WORK_MESSAGES_H_

#include "MessageMacros.h"
#include "Message.h"
#include "Worker.h"
#include "WorkUnit.h"


//////////// WORK TO DO MESSAGE /////////////

/** Message sent by ExecutionEngine to request work to be done by a Worker.

		Arguments:
			workUnit: work to be done
			myName: handle for the Worker
*/
M4_CREATE_MESSAGE_TYPE(WorkToDoMessage,
   <//>,
   </(workUnit, WorkUnit), (myName, Worker)/>)


//////////// WORK DONE MESSAGE /////////////

/** Message sent by Worker to the ExecutionEngine when the work is done.

		Arguments:
			timeS: how long did it take to execute the work
			runThis: work that has been done
			sentFrom: Worker who did the work and sent the message
*/
M4_CREATE_MESSAGE_TYPE(WorkDoneMessage,
  </(timeS, double)/>,
  </(runThis, WorkUnit), (sentFrom, Worker)/>)

#endif // _WORK_MESSAGES_H_
