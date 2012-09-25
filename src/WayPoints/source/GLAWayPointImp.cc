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
#include "GLAWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"

GLAWayPointImp :: GLAWayPointImp () {
    PDEBUG ("GLAWayPointImp :: GLAWayPointImp ()");
    SetTokensRequested( CPUWorkToken::type, NUM_EXEC_ENGINE_THREADS / 2 );
}

GLAWayPointImp :: ~GLAWayPointImp () {
    PDEBUG ("GLAWayPointImp :: GLAWayPointImp ()");
}

void GLAWayPointImp :: InitConstStates( QueryToReqStates& reqStates ) {
    PDEBUG ("GLAWayPointImp :: InitConstStates ()");
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

int GLAWayPointImp :: NumStatesNeeded( QueryID qID ) {
    PDEBUG ("GLAWayPointImp :: NumStatesNeeded ()");
    FATALIF( !statesNeeded.IsThere( qID ), "NumStatesNeeded: Don't know about query %s.", qID.GetStr().c_str());
    return statesNeeded.Find( qID ).GetData();
}

void GLAWayPointImp :: AddGeneratedStates( QueryToGLASContMap& genStates ) {
    PDEBUG ("GLAWayPointImp :: AddGeneratedStates ()");

    FOREACH_EM(curQuery, stateCont, genStates) {
        FATALIF( !constStates.IsThere( curQuery ),
                "Unable to add generated states for unknown query %s.", curQuery.GetStr().c_str());
        GLAStateContainer& curConstStates = constStates.Find(curQuery);
        curConstStates.MoveToStart();
        int numGen = 0;

        // Prepend the generated states
        FOREACH_TWL(curState, stateCont) {
            curConstStates.Insert(curState);
            curConstStates.Advance();
            ++numGen;
        } END_FOREACH;

        for( map<WayPointID,int>::iterator it = constStateIndex[curQuery].begin();
                it != constStateIndex[curQuery].end(); ++it ) {
            it->second += numGen;
        }
    } END_FOREACH;
}

QueryToGLASContMap GLAWayPointImp :: GetConstStates() {
    PDEBUG ("GLAWayPointImp :: GetConstStates()");
    QueryToGLASContMap constStateCopy;
    constStateCopy.copy( constStates );
    return constStateCopy;
}

void GLAWayPointImp :: RemoveQueryData( QueryIDSet toEject ) {
    PDEBUG ("GLAWayPointImp :: RemoveQueryData()");
    QueryIDSet temp = toEject;
    while( !temp.IsEmpty() ) {
        QueryID curID = temp.GetFirst();

        QueryID key;
        GLAStateContainer stateVal;
        if( constStates.IsThere( curID ) )
            constStates.Remove( curID, key, stateVal );

        Swapify<int> intVal;
        if( statesNeeded.IsThere( curID ) )
            statesNeeded.Remove( curID, key, intVal );

        constStateIndex.erase( curID );
    }
}

bool GLAWayPointImp :: PreProcessingPossible( CPUWorkToken& token ) {
    return false;
}

bool GLAWayPointImp :: PostProcessingPossible( CPUWorkToken& token ) {
    return false;
}

bool GLAWayPointImp :: PreFinalizePossible( CPUWorkToken& token ) {
    return false;
}

bool GLAWayPointImp :: FinalizePossible( CPUWorkToken& token ) {
    return false;
}

bool GLAWayPointImp :: PostFinalizePossible( CPUWorkToken& token ) {
    return false;
}

bool GLAWayPointImp :: PreProcessingComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

bool GLAWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

bool GLAWayPointImp :: PostProcessComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

bool GLAWayPointImp :: PreFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

bool GLAWayPointImp :: FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

bool GLAWayPointImp :: PostFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    return false;
}

void GLAWayPointImp :: DoneProducing (QueryExitContainer &whichOnes, HistoryList &history, int result, ExecEngineData& data) {
    PDEBUG ("GLAWayPointImp :: DoneProducing ()");
    // we do not touch data

    // Notify that a token request was successful if we did anything other than
    // process a chunk.
    if (result != PROCESS_CHUNK ){
        TokenRequestCompleted( CPUWorkToken::type );
    }

    // Pre-Processing complete
    if( result == PREPROCESSING ) {
        PreProcessingComplete( whichOnes, history, data );
    }

    // Chunk processing function complete
    if (result == PROCESS_CHUNK) {
        bool res = ProcessChunkComplete( whichOnes, history, data );
        if (res)
            SendAckMsg (whichOnes, history);
    }

    // Post Processing function complete
    else if (result == POST_PROCESSING){
        bool res = PostProcessComplete( whichOnes, history, data );
        if (res)
            GenerateTokenRequests();
    }

    // Pre Finalize function complete
    else if (result == PRE_FINALIZE) {
        bool res = PreFinalizeComplete( whichOnes, history, data );
        if (res)
            GenerateTokenRequests();
    }

    // Finalize function complete
    else if (result == FINALIZE ) {
        bool res = FinalizeComplete( whichOnes, history, data );
        if (res)
            GenerateTokenRequests();
    }

    // Post Finalize function complete
    else if (result == POST_FINALIZE ) {
        PostFinalizeComplete( whichOnes, history, data );
    }

    if (result != resultExitCode){ // not finalize, kill the output since nothing gets to the top
        // zero-out data so the EE does not send it above
        ExecEngineData dummy;
        data.swap(dummy);
    }
}

