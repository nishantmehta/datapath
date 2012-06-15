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

#ifndef GLA_LARGE_WAY_POINT_IMP
#define GLA_LARGE_WAY_POINT_IMP

#include "ID.h"
#include "History.h"
#include "Tokens.h"
#include "GLAWayPointImp.h"
#include "GLAData.h"
#include "GLAHelpers.h"
#include "GLALargeFragmentTracker.h"
#include "Constants.h"

/** WARNING: The chunk processing function has to return 0 and the
  finalize function 1 otherwise acknowledgments are not sent
  properly in the system
  */
class GLALargeWayPointImp : public GLAWayPointImp {

    // this is the set of queries for which we know we have received all of the data, and so we
    // are just waiting for a worker to be available so that we can actually finish them up
    QueryExitContainer queriesToComplete;

    QueryIDSet queriesCompleted;

    QueryToGlobalGLAStateMap myQueryToGlobalStates;

    // Keep track of the fragments that still need to be finalized for each query.
    // There should only be trackers in this map for queries for which we have
    // received and aggregated all of the data for that query.
    typedef EfficientMap<QueryID, GLALargeFragmentTracker> QueryToFragmentTrackerMap;
    QueryToFragmentTrackerMap queryFragmentMap;

    typedef TwoWayList< Swapify<int> > SegmentIDList;
    typedef EfficientMap< QueryID, SegmentIDList > QueryToSegmentListMap;
    QueryToSegmentListMap queriesToCountFrags;

    // Overwritten GLAWayPointImp functions
    void GotChunkToProcess( CPUWorkToken& token, QueryExitContainer &whichOnes, ChunkContainer& chunk, HistoryList& lineage );

    bool PreFinalizePossible( CPUWorkToken& token );
    bool FinalizePossible( CPUWorkToken& token );
    bool PostFinalizePossible( CPUWorkToken& token );

    void ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data);
    void PreFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );
    void FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );
    void PostFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    bool ReceivedQueryDoneMsg( QueryExitContainer& whichOnes );

    public:

    // constructor and destructor
    GLALargeWayPointImp ();
    virtual ~GLALargeWayPointImp ();

    // these are just implementations of the standard WayPointImp functions
    void ProcessAckMsg (QueryExitContainer &whichOnes, HistoryList &lineage);
    void ProcessDropMsg (QueryExitContainer &whichOnes, HistoryList &lineage);
    void TypeSpecificConfigure (WayPointConfigureData &configData);
};

#endif
