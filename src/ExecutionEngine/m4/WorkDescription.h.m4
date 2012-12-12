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

#ifndef WORK_DESCRIPTION_H
#define WORK_DESCRIPTION_H

#include "Data.h"
#include "GLAData.h"
#include "Chunk.h"
#include "AggStorageMap.h"
#include "HashTable.h"
#include "JoinWayPointID.h"
#include "ExecEngineData.h"

// this is the base class for the hierarchy of types that one can send to a CPU
// worker to describe the task that the worker is supposed to complete.  In more
// detail, a CPU worker will always execute a function of type WorkFunc
// (see WorkerMessages.h.m4)... functions of type WorkFunc accept data of type
// WorkDescription.  Sooo, if a specific waypoint wants a function executed, it
// sends a specific WorkFunc to a worker, along with a specific WorkDescription
// object that it has created to parameterize the function

M4_CREATE_BASE_DATA_TYPE(WorkDescription, Data,
<//>, <//>)

M4_CREATE_DATA_TYPE(SelectionPreProcessWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer)/>)

M4_CREATE_DATA_TYPE(SelectionProcessChunkWD, WorkDescription,
</(chunkID, ChunkID)/>,
</(whichQueryExits, QueryExitContainer), (chunkToProcess, Chunk), (constStates, QueryToGLASContMap)/>)

struct PrintFileObj {
    FILE* file;
    string separator;

    PrintFileObj() : file(NULL), separator("") {}
    PrintFileObj(FILE* file, string separator) :
        file(file), separator(separator) {}
};
typedef Swapify<PrintFileObj> FileObj;
typedef EfficientMap<QueryID, FileObj> QueryToFileMap;

// this is the work description for a print
// streams contains file descriptors for all files with query results
M4_CREATE_DATA_TYPE(PrintWorkDescription, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (streams, QueryToFileMap), (chunkToPrint, Chunk)/>)

// this is a work descrption for a table scan... will likely go away when we move
// past the toy version.  Right now, what this has is a list of all of the query
// exits that should appear in the chunk, as well as what chunk ID we should produce
M4_CREATE_DATA_TYPE(TableScanWorkDescription, WorkDescription,
</(whichChunk, int), (isLHS, int)/>,
</(whichQueryExits, QueryExitContainer)/>)

// this is the work description for an aggregate operation... there are two types
// of work descriptions.  The first one just aggregates the one chunk that is sent
M4_CREATE_DATA_TYPE(AggregateOneChunk, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (aggFuncs, AggStorageMap), (chunkToProcess, Chunk)/>)

// this one finishes up the aggregation process for a specific set of query-exits
M4_CREATE_DATA_TYPE(FinishAggregate, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (aggFuncs, AggStorageMap)/>)

// this one is for the LHS of a join (the probing side)... note that wayPointID is NOT the
// same thing as the the WayPointID associated with one of the waypoints.  It is a unique int,
// managed by the JoinWayPointImp class, that is associated with each join way point.  It is
// put into the central hash table so that we do not mix tuples from different join waypoints
M4_CREATE_DATA_TYPE(JoinLHSWorkDescription, WorkDescription,
</(wayPointID, int)/>,
</(whichQueryExits, QueryExitContainer), (chunkToProcess, Chunk), (centralHashTable, HashTable)/>)

// this one is for the RHS of a join (the hashing side)
M4_CREATE_DATA_TYPE(JoinRHSWorkDescription, WorkDescription,
</(wayPointID, int)/>,
</(whichQueryExits, QueryExitContainer), (chunkToProcess, Chunk), (centralHashTable, HashTable)/>)

// this one is for the RHS of a join (the hashing side)
M4_CREATE_DATA_TYPE(JoinLHSHashWorkDescription, WorkDescription,
</(wayPointID, int)/>,
</(whichQueryExits, QueryExitContainer), (chunkToProcess, Chunk), (centralHashTable, HashTable)/>)

M4_CREATE_DATA_TYPE(JoinMergeWorkDescription, WorkDescription,
</(wayPointID, WayPointID), (start, int), (end, int)/>,
</(whichQueryExits, QueryExitContainer), (chunksLHS, ContainerOfChunks), (chunksRHS, ContainerOfChunks)/>)

