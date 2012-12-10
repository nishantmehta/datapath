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
include(DataFunctions.m4)
dnl # // END OF M4 CODE

#ifndef _PROF_MSG_DATA_H_
#define _PROF_MSG_DATA_H_

#include "Data.h"
#include "TwoWayList.h"

/** Container for counters

*/
M4_CREATE_DATA_TYPE(PCounter, Data, 
</(name, string), (value, int64_t)/>,<//>) 

typedef TwoWayList <PCounter> PCounterList;

#endif // _PROF_MSG_DATA_H_
