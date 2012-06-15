//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef GLA_SMALL_WAY_POINT_IMP
#define GLA_SMALL_WAY_POINT_IMP

#include "ID.h"
#include "History.h"
#include "Tokens.h"
#include "GLAWayPointImp.h"
#include "GLAData.h"
#include "GLAHelpers.h"
#include "Constants.h"

//a constant just to allow changing number of states to be merged at a time
#define MERGE_AT_A_TIME 2

/** WARNING: The chunk processing function has to return 0 and the
        finalize function 1 otherwise acknowledgments are not sent
        properly in the system
*/
class GLASmallWayPointImp : public GLAWayPointImp {
    // this is the set of queries for which we know we have received all of the data, and so we
    // are just waiting for a worker to be available so that we can actually finish them up
    QueryExitContainer queriesToComplete;

    QueryIDSet queriesCompleted;

    // container for states
    QueryToGLASContMap myQueryToGLAStates;

    // States that were merged, need to finalize them
    QueryToGLAStateMap mergedStates;
    // the queryExits merged
    QueryExitContainer mergedQueries;

    QueryFragmentMap queryFragmentMap;

    // the fragments we still have to produce for each query
    // when counter is at 0, we are done
    QueryIDToInt fragmentsLeft;

    // A counter for each query representing if the merge is in progress or not
    // 0 = done, >0 = in progress
    QueryIDToInt mergeInProgress;

    // last fragment we generated to ensure a circular list behavior
    off_t lastFragmentId;

    // Helper method
    bool MergeDone();

    // Overwritten virtual methods
    void GotChunkToProcess( CPUWorkToken & token, QueryExitContainer& whichOnes, ChunkContainer& chunk, HistoryList& lineage);

    bool PostProcessingPossible( CPUWorkToken& token );
    bool FinalizePossible( CPUWorkToken& token );

    void ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data);
    void PostProcessComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );
    void FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    bool ReceivedQueryDoneMsg( QueryExitContainer& whichOnes );

public:

    // constructor and destructor
    GLASmallWayPointImp ();
    virtual ~GLASmallWayPointImp ();


    void ProcessAckMsg (QueryExitContainer &whichOnes, HistoryList &lineage);
    void ProcessDropMsg (QueryExitContainer &whichOnes, HistoryList &lineage);
    void TypeSpecificConfigure (WayPointConfigureData &configData);

};

#endif
