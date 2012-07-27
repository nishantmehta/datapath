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

#ifndef EE_DATA_H
#define EE_DATA_H

#include "Data.h"
#include "Chunk.h"
#include "HString.h"
#include "JoinWayPointID.h"
#include "HashData.h"
#include "GLAData.h"
#include "GlobalGLAState.h"
#include "HashTableSegment.h"
#include "Tokens.h"
#include <stdio.h>

// this file has all of the data types that can be sent downstream thru the
// data path graph

// this macro defines the "ExecEngineData" class, which is a generic container
// for data that is sent downstream from one waypoint to another to be processed.
M4_CREATE_BASE_DATA_TYPE(ExecEngineData, DataC,
 <//>,
 <//>)

// this is the most common type of EEData object: one that contains a single chunk
M4_CREATE_DATA_TYPE(ChunkContainer, ExecEngineData,
 <//>,
 </(myChunk, Chunk)/>)

// another type of EEData used to transport states between waypoints
M4_CREATE_DATA_TYPE(StateContainer, ExecEngineData,
</(whichIndex, int)/>,
</(whichQuery, QueryExit), (myState, GLAState)/>)

// this is what is returned by a join worker that has put data into the hash table.
// It lists a small sample of the collisions that were found to happen
M4_CREATE_DATA_TYPE(JoinHashResult, ExecEngineData,
 <//>,
 </(sampledQueries, HashSegmentSample)/>)

// this is what comes out of the hash table extraction waypoint
M4_CREATE_DATA_TYPE(ExtractionContainer, ExecEngineData,
 </(whichSegment, int)/>,
 </(diskTokenQueue, DiskWorkTokenQueue), (result, ExtractionList)/>)

M4_CREATE_DATA_TYPE(ExtractionResult, ExecEngineData,
 </(whichSegment, int)/>,
 </(newSegment, HashTableSegment), (result, ExtractionContainer)/>)

// return data type for Bulk Loader. Need to pass more stuff back
// Arguments:
//   loclDictionary: the local dictionary used by this thread
//   stream: the FILE descriptor used by this thread
//   noTuples: the number of tuples produced by this thread
M4_CREATE_DATA_TYPE(TextLoaderResult, ExecEngineData,
 </(stream, FILE*), (noTuples, off_t)/>,
 </(localDictionary, HString::DictionaryWrapper), (myChunk, Chunk)/>)

dnl M4_CREATE_DATA_TYPE(JoinMergeChunkContainer, ExecEngineData,
dnl<//>,
dnl </(myChunksLHS, ContainerOfChunks),(myChunksRHS, ContainerOfChunks)/>)


/****** Return types from GLA Process chunk *******/

/** Results from preprocessing */
M4_CREATE_DATA_TYPE(GLAPreProcessRez, ExecEngineData,
<//>,
</(constStates, QueryToGLASContMap), (statesNeeded, QueryIDToInt)/>)

/** Results containing  GLAStates */
M4_CREATE_DATA_TYPE(GLAStatesRez, ExecEngineData,
<//>,
</(glaStates, QueryToGLAStateMap)/>)

/** special version used by GLA merge */
M4_CREATE_DATA_TYPE(GLAStatesFrRez, ExecEngineData,
<//>,
</(glaStates, QueryToGLAStateMap), (constStates, QueryToGLASContMap), (fragInfo, QueryIDToInt), (queriesToIterate, QueryIDSet)/>)

/** Results containing GlobalGLAStates */
M4_CREATE_DATA_TYPE( GlobalGLAStatesRez, ExecEngineData,
<//>,
</(glaStates, QueryToGlobalGLAPtrMap)/>)

#endif
