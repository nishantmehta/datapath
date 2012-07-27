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

void GLAWayPointImp :: PreProcessingComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: PostProcessComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: PreFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: PostFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    // Do nothing
}

void GLAWayPointImp :: DoneProducing (QueryExitContainer &whichOnes, HistoryList &history, int result, ExecEngineData& data) {
    PDEBUG ("GLAWayPointImp :: DoneProducing ()");
    // we do not touch data

    // Notify that a token request was successful if we did anything other than
    // process a chunk.
    if (result != 0 ){
        TokenRequestCompleted( CPUWorkToken::type );
    }

    // Pre-Processing complete
    if( result == -1 ) {
        PreProcessingComplete( whichOnes, history, data );
    }

    // Chunk processing function complete
    if (result == 0) {
        ProcessChunkComplete( whichOnes, history, data );
        SendAckMsg (whichOnes, history);
    }

    // Post Processing function complete
    else if (result == 1){
        PostProcessComplete( whichOnes, history, data );
        GenerateTokenRequests();
    }

    // Pre Finalize function complete
    else if (result == 2) {
        PreFinalizeComplete( whichOnes, history, data );
        GenerateTokenRequests();
    }

    // Finalize function complete
    else if (result == 3 ) {
        FinalizeComplete( whichOnes, history, data );
        GenerateTokenRequests();
    }

    // Post Finalize function complete
    else if (result == 4 ) {
        PostFinalizeComplete( whichOnes, history, data );
    }

    if (result!=3){ // not finalize, kill the output since nothing gets to the top
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
    FATAL("Don't know what to do with this!");
}