void GLAWayPointImp :: RequestGranted (GenericWorkToken &returnVal) {
    PDEBUG ("GLAWayPointImp :: RequestGranted ()");

    // we know that the reason that this request is granted is that we have one or more
    // query exits that we are ready to finish up... first, cast the return val appropriately
    CPUWorkToken myToken;
    myToken.swap (returnVal);

    if( PostFinalizePossible( myToken ) ) {
        return;
    }
    else if( FinalizePossible( myToken ) ) {
        return;
    }
    else if( PreFinalizePossible( myToken ) ) {
        return;
    }
    else if( PostProcessingPossible( myToken ) ) {
        return;
    }
    else if( PreProcessingPossible( myToken ) ) {
        return;
    }
    else {// nothing to do
        TokenRequestCompleted( CPUWorkToken::type );
        GiveBackToken(myToken);
    }
}

void GLAWayPointImp :: ProcessHoppingDataMsg (HoppingDataMsg &data) {
    PDEBUG ("GLAWayPointImp :: ProcessHoppingDataMsg ()");

    if( CHECK_DATA_TYPE(data.get_data(), ChunkContainer) ) {
        // in this case, the first thing we do is to request a work token
        GenericWorkToken returnVal;
        if (!RequestTokenImmediate (CPUWorkToken::type, returnVal)) {

            // if we do not get one, then we will just return a drop message to the sender
            SendDropMsg (data.get_dest (), data.get_lineage ());
            return;
        }

        // convert the token into the correct type
        CPUWorkToken myToken;
        myToken.swap (returnVal);

        // OK, got a token!  So first thing is to extract the chunk from the message
        ChunkContainer temp;
        data.get_data ().swap (temp);

        // at this point, we are ready to create the work spec.  First we figure out what queries to finish up
        QueryExitContainer whichOnes;
        whichOnes.copy (data.get_dest ());

        // if we have a chunk produced by a table waypoint log it
        CHECK_FROM_TABLE_AND_LOG( data.get_lineage(), GLAWayPoint );

        GotChunkToProcess( myToken, whichOnes, temp, data.get_lineage() );
    }
    else if( CHECK_DATA_TYPE(data.get_data(), StateContainer) ) {
        StateContainer temp;
        data.get_data().swap( temp );

        GotState( temp );
    }
    else {
        FATAL("GLAWaypoint got a hopping data message containing a type of data it "
                "didn't expect!");
    }
}

// the only kind of message we are interested in is a query done message... everything else is
// just forwarded on, down the graph
void GLAWayPointImp:: ProcessHoppingDownstreamMsg (HoppingDownstreamMsg &message) {
    PDEBUG ("GLAWayPointImp :: ProcessHoppingDownstreamMsg ()");

    bool retVal = false;

    // see if we have a query done message
    if (CHECK_DATA_TYPE (message.get_msg (), QueryDoneMsg)) {

        // do the cast via a swap
        QueryDoneMsg temp;
        temp.swap (message.get_msg ());

        retVal = ReceivedQueryDoneMsg( temp.get_whichOnes() );
    }
    else {
        SendHoppingDownstreamMsg (message);
    }

    if( retVal ) {
        GenerateTokenRequests();
    }
}

void GLAWayPointImp :: ProcessHoppingUpstreamMsg( HoppingUpstreamMsg& message) {
    PDEBUG("GLAWayPointImp :: ProessHoppingUpstreamMsg()");
    bool retVal = false;

    if( CHECK_DATA_TYPE( message.get_msg(), StartProducingMsg) ) {
        StartProducingMsg temp;
        temp.swap( message.get_msg() );
        QueryExit whichOne = temp.get_whichOne();
        temp.swap( message.get_msg() );

        retVal = ReceivedStartProducingMsg( message, whichOne );
    }
    else {
        SendHoppingUpstreamMsg( message );
    }

    if( retVal ) {
        GenerateTokenRequests();
    }
}

bool GLAWayPointImp :: ReceivedStartProducingMsg(HoppingUpstreamMsg& message, QueryExit& whichOne ) {
    // Default behaior: just forward it.
    SendHoppingUpstreamMsg(message);
    return false;
}

void GLAWayPointImp :: GotState( StateContainer& state ) {
    // Extract information from the state container.
    QueryExit qe;
    state.get_whichQuery().swap(qe);

    WayPointID source = state.get_source();

    GLAState myState;
    state.get_myState().swap(myState);

    // Get information we have about the query.
    FATALIF( !statesNeeded.IsThere( qe.query ), "Got a state container for a query we don't know about!");
    Swapify<int>& tempStatesNeeded = statesNeeded.Find(qe.query);
    int myStatesNeeded = tempStatesNeeded.GetData();

    FATALIF( myStatesNeeded == 0, "Got a state for a query that doesn't need any more states!" );

    FATALIF( !constStates.IsThere( qe.query ), "Got a state container for a query we have no const states for!");
    GLAStateContainer& myConstStates = constStates.Find(qe.query);

    myConstStates.MoveToStart();

    int whichIndex = constStateIndex[qe.query][source];

    for( int i = 0; i < whichIndex; ++i ) {
        myConstStates.Advance();
    }

    GLAState& curState = myConstStates.Current();
    myState.swap(curState);

    // Update the number of states needed
    --myStatesNeeded;
    Swapify<int> tempVal(myStatesNeeded);
    tempStatesNeeded.swap(tempVal);

    if( myStatesNeeded == 0 ) {
        GotAllStates( qe.query );
    }
}

void GLAWayPointImp :: SetResultExitCode( ExitCode exitCode ) {
    resultExitCode = exitCode;
}
