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
#include "GLALargeWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"
#include "Constants.h"

#include <iostream>

using namespace std;

GLALargeWayPointImp :: GLALargeWayPointImp () {
    PDEBUG ("GLALargeWayPointImp :: GLALargeWayPointImp ()");
}

GLALargeWayPointImp :: ~GLALargeWayPointImp () {
    PDEBUG ("GLALargeWayPointImp :: GLALargeWayPointImp ()");
}

void GLALargeWayPointImp :: TypeSpecificConfigure (WayPointConfigureData &configData) {
    PDEBUG ("GLALargeWayPointImp :: TypeSpecificConfigure ()");

    // load up the list where we will place done queries
    CONVERT_SWAP(configData, tempConfig, GLALargeConfigureData)

    // tell people that we are ready to go with our queries... these messages will
    // eventually make it down to the table scan, which will begin producing data
    QueryExitContainer queries;
    GetFlowThruQueryExits(queries);

    FOREACH_TWL(iter, queries){
        // start the pool of states for the new query
        QueryID q=iter.query;

        if( !myQueryToGlobalStates.IsThere(q) ) {
            // The number of segments per state is a system-wide constant
            // determined at compile time.
            GlobalGLAState glob( NUM_SEGS );
            myQueryToGlobalStates.Insert( q, glob );
        }
    } END_FOREACH
}

void GLALargeWayPointImp :: GotChunkToProcess( CPUWorkToken& token, QueryExitContainer &whichOnes, ChunkContainer& chunk, HistoryList& lineage ) {
    PDEBUG ("GLALargeWayPointImp :: GotChunkToProcess ()");
    QueryToGlobalGLAPtrMap qToGlobalGLAPtr;
    FOREACH_TWL(qe, whichOnes){
        QueryID foo = qe.query;
        FATALIF(!myQueryToGlobalStates.IsThere(qe.query), "Did not find the entry ");
        GlobalGLAState& cont = myQueryToGlobalStates.Find(qe.query);
        GlobalGLAPtr gPtr( cont );
        qToGlobalGLAPtr.Insert( foo, gPtr );
    }END_FOREACH;

    QueryExitContainer whichOnesCopy;
    whichOnesCopy.copy( whichOnes );

    GLALargeProcessChunkWD workDesc (whichOnes, qToGlobalGLAPtr, chunk.get_myChunk ());

    WorkFunc myFunc = GetWorkFunction( GLALargeProcessChunkWorkFunc::type);

    WayPointID myID = GetID();
    myCPUWorkers.DoSomeWork (myID, lineage, whichOnesCopy, token, workDesc, myFunc);
}

bool GLALargeWayPointImp::FinalizePossible( CPUWorkToken& token ) {
    PDEBUG ("GLALargeWayPointImp :: FinalizePossible()");

    // If we're at the end of the queryFragmentMap already, just move to the
    // beginning. This allows us to have a circular list style of choice of
    // which state to finalize fragments from
    if( queryFragmentMap.AtEnd() )
        queryFragmentMap.MoveToStart();

    while( !queryFragmentMap.AtEnd() ) {
        GLALargeFragmentTracker &fTracker = queryFragmentMap.CurrentData();
        QueryID qID = queryFragmentMap.CurrentKey();

        int seg;
        int frag;

        if( fTracker.getNext( seg, frag ) ) {
            //whichOnes will contain only one query
            QueryExitContainer whichOnes;

            // Get the query exit for the query from queriesToComplete
            FOREACH_TWL(el, queriesToComplete){
                QueryExit qe=el;

                if (qe.query.IsEqual(qID))
                    whichOnes.Insert(qe);
            }END_FOREACH;

            FATALIF( whichOnes.IsEmpty(), "No matching query exit for query ID." );
            // Maybe add a check to make sure there is exactly one query exit?

            QueryExitContainer whichOnes1;
            whichOnes1.copy (whichOnes);

            // Get the correct segment from the global state
            FATALIF( !myQueryToGlobalStates.IsThere( qID ), "No global state defined for this query!" );
            GlobalGLAState & glob = myQueryToGlobalStates.Find( qID );
            GLAState & segment = glob.Peek( seg );
            // The segment should already be a GLAPtr containing a pointer
            // to the actual GLA object.
            GLAState tempSeg;
            tempSeg.copy( segment );

            QueryToGLAStateMap tempStates;
            tempStates.Insert( qID, tempSeg );

            GLAFinalizeWD workDesc (frag, whichOnes1, tempStates);

            // we now create a history list data object...
            GLALargeHistory hist (GetID (), seg, frag);
            HistoryList lineage;
            lineage.Insert(hist);

            // now, actually get the work done!
            WayPointID myID = GetID();
            WorkFunc myFunc = GetWorkFunction( GLALargeFinalizeWorkFunc::type);

            myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

            return true;
        }

        queryFragmentMap.Advance();
    }

    // We didn't find any fragments that could be finalized.
    return false;
}

