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

#ifndef NOTIFY_H
#define NOTIFY_H

#include "Data.h"
#include "ID.h"
#include "ExecEngineData.h"
#include "History.h"

// this macro defines the objects that are used to store notifications that are
// sent from one waypoint to another in the network (notifications are things
// like: "ready to start processing chunks for a query exit" or "done sending
// chunks for a query exit"... sender identifies the waypoint who sent the
// notification
M4_CREATE_BASE_DATA_TYPE(Notification, DataC,
 </(sender, WayPointID)/>,
 <//>)

// this is a specific type of notification that tells us that a query or queries are done,
// in the sense that no more data is going to be coming through the system
M4_CREATE_DATA_TYPE(QueryDoneMsg, Notification,
 <//>,
 </(whichOnes, QueryExitContainer)/>)

// this type of notification tells the receiver that it is time to start sending data
// to one particualr query exit
M4_CREATE_DATA_TYPE(StartProducingMsg, Notification,
 <//>,
 </(whichOne, QueryExit)/>)

// this is used to notify the hash cleaner that some segments were too full... contains
//  set of sampled entries from all of the too-full segments
M4_CREATE_DATA_TYPE(TooFullMessage, Notification,
 <//>,
 </(whatWeFound, HashSegmentSample)/>)

// this is sent from the hash cleaner to a particular waypoint to notify the waypoint
// that it has now become disk-based
M4_CREATE_DATA_TYPE(IKillYouMessage, Notification,
 <//>,
 <//>)

// when a join waypoint finlly dies (that is, when a "wounded" waypoint that is being emptied
// out of the hash table has its last bytes ever removed from the hash table) it sends this
// message to both the hash cleaner and to the writer letting them know that it has finally died
M4_CREATE_DATA_TYPE(WayPointDeadMsg, Notification,
 <//>,
 <//>)


#endif
