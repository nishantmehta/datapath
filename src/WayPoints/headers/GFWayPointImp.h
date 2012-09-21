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

#ifndef GF_WaY_POINT_IMP
#define GF_WAY_POINT_IMP

#include "ID.h"
#include "History.h"
#include "Tokens.h"
#include "WayPointImp.h"
#include "GLAData.h"
#include "GLAHelpers.h"
#include "Constants.h"
#include "GLAWayPointImp.h"

class GFWayPointImp : public GLAWayPointImp {

    /*
     * Class Members
     */

    // QueryIDSets for the various stages a query can be in.
    QueryIDSet queriesToPreprocess;
    QueryIDSet queriesProcessing;
    QueryIDSet queriesCompleted;

    // container for cached GFs
    QueryToGLASContMap queryToGFs;

    // container for constant states
    QueryToGLASContMap constStates;

    // A counter for each query representing how many state objects the
    // GF is waiting on to begin processing.
    QueryIDToInt statesNeeded;

    // Used to keep track of which constant states go where in the constStates list.
    typedef map< WayPointID, int > ConstStateIndexMap;
    typedef map< QueryID, ConstStateIndexMap > QueryToConstStateIndexMap;
    QueryToConstStateIndexMap constStateIndex;

    // Map of QueryID to QueryExit
    typedef EfficientMap<QueryID, QueryExit> QueryIDToExitMap;
    QueryIDToExitMap queryIdentityMap;

    /*
     * Overridden GLAWayPointImp functions
     */

    bool PreProcessingPossible( CPUWorkToken& token );
    bool PreProcessingComplete( QueryExitContainer& whichOnes,
            HistoryList& history, ExecEngineData& data);

    void GotState( StateContainer& state );

    void GotChunkToProcess( CPUWorkToken& token, QueryExitContainer& whichOnes,
            ChunkContainer& chunk, HistoryList& history );
    bool ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data);

    bool ReceivedQueryDoneMsg( QueryExitContainer& whichOnes );
    bool ReceivedStartProducingMsg( HoppingUpstreamMsg& message, QueryExit& whichOne );

public:

    // Constructor and destructor
    GFWayPointImp ();
    virtual ~GFWayPointImp ();

    /*
     * Overridden WayPointImp functions
     */
    void TypeSpecificConfigure( WayPointConfigureData& configData );
};

#endif // GF_WAY_POINT_IMP