bool GLALargeWayPointImp :: PreFinalizePossible( CPUWorkToken& token ) {
    PDEBUG ("GLALargeWayPointImp :: PreFinalizePossible()");

    // If we're at the end of queriesToCountFrags already, just move to the
    // beginning. This allows us to have a circular list style of choice of
    // which state to finalize fragments from
    if( queriesToCountFrags.AtEnd() )
        queriesToCountFrags.MoveToStart();

    while( !queriesToCountFrags.AtEnd() ) {
        SegmentIDList &segList = queriesToCountFrags.CurrentData();
        QueryID &curID = queriesToCountFrags.CurrentKey();

        if( !segList.IsEmpty() ) {
            // Remove the next segment to check from the list
            segList.MoveToStart();
            Swapify<int> seg;
            segList.Remove( seg );

            int segID = seg.GetData();
            QueryID qID = curID;

            // whichOnes will contain only one query
            QueryExitContainer whichOnes;

            // Get the query exit for the query from queriesToComplete
            FOREACH_TWL(el, queriesToComplete) {
                QueryExit qe = el;

                if( qe.query.IsEqual(qID) )
                    whichOnes.Insert(qe);
            }END_FOREACH;

            FATALIF( whichOnes.IsEmpty(), "No matching query exit for query ID.");

            QueryExitContainer whichOnes1;
            whichOnes1.copy( whichOnes );

            // Get the correct global state
            FATALIF( !myQueryToGlobalStates.IsThere( qID ), "No global state defined for this query!");
            GlobalGLAState & glob = myQueryToGlobalStates.Find( qID );

            QueryID qIDcopy = qID;

            QueryToGlobalGLAPtrMap qToGlobalGLAPtr;
            GlobalGLAPtr gPtr( glob );
            qToGlobalGLAPtr.Insert( qIDcopy, gPtr );

            QueryIDToInt segToCheck;
            qIDcopy = qID;

            segToCheck.Insert( qIDcopy, seg );

            GLALargeFragmentCountWD workDesc ( whichOnes1, qToGlobalGLAPtr, segToCheck );

            GLALargeHistory hist (GetID (), segID, 0);
            HistoryList lineage;
            lineage.Insert( hist );

            WayPointID myID = GetID();
            WorkFunc myFunc = GetWorkFunction(GLALargeFragmentCountWorkFunc::type);

            myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

            return true;
        }

        queriesToCountFrags.Advance();
    }

    // We didn't find any fragment checks to perform
    return false;
}

bool GLALargeWayPointImp :: PostFinalizePossible( CPUWorkToken& token ) {
    PDEBUG ("GLALargeWayPointImp :: PostFinalizePossible()");

    bool foundSegs = false;

    QueryToSegmentListMap toDealloc;

    FOREACH_EM( qID, fTracker, queryFragmentMap ) {
        vector<int> segsToDealloc;
        if( fTracker.getDeallocatable( segsToDealloc ) ) {
            foundSegs = true;

            SegmentIDList segList;

            for( vector<int>::iterator it = segsToDealloc.begin(); it != segsToDealloc.end(); ++it ) {
                Swapify<int> temp(*it);
                segList.Insert( temp );
            }

            QueryID qIDCopy = qID;

            toDealloc.Insert( qIDCopy, segList );
        }
    }END_FOREACH;

    if( foundSegs ) {
        QueryExitContainer whichOnes;

        FOREACH_TWL(el, queriesToComplete) {
            QueryExit qe = el;
            QueryID tempID = qe.query;
            // Maybe make this IsEqual?
            if (toDealloc.IsThere(tempID))
                whichOnes.Insert(qe);
        }END_FOREACH;

        FATALIF( whichOnes.IsEmpty(), "No matching query exit for query ID." );

        QueryExitContainer whichOnesCopy;
        whichOnesCopy.copy( whichOnes );

        QueryToGLASContMap tempStates;

        FOREACH_EM( curQueryID, curSegmentIDList, toDealloc ) {
            QueryID idCopy;
            idCopy.copy( curQueryID );

            // Get the correct segment from the global state
            FATALIF( !myQueryToGlobalStates.IsThere( curQueryID ), "No global state defined for this query!" );
            GlobalGLAState & glob = myQueryToGlobalStates.Find( curQueryID );

            GLAStateContainer cont;

            FOREACH_TWL( curSegID, curSegmentIDList ) {
                GLAState & segment = glob.Peek( curSegID.GetData() );
                cont.Insert( segment );
            }END_FOREACH;

            tempStates.Insert( idCopy, cont );

        }END_FOREACH;

        GLALargeDeallocateWD workDesc (whichOnes, tempStates );

        GLALargeHistory hist ( GetID(), -1, -1 );
        HistoryList lineage;
        lineage.Insert(hist);

        WayPointID myID = GetID();
        WorkFunc myFunc = GetWorkFunction(GLALargeDeallocateWorkFunc::type);

        myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );
    }

    return foundSegs;
}

void GLALargeWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data) {
    PDEBUG ("GLALargeWayPointImp :: ProcessChunkComplete()");
    // extract the states comming back
    GlobalGLAStatesRez temp;
    temp.swap(data);
}

void GLALargeWayPointImp :: FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLALargeWayPointImp :: FinalizeComplete()");
    int frag;

    history.MoveToStart();
    if (history.Current().Type() == GLALargeHistory::type){
        FATALIF(history.RightLength () != 1, "Why do we have more than the GLA lineage?");
        GLALargeHistory myHistory;
        myHistory.copy(history.Current());
        frag = myHistory.get_whichFragment();
        //cout<<"Finalized recvd for fragmentNo: "<<frag<<endl;
    }
}

void GLALargeWayPointImp :: PostFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLALargeWayPointImp :: PostFinalizeComplete()");
    // May have eliminated some queries from queriesToComplete

    Bitstring compQ; // Completed queries

    // Find out if any queries need to be removed from queriesToComplete
    FOREACH_TWL( el, whichOnes ) {
        QueryExit qe = el;
        QueryID qID = qe.query;

        if( queryFragmentMap.IsThere( qID ) ) {
            GLALargeFragmentTracker & fTracker = queryFragmentMap.Find( qID );

            if( fTracker.allDeallocated() ) { // This state has been completely deallocated
                compQ.Union( qID ); // Need to remove this query

                QueryID tempQ;
                GLALargeFragmentTracker tempT;
                queryFragmentMap.Remove( qID, tempQ, tempT );
            }
        }
    }END_FOREACH;

    if( !compQ.IsEmpty() ) { // there are queries to eliminate
        QueryExitContainer newQueriesToComplete;
        // scan queriesToComplete and remove those that are in whichOnes and have
        // had all of their segments deallocated.
        FOREACH_TWL(el, queriesToComplete){
            QueryExit qe=el;
            if ( !qe.query.Overlaps(compQ) ) {
                newQueriesToComplete.Insert(qe); // stays in queriesToComplete
            }
        }END_FOREACH;
        queriesToComplete.swap(newQueriesToComplete);
    }
}

void GLALargeWayPointImp :: PreFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLALargeWayPointImp :: PreFinalizeComplete()");
    // May have checked all of the segments in a query
    // There is also the possibility that there are no fragments to be
    // generated, and as such a query done message will need to be sent
    // immediately
    QueryExitContainer allComplete;

    FOREACH_TWL( el, whichOnes ) {
        QueryExit qe = el;
        QueryID qID = qe.query;

        FATALIF( !myQueryToGlobalStates.IsThere(qID), "Got a fragment count done for a query we have no global state for.");
        GlobalGLAState &gState = myQueryToGlobalStates.Find(qID);

        if( gState.FragmentCountsValid() && !queryFragmentMap.IsThere(qID) ) {
            if( queriesToCountFrags.IsThere( qID ) ) {
                QueryID putKeyHere;
                SegmentIDList putValueHere;
                queriesToCountFrags.Remove( qID, putKeyHere, putValueHere );

                vector<int> fragCount;
                gState.GetFragmentCount( fragCount );

                GLALargeFragmentTracker fTracker( fragCount );

                if( fTracker.isFinished() )
                    allComplete.Insert(el);

                QueryID qIDcopy = qe.query;

                queryFragmentMap.Insert( qIDcopy, fTracker );
            }
        }
    }END_FOREACH;

    if( allComplete.Length() > 0 )
        SendQueryDoneMsg(allComplete);
}

