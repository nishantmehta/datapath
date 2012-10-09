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

#include "SelectionWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"


SelectionWayPointImp :: SelectionWayPointImp () {
    PDEBUG("SelectionWayPointImp :: SelectionWayPointImp ()");
    SetResultExitCode( WP_PROCESS_CHUNK );
}
SelectionWayPointImp :: ~SelectionWayPointImp () {PDEBUG("SelectionWayPointImp :: SelectionWayPointImp ()");}

bool SelectionWayPointImp :: ReceivedStartProducingMsg( HoppingUpstreamMsg& message, QueryExit& whichOne ) {
    PDEBUG( "SelectionWayPointImp :: ReceivedStartProducingMsg ()" );

    QueryID qID = whichOne.query;

    if( qID.Overlaps(queriesProcessing) ) {
        // No need to preprocess, we should already have everything we need.
        // Also forward the message
        SendStartProducingMsg( whichOne );
    }
    else {
        queriesToPreprocess.Union(qID);
    }

    return true;
}

bool SelectionWayPointImp :: PreProcessingPossible( CPUWorkToken& token ) {
    PDEBUG( "SelectionWayPointImp :: PreProcessingPossible()" );

    if( queriesToPreprocess.IsEmpty() )
        return false;

    QueryIDSet curQueries = queriesToPreprocess;
    // Clear out queriesToPreprocess
    queriesToPreprocess.Difference(curQueries);

    HistoryList lineage;

    QueryExitContainer qExits;

    while( !curQueries.IsEmpty() ) {
        QueryID temp = curQueries.GetFirst();

        QueryExit qe = GetExit( temp );
        qExits.Append(qe);
    }

    QueryExitContainer whichOnes;
    whichOnes.copy(qExits);

    SelectionPreProcessWD workDesc( qExits );

    WayPointID myID = GetID();
    WorkFunc myFunc = GetWorkFunction( SelectionPreProcessWorkFunc :: type );

    myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

    return true;
}

bool SelectionWayPointImp :: PreProcessingComplete( QueryExitContainer& whichOnes,
        HistoryList& history, ExecEngineData& data ) {
    PDEBUG( "SelectionWayPointImp :: PreProcessingComplete()" );

    SelectionPreProcessRez result;
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

void SelectionWayPointImp :: GotAllStates( QueryID query ) {
    PDEBUG( "SelectionWayPointImp :: GotAllStates ()");

    // Got the last state we needed.
    queriesProcessing.Union(query);

    QueryExit myExit = GetExit( query );
    SendStartProducingMsg(myExit);
}

void SelectionWayPointImp :: GotChunkToProcess( CPUWorkToken& token,
        QueryExitContainer& whichOnes, ChunkContainer& chunk, HistoryList& history ) {
    PDEBUG( "SelectionWayPointImp :: GotChunkToProcess()" );

    ChunkID chunkID;

    // if we have a chunk produced by a table waypoint log it
    CHECK_FROM_TABLE_AND_LOG_WITH_ID(history, Selection, chunkID);

    // create the work spec and get it done!
    QueryExitContainer myDestinations;
    myDestinations.copy (whichOnes);
    QueryToGLASContMap constStates = GetConstStates();

    SelectionProcessChunkWD workDesc (chunkID, myDestinations, chunk.get_myChunk(), constStates);

    // and send off the work request
    WayPointID myID;
    myID = GetID ();
    WorkFunc myFunc;
    myFunc = GetWorkFunction (SelectionProcessChunkWorkFunc::type);
    myCPUWorkers.DoSomeWork (myID, history, whichOnes, token, workDesc, myFunc);
}