// work description for the hash table cleaner
M4_CREATE_DATA_TYPE(HashCleanerWorkDescription, WorkDescription,
</(whichSegment, int)/>,
</(centralHashTable, HashTable), (diskTokenQueue, DiskWorkTokenQueue),
  (dyingWayPointsToSend, JoinWayPointIDList), (dyingWayPointsToHold, JoinWayPointIDList),
  (theseQueriesAreDone, QueryExitContainer), (equivalences, JoinWayPointIDEquivalences)/>)


// Text loader instructions.
// Arguments:
//   loclDictionary: the local dictionary used by this thread
//   stream: the FILE descriptor used by this thread
M4_CREATE_DATA_TYPE(TextLoaderWorkDescription, WorkDescription,
</(stream, FILE*), (file, string)/>,
</(whichQueryExits, QueryExitContainer)/>)

// this is used for the "test" version of the writer... sends the waypoint that this work is being done for
// as well as all of the chunks that have been sent
M4_CREATE_DATA_TYPE(WriterWorkDescription, WorkDescription,
</(whichWayPoint, WayPointID), (whichSegment, int)/>,
</(extractionList, ExtractionList)/>)


/***** WorkDescriptions *****/

/** work for GLAPreProcessWorkFunc:

    only needs the list of query exits.
*/
M4_CREATE_DATA_TYPE(GLAPreProcessWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer)/>)

/** work for GLAProcessChunkWorkFunc:

    glaStates: a map and must have an element for each value in whichQueryExits
               if the state exists. Missing entries result in creation of the state.
*/
M4_CREATE_DATA_TYPE(GLAProcessChunkWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaStates, QueryToGLAStateMap), (constStates, QueryToGLASContMap), (chunkToProcess, Chunk), (garbageStates, QueryToGLAStateMap)/>)

/*** work description for GLAMergeStatesWorkFunc
     glaStates contains a list of states for each query
*/
M4_CREATE_DATA_TYPE(GLAMergeStatesWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaStates, QueryToGLASContMap)/>)

/*** work description for GLAPreFinalizeWorkFunc
     glaStates contains a map from queryID to GLAState
*/
M4_CREATE_DATA_TYPE(GLAPreFinalizeWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaStates, QueryToGLAStateMap), (constStates, QueryToGLASContMap)/>)

/*** work description for the finalize function

     fragInfo specifies the fragment number to use for each query that supports fragments

*/
M4_CREATE_DATA_TYPE(GLAFinalizeWD, WorkDescription,
</(fragmentNo, int)/>,
</(whichQueryExit, QueryExit), (glaState, GLAState)/>)

/***** Generalized Filter Work Descriptions *****/

M4_CREATE_DATA_TYPE(GTPreProcessWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer)/>)

M4_CREATE_DATA_TYPE(GTProcessChunkWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (filters, QueryToGLAStateMap), (constStates, QueryToGLASContMap), (chunkToProcess, Chunk)/>)

/***** GIST Work Descriptions *****/

// Creates any needed generated constant states and, along with the
// received states, creates the GIST state.
M4_CREATE_DATA_TYPE(GISTPreProcessWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (receivedStates, QueryToGLASContMap)/>)

// Gets a new Global Scheduler from the GIST and uses it to generate the new
// round's local schedulers
M4_CREATE_DATA_TYPE(GISTNewRoundWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (gists, QueryToGLAStateMap)/>)

// Uses the gist state, local scheduler and gla to perform steps on the gist
// until either the local scheduler is exhausted or a timeout is reached.
M4_CREATE_DATA_TYPE(GISTDoStepsWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (workUnits, QueryToGistWorkUnit)/>)

// Merges the GLAs for a query together.
M4_CREATE_DATA_TYPE(GISTMergeStatesWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaStates, QueryToGLASContMap)/>)

// Consults with the merged GLA to determine whether or not the GIST should
// go for another round or produce results this round.
M4_CREATE_DATA_TYPE(GISTShouldIterateWD, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaStates, QueryToGLAStateMap), (gists, QueryToGLAStateMap)/>)

// Produces results for this GIST.
M4_CREATE_DATA_TYPE(GISTProduceResultsWD, WorkDescription,
</(fragmentNo, int)/>,
</(whichOne, QueryExit), (gist, GLAState)/>)

#endif // WORK_DESCRIPTION_H