bool GLALargeWayPointImp :: ReceivedQueryDoneMsg( QueryExitContainer& whichOnes ) {
    PDEBUG ("GLALargeWayPointImp :: ReceivedQueryDoneMsg()");
    // extract the queries that are done, add them to the list of those to complete
    for (whichOnes.MoveToStart (); whichOnes.RightLength (); ) {
        QueryExit myExit;
        whichOnes.Remove (myExit);
        QueryID qID = myExit.query;
        FATALIF(myExit.query.IsEmpty(), "This should be valid");
        queriesToComplete.Insert (myExit);

        // Get the global state pertaining to the query we're going to finish
        // and get the number of fragments to be produced for each segment
        // so that we can set up a fragment tracker for it to keep track
        // of fragments we need to produce
        FATALIF( !myQueryToGlobalStates.IsThere( qID ), "No state for query we got a done message for." );
        GlobalGLAState & glob = myQueryToGlobalStates.Find( qID );

        // Finalize the global state, since we're done aggregating at this point.
        // This ensures we don't try to do any more checkins or checkouts and
        // enables the Peek and GetFragmentCount operations
        glob.Finalize();

        // Now we need to get the fragment counts for all of the segments
        // We have to do this separately because we have to ensure that
        // GetFragmentCount is called only once, as the GLA may do a
        // lot of setup to determine the actual fragment count
        SegmentIDList segList;
        for( int i = 0; i < NUM_SEGS; ++i ) {
            Swapify<int> sID(i);
            segList.Append(sID);
        }

        QueryID qIDcopy = qID;
        queriesToCountFrags.Insert( qIDcopy, segList );
    }

    return true;
}

void GLALargeWayPointImp :: ProcessAckMsg (QueryExitContainer &whichOnes, HistoryList &lineage) {
    PDEBUG ("GLALargeWayPointImp :: ProcessAckMsg ()");
    // make sure that the HistoryList has one item that is of the right type
    EXTRACT_HISTORY_ONLY(lineage, myHistory, GLALargeHistory)

    int frag = myHistory.get_whichFragment();
    int     seg = myHistory.get_whichSegment();

    Bitstring queries; // queries that are acked
    QueryExitContainer allComplete;
    Bitstring compQ; // completed queries
    FOREACH_TWL(el, whichOnes){
        queries.Union(el.query);
        FATALIF(!queryFragmentMap.IsThere(el.query), "Received an ack for a query I do not know about");
        GLALargeFragmentTracker & fTracker = queryFragmentMap.Find( el.query );
        fTracker.fragAcked( seg, frag );
        if (fTracker.isFinished()){ // done with this query
            compQ.Union(el.query);
            QueryExit qe=el;
            allComplete.Insert(qe);
        }
    }END_FOREACH;

    LOG_ENTRY_P(2, "Fragment %d of Segment %d of %s query %s PROCESSED",
            frag, seg, GetID().getName().c_str(), queries.GetStr().c_str());

    // did we finish some queries?
    if (allComplete.Length()>0){
        SendQueryDoneMsg(allComplete);
    }

    // need more tokens to possibly deallocate some segments
    GenerateTokenRequests();
}

void GLALargeWayPointImp :: ProcessDropMsg (QueryExitContainer &whichOnes, HistoryList &lineage) {
    PDEBUG ("GLALargeWayPointImp :: ProcessDropMsg ()");

    // make sure that the HistoryList has one item that is of the right type
    EXTRACT_HISTORY_ONLY(lineage, myHistory, GLALargeHistory);

    int    frag = myHistory.get_whichFragment();
    int     seg = myHistory.get_whichSegment();

    cout<<"Drop for fragment: "<<frag << " of segment: " << seg << endl;

    Bitstring queries; // queries that are dropped
    FOREACH_TWL(el, whichOnes){
        queries.Union(el.query);
        QueryID qID = el.query;
        FATALIF( !queryFragmentMap.IsThere( qID ), "Received a drop message for a query I don't know about" );
        GLALargeFragmentTracker & fTracker = queryFragmentMap.Find( qID );
        fTracker.fragDropped( seg, frag );
    }END_FOREACH;

    LOG_ENTRY_P(2, "Fragment %d of segment %d of %s query %s DROPPED",
            frag, seg, GetID().getName().c_str(), queries.GetStr().c_str());


    // need more tokens to resend the dropped stuff
    GenerateTokenRequests();
}
