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

#include "GFWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"
#include "Constants.h"

#include <iostream>

// Note: I have arranged the function definitions here in a different order
// than they are declared in the header. This is because I wanted to arrange
// the definitions in approximately the order the functions should be called.

GFWayPointImp :: GFWayPointImp() {
    PDEBUG( "GFWayPointImp :: GFWayPointImp()" );
    SetResultExitCode( PROCESS_CHUNK );
}

GFWayPointImp :: ~GFWayPointImp() {
    PDEBUG( "GFWayPointImp :: ~GFWayPointImp()" );
}

void GFWayPointImp :: TypeSpecificConfigure( WayPointConfigureData& configData ) {
    PDEBUG( "GFWayPointImp :: TypeSpecificConfigure()" );

    GFConfigureData myConfig;
    myConfig.swap(configData);

    QueryExitContainer queries;
    GetFlowThruQueryExits(queries);

    FOREACH_TWL(iter, queries) {
        QueryID q=iter.query;
        GLAStateContainer cont; // empty container
        if( !queryToGFs.IsThere(q) ) {
            queryToGFs.Insert(q, cont);
        }

        q = iter.query;
        QueryExit temp = iter;

        queryIdentityMap.Insert(q, iter);
    } END_FOREACH;

    QueryToReqStates& reqStates = myConfig.get_reqStates();

    FOREACH_EM(query, list, reqStates) {
        int index = 0;
        GLAStateContainer myConstStates;
        FOREACH_TWL(sourceWP, list) {
            constStateIndex[query][sourceWP] = index++;
            GLAState dummy;
            myConstStates.Append(dummy);
        } END_FOREACH;

        QueryID key = query;
        constStates.Insert(key, myConstStates);

        key = query;
        Swapify<int> numNeeded(index);
        statesNeeded.Insert(key, numNeeded);

    } END_FOREACH;
}

bool GFWayPointImp :: ReceivedStartProducingMsg( HoppingUpstreamMsg& message, QueryExit& whichOne ) {
    PDEBUG( "GFWayPointImp :: ReceivedStartProducingMsg()" );

    QueryID qID = whichOne.query;

    // Check to see if we are running this query for the first time or if we
    // are being asked to rerun a query.
    if( qID.Overlaps(queriesCompleted) ) {
        // No need to preprocess, we should already have everything we need
        queriesCompleted.Difference(qID);
        queriesProcessing.Union(qID);
    }
    else {
        queriesToPreprocess.Union(qID);
    }

    return true;
}

bool GFWayPointImp :: PreProcessingPossible( CPUWorkToken& token ) {
    PDEBUG( "GFWayPointImp :: PreProcessingPossible()" );

    if( queriesToPreprocess.IsEmpty() )
        return false;

    QueryIDSet curQueries = queriesToPreprocess;
    // Clear out queriesToPreprocess
    queriesToPreprocess.Difference(curQueries);

    HistoryList lineage;

    QueryExitContainer qExits;

    while( !curQueries.IsEmpty() ) {
        QueryID temp = curQueries.GetFirst();

        FATALIF( !queryIdentityMap.IsThere( temp ), "Told to preprocess a query that I don't know about." );

        QueryExit qe = queryIdentityMap.Find( temp );
        qExits.Append(qe);
    }

    QueryExitContainer whichOnes;
    whichOnes.copy(qExits);

    GFPreProcessWD workDesc( qExits );

    WayPointID myID = GetID();
    WorkFunc myFunc = GetWorkFunction( GFPreProcessWorkFunc :: type );

    myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

    return true;
}

bool GFWayPointImp :: PreProcessingComplete( QueryExitContainer& whichOnes,
        HistoryList& history, ExecEngineData& data ) {
    PDEBUG( "GFWayPointImp :: PreProcessingComplete()" );

    GFPreProcessRez result;
    result.swap(data);

    QueryExitContainer endAtMe;
    GetEndingQueryExits(endAtMe);

    // Any const states that were generated.
    QueryToGLASContMap& rezConstStates = result.get_constStates();

    QueryExitContainer startProcessing;
    QueryIDSet startTerminating;

    FOREACH_TWL( curQuery, whichOnes ) {
        QueryID curID = curQuery.query;

        FATALIF( !statesNeeded.IsThere( curID ), "Don't have a value for the number of states needed "
                "for one of our queries!");
        Swapify<int> tempVal = statesNeeded.Find( curID );
        int curStatesNeeded = tempVal.GetData();

        if( curStatesNeeded == 0 ) {
            QueryExit curQueryCopy = curQuery;
            startProcessing.Append(curQueryCopy);
        }
        else {
            startTerminating.Union(curID);
        }
    } END_FOREACH;

    FOREACH_EM(curQuery, stateCont, rezConstStates) {
        GLAStateContainer& curConstStates = constStates.Find(curQuery);
        curConstStates.MoveToStart();
        int numGen = 0;

        // Prepend the generated states
        FOREACH_TWL(curState, stateCont) {
            curConstStates.Insert(curState);
            curConstStates.Advance();
            ++numGen;
        } END_FOREACH;

        for( ConstStateIndexMap::iterator it = constStateIndex[curQuery].begin();
                it != constStateIndex[curQuery].end(); ++it ) {
            it->second += numGen;
        }
    } END_FOREACH;

    FOREACH_TWL( curQuery, startProcessing ) {
        queriesProcessing.Union(curQuery.query);

        SendStartProducingMsg(curQuery);
    } END_FOREACH;

    // If we have any queries that still require states, send the start producing
    // messages to the terminating query exits.
    FOREACH_TWL(qe, endAtMe) {
        if( startTerminating.Overlaps(qe.query) ) {
            SendStartProducingMsg(qe);
        }
    } END_FOREACH;

    return true;
}

