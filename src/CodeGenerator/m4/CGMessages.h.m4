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
	This header file contains the messages used by CodeGenerator.
*/

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _CG_MESSAGES_H_
#define _CG_MESSAGES_H_

#include <string>

#include "Message.h"
#include "SymbolicWaypointConfig.h"
#include "WayPointConfigureData.h"

using namespace std;


/** Message sent to the code generator to ask for new code to be generated.

		Arguments:
			dirName: directory where to generate the new code
			configs: configuration for existing waypoints
*/
M4_CREATE_MESSAGE_TYPE(GenerateNewCodeMessage,
	</(dirName, string)/>,
	</(configs, SymbolicWPConfigContainer)/>)

/** Message sent by the code generator when the new code was generated.

		Arguments:
			code: configuration objects for the waypoints
*/
M4_CREATE_MESSAGE_TYPE(GeneratedCodeMessage,
	<//>,
	</(code, WayPointConfigurationList)/>)

#endif // _CG_MESSAGES_H_
