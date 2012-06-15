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

#ifndef EE_MSG_H
#define EE_MSG_H

#include "Data.h"
#include "ID.h"
#include "ExecEngineData.h"
#include "History.h"
#include "Notifications.h"

// note that in all of these messages, "currentPos" should (usually) be set to be the
// identifier of the waypoint who is sending the message, since the execution engine
// delivers the message to the next logical recipient in the graph after currentPos

// this macro defines the "hopping data message", which is used to send data 
// through the network from a data producer. dest is all of the places that the msg 
// needs to go, lineage is a list of History objects (where each waypoint that
// obtains and then forwards on the message can add new history objects to the
// list), and data is the actual data in the message.
M4_CREATE_BASE_DATA_TYPE(HoppingDataMsg, DataC,
 </(currentPos, WayPointID)/>, 
 </(dest, QueryExitContainer), (lineage, HistoryList), (data, ExecEngineData)/>)

// this macro defines direct messages, that are sent to a particular waypoint
M4_CREATE_BASE_DATA_TYPE(DirectMsg, Data,
 </(receiver, WayPointID)/>,
 </(message, Notification)/>)

// this macro defines hopping downstream messages... these are a lot like hopping
// data messages, except that they contain Notification objects instead of data 
M4_CREATE_DATA_TYPE(HoppingDownstreamMsg, DataC,
 </(currentPos, WayPointID)/>, 
 </(dest, QueryExitContainer), (msg, Notification)/>)

// this macro defines hopping upstream messages... unlike the downstream messages,
// these have only one destination---the waypoint(s) that produce(s) data associated
// with a particular query exit
M4_CREATE_DATA_TYPE(HoppingUpstreamMsg, DataC,
 </(currentPos, WayPointID)/>, 
 </(dest, QueryExit), (msg, Notification)/>)

#endif
