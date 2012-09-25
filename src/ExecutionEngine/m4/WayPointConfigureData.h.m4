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

#ifndef WAY_POINT_CONFIG_H
#define WAY_POINT_CONFIG_H

#include "TwoWayList.h"
#include "ID.h"
#include "WorkFuncs.h"
#include "ContainerTypes.h"
#include "GLAData.h"

// this contains all of the info needed to configure a waypoint

// this is the basic configuration data type that is sent into a waypoint... myID is the waypoint's identifier; 
// funcList is a list of functions to add to the waypoint (if another function with the same wrapper is in the
// waypoint, it is overwritten), endingQueryExits is the set of query exits that go no further than the waypint,
// and flowThroughQueryExits is the set of query exits whose data goes through the waypoint on its way to somewhere
// else in the system
M4_CREATE_BASE_DATA_TYPE(WayPointConfigureData, Data,
 </(myID, WayPointID)/>,
 </(funcList, WorkFuncContainer), (endingQueryExits, QueryExitContainer), 
   (flowThroughQueryExits, QueryExitContainer)/>)

// we should have one of these for each of the types of waypoints in the system...
// this contains all of the additional data that is needed to configure the waypoint,
// above and beyond the data that is held in the basic WayPointConfigureData class.
M4_CREATE_DATA_TYPE(SelectionConfigureData, WayPointConfigureData, <//>, <//>)
M4_CREATE_DATA_TYPE(AggregateConfigureData, WayPointConfigureData, <//>, <//>)
M4_CREATE_DATA_TYPE(WriterConfigureData, WayPointConfigureData, <//>,<//>)
M4_CREATE_DATA_TYPE(JoinMergeConfigureData, WayPointConfigureData, <//>,<//>)
M4_CREATE_DATA_TYPE(GLAConfigureData, WayPointConfigureData, <//>,
</(reqStates, QueryToReqStates), (resultIsState, QueryIDToBool)/>)

M4_CREATE_DATA_TYPE(GTConfigureData, WayPointConfigureData, <//>,
</(reqStates, QueryToReqStates)/>)

struct PrintFileInfo {
    string file;
    string header;
    string separator;

    PrintFileInfo() : file(""), header(""), separator("") {}
    PrintFileInfo( string file, string header, string separator ) :
        file(file), header(header), separator(separator) {}
};
typedef Swapify< PrintFileInfo > FileInfoObj;
typedef EfficientMap< QueryID, FileInfoObj> QueryToFileInfoMap;

M4_CREATE_DATA_TYPE(PrintConfigureData, WayPointConfigureData, <//>,
</(queriesInfo, QueryToFileInfoMap)/>)

// this contains a copy of the central hash table
M4_CREATE_DATA_TYPE(HashTableCleanerConfigureData, WayPointConfigureData,
<//>,
</(centralHashTable, HashTable)/>)

// this contains a copy of the central hash table, as well as the "disk based twin" waypoint that will be used
// if this waypoint gets kicked out of the hash table.  It also contains a list of the new queries that have
// never been seen before by this join waypoint, since these are treated differently than the existing queries
// in that some state must be recorded for them.  Note that newEndingQueries must be a subset of endingQueryExits,
// and newFlorThruQueries must be a subset of flowThroughQueryExits
M4_CREATE_DATA_TYPE(JoinConfigureData, WayPointConfigureData, 
</(myDiskBasedTwinID, WayPointID), (hashTableCleaner, WayPointID)/>, 
</(newEndingQueries, QueryExitContainer), (newFlowThruQueries, QueryExitContainer),
  (centralHashTable, HashTable)/>)

// for my toy table scan waypoint, we need to supply the query exits to send data to, and whethere this is supplying
// data to the LHS or the RHS of the single join in the plan
M4_CREATE_DATA_TYPE(TableScanConfigureData, WayPointConfigureData, 
 </(isLHS, int)/>, 
 </(myExits, QueryExitContainer)/>)

/** real table scanner/writter. 

		Arguments:
		  relName: relation name
			deletedQE: the queries that finished
			newQE: the query-exitsWP for which we have to generate chunks
			columnsToSlotsMap: the mapping from physical columns to slots
			queryColumnsMap: the mapping from query-exitWP to slots they need
*/			 
M4_CREATE_DATA_TYPE(TableConfigureData, WayPointConfigureData, 
 </(relName, string)/>, 
 </(newQE, QueryExitContainer), (deletedQE, QueryExitContainer), (queryColumnsMap, QueryExitToSlotsMap), (columnsToSlotsMap, SlotToSlotMap)/>)


/** Text Loader
		
		Arguments:
				files: names of files that we are bulkloading
				queries: the queries that are going to tag the produced chunks
*/
M4_CREATE_DATA_TYPE(TextLoaderConfigureData, WayPointConfigureData, 
</(files, StringContainer)/>,
</(queries, QueryExitContainer)/>)

M4_CREATE_DATA_TYPE(TileJoinConfigureData, WayPointConfigureData,
 </(lhsrelName, string), (rhsrelName, string)/>,
 </(JoinWP, WayPointID), (queryColumnsMapLhs, QueryExitToSlotsMap), (queryColumnsMapRhs, QueryExitToSlotsMap), (columnsToSlotsPairLhs, SlotPairContainer), (columnsToSlotsPairRhs, SlotPairContainer)/>)

// this is the list of way point configurations sent into the execution engine
typedef TwoWayList<WayPointConfigureData> WayPointConfigurationList;

#endif
