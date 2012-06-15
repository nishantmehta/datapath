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
	This header file contains the messages used by CodeLoader.
*/

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _CL_MESSAGES_H_
#define _CL_MESSAGES_H_

#include <string>

#include "Message.h"
#include "SymbolicWaypointConfig.h"
#include "WayPointConfigureData.h"

using namespace std;


/** Message sent to the code loader to ask for new code to be loaded.

		Arguments:
			dirName: directory where the new code is (previously generated)
			configs: configuration for existing waypoints
*/
M4_CREATE_MESSAGE_TYPE(LoadNewCodeMessage,
	</(dirName, string)/>,
	</(configs, WayPointConfigurationList)/>)

/** Message sent by the code loader when the new code was loaded.

		Arguments:
			code: new configuration objects for the waypoints
*/
M4_CREATE_MESSAGE_TYPE(LoadedCodeMessage,
	<//>,
	</(configs, WayPointConfigurationList)/>)

#endif // _CL_MESSAGES_H_
