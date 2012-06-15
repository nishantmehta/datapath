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
include(RemoteMessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include "RemoteMessage.h"
#include "EventProcessor.h"

dnl This file describes the messages needed only in this test.

///////////// TEST GENERATOR MESSAGE  ////////////

/**
Message used in the CommunicationFramework test.
It is sent by the KeyboardReader to the Adder on the remote host through its
local proxy event processor.

Arguments:
	number: the number to be added
*/

M4_CREATE_REMOTE_MESSAGE_TYPE(AddMessage,
	</(number, long int)/>,
	<//>)


/**
Message used in the CommunicationFramework test.
It is sent by the server to the client on the remote host through its
local proxy event processor.

Arguments:
	sum: the sum of numbers sent from the client
*/

M4_CREATE_REMOTE_MESSAGE_TYPE(KillMessage,
	</(sum, long int)/>,
	<//>)


#endif // _MESSAGES_H_
