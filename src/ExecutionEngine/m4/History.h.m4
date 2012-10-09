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

#ifndef HISTORY_H
#define HISTORY_H

#include "Data.h"
#include "TwoWayList.h"
#include "ID.h"
#include "TwoWayList.cc"
#include "ChunkID.h"

// this macro defines the history objects that are used to store lineage info
// that follows data objects as they move through the path network.  As a waypoint
// processes a data message, it is free to add a new history object to the end of
// the HistoryList that accompanies the the data through the network.  Then, as
// the data message is either acked or dropped, that history object will be
// retured at a later time so that the waypoint can deal with the ack or drop.
// whichWayPoint tells us which waypoint created the history object
M4_CREATE_BASE_DATA_TYPE(History, DataC,
 </(whichWayPoint, WayPointID)/>,
 <//>)

// this defines the HistroyList data structure... new History objects are appended
// to this as a data message works its way through the data path graph
typedef TwoWayList <History> HistoryList;

// history specific to an aggregate waypoint
M4_CREATE_DATA_TYPE(AggHistory, History,
 <//>,
 <//>)

// history specific to an GLA waypoint
M4_CREATE_DATA_TYPE(GLAHistory, History,
 </(whichFragment, int)/>,
 <//>)

M4_CREATE_DATA_TYPE(GISTHistory, History,
</(whichFragment, int)/>,
<//>)

// history specific to the hash table cleaner
M4_CREATE_DATA_TYPE(HashCleanerHistory, History,
 </(whichSegment, int)/>,
 <//>)

// history specific to a table scan waypoint
M4_CREATE_DATA_TYPE(TableScanHistory, History,
 </(whichChunk, int)/>,
 </(whichExits, QueryExitContainer)/>)

// history specific to a table waypoint
M4_CREATE_BASE_DATA_TYPE(TableHistory, History,
 </(whichChunk, ChunkID)/>,
 </(whichExits, QueryExitContainer)/>)

// two types of specific histories for Table
M4_CREATE_DATA_TYPE(TableReadHistory, TableHistory, 
 <//>,
 <//>)
// this is not used now but might be used in the future
// if writer part of Table neds some extra data
M4_CREATE_DATA_TYPE(TableWriteHistory, TableHistory, 
 <//>,
 <//>)																					 

// history for text lodaders. Contains info about which stream produced the chunk 
M4_CREATE_DATA_TYPE(TextLoaderHistory, History,
 </(whichChunk, ChunkID), (file, string)/>,
 <//>)

M4_CREATE_DATA_TYPE(TileJoinScanHistory, History,
 </(bucketID, int), (whichChunk, ChunkID), (isLHS, bool), (file, string)/>,
 </(whichExits, QueryExitContainer)/>)

M4_CREATE_DATA_TYPE(TileJoinWriteHistory, History,
 <//>,
 <//>)

M4_CREATE_DATA_TYPE(TileJoinMergeHistory, History,
 </(bucketID, __uint64_t)/>,
 </(chunkIDLHSList, ChunkIDContainer), (chunkIDRHSList, ChunkIDContainer), (whichExits, QueryExitContainer)/>)

#endif
