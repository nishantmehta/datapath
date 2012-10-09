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

#ifndef GLA_DATA_H
#define GLA_DATA_H

#include <inttypes.h>

#include "Data.h"
#include "Chunk.h"
#include "TwoWayList.h"
#include "ID.h"
#include "ChunkID.h"
#include "QueryID.h"
#include "WayPointID.h"
#include "EfficientMap.h"

/** This header contains Data types useful for the GLA implementation */


/** Base class for GLAStates 

    glaType is the MD5 sum of the name. Used for debugging/correctness.
*/
M4_CREATE_BASE_DATA_TYPE(GLAState, DataC,
</(glaType, uint64_t)/>,<//>)

// container types for states
typedef TwoWayList <GLAState> GLAStateContainer;
typedef EfficientMap <QueryID, GLAState> QueryToGLAStateMap;
typedef EfficientMap <QueryID, GLAStateContainer> QueryToGLASContMap;

typedef EfficientMap< QueryID, Swapify<int> > QueryIDToInt;
typedef EfficientMap< QueryID, Swapify<bool> > QueryIDToBool;

typedef TwoWayList<WayPointID> ReqStateList;
typedef EfficientMap<QueryID, ReqStateList> QueryToReqStates;

/** GLAPointer stores a pointer to a GLA state. This is an in-memory
    pointer.
*/

M4_CREATE_DATA_TYPE(GLAPtr, GLAState,
</(glaPtr, void*)/>,<//>)

/******* Data containing QueryIDSet and Chunk **/
M4_CREATE_BASE_DATA_TYPE(CacheData, DataC,
 </(queryIDs, QueryIDSet)/>,
 </(cacheChunk, Chunk)/>,
 <//>)

typedef EfficientMap <ChunkID, CacheData> ChunkToCacheMap;

/** Data Structures for GIST Waypoints */

M4_CREATE_BASE_DATA_TYPE(GISTWorkUnit, DataC,
<//>,
</(gist, GLAState), (localScheduler, GLAState), (gla, GLAState)/>)

typedef TwoWayList <GISTWorkUnit> GistWUContainer;
typedef EfficientMap<QueryID, GISTWorkUnit> QueryToGistWorkUnit;
typedef EfficientMap<QueryID, GistWUContainer> QueryToGistWUContainer;

/******* TODO: add other representations for GLAStates such as serialized GLAs ********/


#endif // GLA_DATA_H
