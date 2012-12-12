dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Dudley
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

#ifndef _PROF_MSG_H_
#define _PROF_MSG_H_

#include "ProfMSG-Data.h"
#include "Message.h"
#include "Timer.h"

// messages to the profiler to record execution

// message with basic counter to be incremented
M4_CREATE_MESSAGE_TYPE(ProfileMessage,
</(wallTimeStart, double), (cTimeStart, clock_t), (wallTimeEnd, double), (cTimeEnd, clock_t)/>,
</(counter, PCounter)/>)


// message with a set of counters
M4_CREATE_MESSAGE_TYPE(ProfileSetMessage,
</(wallTimeStart, double), (cTimeStart, clock_t), (wallTimeEnd, double), (cTimeEnd, clock_t)/>,
</(counters, PCounterList)/>)

// message stating that the current time interval has elapsed
M4_CREATE_MESSAGE_TYPE(ProfileIntervalMessage,
</(wallTime, double), (cTime, clock_t)/>, <//>)

#endif //  _PROF_MSG_H_
