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

#ifndef _DP_MESSAGES_H_
#define _DP_MESSAGES_H_

#include <string>

using namespace std;


////////// NEW PLAN //////////////////////

/** Message is sent by the main program to the coordinator with new xml files
		containing plans.

		Arguments:
			xmlFile: full path to the xml file containing the new plan
*/
M4_CREATE_MESSAGE_TYPE(NewPlan,
	</(confFile, string)/>,
	<//>)

#endif // _TRANS_MESSAGES_H_
