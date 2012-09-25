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

#include "GTWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"
#include "Constants.h"

#include <iostream>

// Note: I have arranged the function definitions here in a different order
// than they are declared in the header. This is because I wanted to arrange
// the definitions in approximately the order the functions should be called.

GTWayPointImp :: GTWayPointImp() {
    PDEBUG( "GTWayPointImp :: GTWayPointImp()" );
    SetResultExitCode( PROCESS_CHUNK );
}

GTWayPointImp :: ~GTWayPointImp() {
    PDEBUG( "GTWayPointImp :: ~GTWayPointImp()" );
}

void GTWayPointImp :: TypeSpecificConfigure( WayPointConfigureData& configData ) {
    PDEBUG( "GTWayPointImp :: TypeSpecificConfigure()" );

    GTConfigureData myConfig;
    myConfig.swap(configData);

    QueryExitContainer queries;
    GetFlowThruQueryExits(queries);

    FOREACH_TWL(iter, queries) {
        QueryID q=iter.query;
        GLAStateContainer cont; // empty container
        if( !queryToGTs.IsThere(q) ) {
            queryToGTs.Insert(q, cont);
        }

        q = iter.query;
        QueryExit temp = iter;

        queryIdentityMap.Insert(q, iter);
    } END_FOREACH;

    QueryToReqStates& reqStates = myConfig.get_reqStates();

    InitConstStates( reqStates );
}

bool GTWayPointImp :: ReceivedStartProducingMsg( HoppingUpstreamMsg& message, QueryExit& whichOne ) {
    PDEBUG( "GTWayPointImp :: ReceivedStartProducingMsg()" );

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

bool GTWayPointImp :: PreProcessingPossible( CPUWorkToken& token ) {
    PDEBUG( "GTWayPointImp :: PreProcessingPossible()" );

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

    GTPreProcessWD workDesc( qExits );

    WayPointID myID = GetID();
    WorkFunc myFunc = GetWorkFunction( GTPreProcessWorkFunc :: type );

    myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

    return true;
}

bool GTWayPointImp :: PreProcessingComplete( QueryExitContainer& whichOnes,
        HistoryList& history, ExecEngineData& data ) {
    PDEBUG( "GTWayPointImp :: PreProcessingComplete()" );

    GTPreProcessRez result;
    result.swap(data);

    QueryExitContainer endAtMe;
    GetEndingQueryExits(endAtMe);

    // Any const states that were generated.
    QueryToGLASContMap& rezConstStates = result.get_constStates();

    QueryExitContainer startProcessing;
    QueryIDSet startTerminating;

    FOREACH_TWL( curQuery, whichOnes ) {
        QueryID curID = curQuery.query;

        int curStatesNeeded = NumStatesNeeded( curID );

        if( curStatesNeeded == 0 ) {
            QueryExit curQueryCopy = curQuery;
            startProcessing.Append(curQueryCopy);
        }
        else {
            startTerminating.Union(curID);
        }
    } END_FOREACH;

    AddGeneratedStates( rezConstStates );

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

void GTWayPointImp :: GotAllStates( QueryID query ) {
    // Got the last state we needed.
    queriesProcessing.Union(query);

    QueryExit myExit = queryIdentityMap.Find(query);
    SendStartProducingMsg(myExit);
}

void GTWayPointImp :: GotChunkToProcess( CPUWorkToken& token,
        QueryExitContainer& whichOnes, ChunkContainer& chunk, HistoryList& history ) {
    PDEBUG( "GTWayPointImp :: GotChunkToProcess()" );

    // Build the work spec
    QueryToGLAStateMap qToFilter;
    FOREACH_TWL( qe, whichOnes ) {
        QueryID qID = qe.query;
        FATALIF(!queryToGTs.IsThere(qID), "Did not find a filter container for a query!");
        GLAStateContainer& cont = queryToGTs.Find(qID);
        if( cont.Length() > 0 ) {
            GLAState state;
            cont.Remove(state);
            qToFilter.Insert(qID, state);
        }
    } END_FOREACH;

    QueryExitContainer whichOnesCopy;
    whichOnesCopy.copy( whichOnes );

    QueryToGLASContMap qToConstState = GetConstStates();

    GTProcessChunkWD workDesc( whichOnes, qToFilter, qToConstState, chunk.get_myChunk());

    WorkFunc myFunc = GetWorkFunction( GTProcessChunkWorkFunc::type );

    WayPointID myID = GetID();
    myCPUWorkers.DoSomeWork( myID, history, whichOnesCopy, token, workDesc, myFunc );
}

bool GTWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes,
        HistoryList& history, ExecEngineData& data ) {
    PDEBUG( "GTWayPointImp :: ProcessChunkComplete()" );

    GTProcessChunkRez result;
    result.swap(data);

    QueryToGLAStateMap& filters = result.get_filters();

    FOREACH_EM(key, filter, filters) {
        FATALIF(!queryToGTs.IsThere(key), "Got back filters for a query we don't know about!");
        GLAStateContainer& cont = queryToGTs.Find(key);
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

bool GTWayPointImp :: ReceivedQueryDoneMsg( QueryExitContainer& whichOnes ) {
    PDEBUG( "GTWayPointImp :: ReceivedQueryDoneMsg()" );

    // Just forward it
    SendQueryDoneMsg( whichOnes );
}