void GFWayPointImp :: GotState( StateContainer& state ) {
    PDEBUG( "GFWayPointImp :: GotState()" );

    // Extract the information from the state container
    QueryExit qe;
    state.get_whichQuery().swap(qe);

    WayPointID source = state.get_source();

    GLAState myState;
    state.get_myState().swap(myState);

    // Get information we have about the query
    FATALIF( !statesNeeded.IsThere( qe.query ), "Got a state container for a"
            "query we don't know about!");
    Swapify<int>& tempStatesNeeded = statesNeeded.Find(qe.query);
    int myStatesNeeded = tempStatesNeeded.GetData();

    FATALIF( myStatesNeeded == 0, "Got a state for a query that doesn't need "
            "any more states!");

    FATALIF( !constStates.IsThere( qe.query ), "Got a state container for a "
            "query we have no const states for!");
    GLAStateContainer& myConstStates = constStates.Find(qe.query);

    myConstStates.MoveToStart();

    int whichIndex = constStateIndex[qe.query][source];

    for( int i = 0; i < whichIndex; ++i ) {
        myConstStates.Advance();
    }

    GLAState& curState = myConstStates.Current();
    myState.swap(curState);

    --myStatesNeeded;
    Swapify<int> tempVal(myStatesNeeded);
    tempStatesNeeded.swap(tempVal);

    if( myStatesNeeded == 0 ) {
        // Got the last state we needed.
        queriesProcessing.Union(qe.query);

        QueryExit myExit = queryIdentityMap.Find(qe.query);
        SendStartProducingMsg(myExit);
    }
}

void GFWayPointImp :: GotChunkToProcess( CPUWorkToken& token,
        QueryExitContainer& whichOnes, ChunkContainer& chunk, HistoryList& history ) {
    PDEBUG( "GFWayPointImp :: GotChunkToProcess()" );

    // Build the work spec
    QueryToGLAStateMap qToFilter;
    FOREACH_TWL( qe, whichOnes ) {
        QueryID qID = qe.query;
        FATALIF(!queryToGFs.IsThere(qID), "Did not find a filter container for a query!");
        GLAStateContainer& cont = queryToGFs.Find(qID);
        if( cont.Length() > 0 ) {
            GLAState state;
            cont.Remove(state);
            qToFilter.Insert(qID, state);
        }
    } END_FOREACH;

    QueryExitContainer whichOnesCopy;
    whichOnesCopy.copy( whichOnes );

    QueryToGLASContMap qToConstState;
    qToConstState.copy(constStates);

    GFProcessChunkWD workDesc( whichOnes, qToFilter, qToConstState, chunk.get_myChunk());

    WorkFunc myFunc = GetWorkFunction( GFProcessChunkWorkFunc::type );

    WayPointID myID = GetID();
    myCPUWorkers.DoSomeWork( myID, history, whichOnesCopy, token, workDesc, myFunc );
}

bool GFWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes,
        HistoryList& history, ExecEngineData& data ) {
    PDEBUG( "GFWayPointImp :: ProcessChunkComplete()" );

    GFProcessChunkRez result;
    result.swap(data);

    QueryToGLAStateMap& filters = result.get_filters();

    FOREACH_EM(key, filter, filters) {
        FATALIF(!queryToGFs.IsThere(key), "Got back filters for a query we don't know about!");
        GLAStateContainer& cont = queryToGFs.Find(key);
        cont.Insert(filter);
    } END_FOREACH;

    Chunk myChunk;
    Chunk& rezChunk = result.get_chunk();
    myChunk.swap(rezChunk);

    // Create the chunk container and swap it with data, so it will be sent
    // to the next waypoint
    ChunkContainer chunkCont(myChunk);
    chunkCont.swap(data);

    return false; // don't send ack
}

bool GFWayPointImp :: ReceivedQueryDoneMsg( QueryExitContainer& whichOnes ) {
    PDEBUG( "GFWayPointImp :: ReceivedQueryDoneMsg()" );

    // Just forward it
    SendQueryDoneMsg( whichOnes );
}

