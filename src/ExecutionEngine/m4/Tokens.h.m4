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

include(DataFunctions.m4)

#ifndef TOKENS_H
#define TOKENS_H

// this file contains all of the various work token types

#include "Data.h"
#include "DistributedQueue.cc"

// this macro defines the generic token type, which is sent to waypoints when they
// request the ability to do some work... tokens must also be sent back to the
// execution engine when the work is done.  grantedTo is the identifier for the
// waypoint who was actually granted the right to use the token
M4_CREATE_BASE_DATA_TYPE(GenericWorkToken, Data,
 </(label, int)/>,
 <//>)

// this macro defines the CPU token type
M4_CREATE_DATA_TYPE(CPUWorkToken, GenericWorkToken,
 <//>,
 <//>)

// this macro defines the Disk token type
M4_CREATE_DATA_TYPE(DiskWorkToken, GenericWorkToken,
 <//>,
 <//>)

// this is used to send a list of disk tokens for writer waypoints to use
typedef DistributedQueue <DiskWorkToken> DiskWorkTokenQueue;

#endif
