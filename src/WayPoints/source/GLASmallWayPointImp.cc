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
#include "GLASmallWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"
#include "Constants.h"

#include <iostream>

GLASmallWayPointImp :: GLASmallWayPointImp () {
    PDEBUG ("GLASmallWayPointImp :: GLASmallWayPointImp ()");
    lastFragmentId = 0; // start search from beginning
}

GLASmallWayPointImp :: ~GLASmallWayPointImp () {
    PDEBUG ("GLASmallWayPointImp :: GLASmallWayPointImp ()");
}

void GLASmallWayPointImp :: TypeSpecificConfigure (WayPointConfigureData &configData) {
    PDEBUG ("GLASmallWayPointImp :: TypeSpecificConfigure ()");

    // load up the list where we will place done queries
    GLAConfigureData tempConfig;
    tempConfig.swap (configData);

    // tell people that we are ready to go with our queries... these messages will
    // eventually make it down to the table scan, which will begin producing data
    QueryExitContainer queries;
    GetFlowThruQueryExits(queries);
    FOREACH_TWL(iter, queries){
        // start the pool of states for the new query
        QueryID q=iter.query;
        GLAStateContainer cont; // empty container
        if (!myQueryToGLAStates.IsThere(q)){
            myQueryToGLAStates.Insert(q, cont);
        }
    } END_FOREACH
}

bool GLASmallWayPointImp::FinalizePossible( CPUWorkToken& token ) {
    PDEBUG ("GLASmallWayPointImp :: FinalizePossible()");
    if( !MergeDone() )
        return false;

    int fragmentNo;
    lastFragmentId = queryFragmentMap.FindFirstSet(lastFragmentId);
    if (lastFragmentId == -1){
        lastFragmentId = 0; // start from the beginning next time
        return false; // we did not find any fragment
    } else {
        fragmentNo = lastFragmentId;
    }

    //whichOnes will contains only one query

    Bitstring qryOut = queryFragmentMap.GetBits(fragmentNo);
    queryFragmentMap.Clear(fragmentNo); // queries out

    QueryExitContainer whichOnes;

    // scan mergedQueries and extract the exits that are in qryOut
    FOREACH_TWL(el, mergedQueries){
        QueryExit qe=el;
        if (qe.query.Overlaps(qryOut))
            whichOnes.Insert(qe);
    }END_FOREACH;

    QueryExitContainer whichOnes1;
    whichOnes1.copy (whichOnes);

    //TODO: No need to send map, just send only one states corresponding to whichOnes
    QueryToGLAStateMap tempMergedStates;
    tempMergedStates.copy(mergedStates);

    // this cleans up mergedStates
    GLAFinalizeWD workDesc (fragmentNo, whichOnes1, tempMergedStates);

    // we now create a history list data object...
    GLAHistory hist (GetID (), fragmentNo);
    HistoryList lineage;
    lineage.Insert(hist);

    WayPointID myID = GetID();
    WorkFunc myFunc = GetWorkFunction( GLAFinalizeWorkFunc::type );

    myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

    return true;
}

bool GLASmallWayPointImp::PostProcessingPossible( CPUWorkToken& token ) {
    PDEBUG ("GLASmallWayPointImp :: PostProcessingPossible()");
    if( MergeDone() || queriesToComplete.IsEmpty() )
        return false;

    QueryToGLASContMap stateM;
    FOREACH_TWL(qe, queriesToComplete){
        // find the states for this query
        QueryID q=qe.query;
        QueryID foo;
        FATALIF(!myQueryToGLAStates.IsThere(q), "Why I am having a queryToComplete but no GLA state container?");

        GLAStateContainer& cont = myQueryToGLAStates.Find(q);
        int& mCount = mergeInProgress.Find(qe.query).GetData();

        //schedule merge only when we have 2 or more states in the GLAStateContainer
        // the exception is the case when we only have 1 state from the beginning
        if((cont.Length() > 1) || (cont.Length() == 1 && mCount == 0)) {
            //              cout<<"cont Length before merge schedule: "<<cont.Length()<<endl;
            GLAStateContainer tempCont;
            int mergeCount=0;
            for(cont.MoveToStart(); cont.RightLength();){
                GLAState cs;
                cont.Remove(cs);
                tempCont.Insert(cs);
                mergeCount++;

                //check if merge limit is reached
                if(mergeCount == MERGE_AT_A_TIME){
                    break;
                }
            }
            //cout<<"length of cont in merge for query"<<q.GetStr()<<" = "<<tempCont.Length()<<endl;
            stateM.Insert(q,tempCont);

            //set the mergeInProgress for the query
            mCount++;
            //    cout<<"mCount "<<mCount<<endl;
        }

    }END_FOREACH;

    //check if we have anything to do to make use of the granted token
    stateM.MoveToStart();
    if(!stateM.AtEnd()){
        // stateM has the map from query to states that need to be merged
        QueryExitContainer whichOnes;
        whichOnes.copy (queriesToComplete);

        QueryExitContainer whichOnes1;
        whichOnes1.copy (queriesToComplete);

        //dummy fragmentNo for merge
        GLAHistory hist (GetID (), -1);
        HistoryList lineage;
        lineage.Insert(hist);

        // now, actually get the work done!
        GLAMergeStatesWD workDesc (whichOnes1, stateM);

        WayPointID myID = GetID();
        WorkFunc myFunc = GetWorkFunction( GLAMergeStatesWorkFunc :: type );

        myCPUWorkers.DoSomeWork( myID, lineage, whichOnes, token, workDesc, myFunc );

        return true;
    }
    else{
        return false;
    }

}

bool GLASmallWayPointImp::MergeDone() {
    PDEBUG ("GLASmallWayPointImp :: MergeDone()");
    //check if we have every qid in mergedStates
    FOREACH_EM(qid, cont, myQueryToGLAStates){
      if(!mergedStates.IsThere(qid)){
        return false;
      }
    } END_FOREACH;

    //here, means Merge is done
    return true;
}

void GLASmallWayPointImp :: ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLASmallWayPointImp :: ProcessChunkComplete()");
    // extract the states comming back
    GLAStatesRez temp;
    temp.swap(data);

    FOREACH_EM(key, d, temp.get_glaStates()){
#ifdef DEBUG
        cout<<"\nStateChunk "<<key.GetStr()<<" "<<d.get_glaType()<<endl;
#endif
        FATALIF(!myQueryToGLAStates.IsThere(key), "Did not find the entry ");
        GLAStateContainer& cont = myQueryToGLAStates.Find(key);
        cont.Insert(d);
    }END_FOREACH;
}

void GLASmallWayPointImp :: PostProcessComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLASmallWayPointImp :: PostProcessingComplete()");
    //      cout<<"\n"<<GetID().getName()<<" Merged recvd"<<endl;
    GLAStatesFrRez rez;
    rez.swap(data);

    QueryToGLAStateMap& tempGlaStates = rez.get_glaStates();
    for(tempGlaStates.MoveToStart(); !tempGlaStates.AtEnd();){
        QueryID q = tempGlaStates.CurrentKey();
        int& mCount = mergeInProgress.Find(q).GetData();
        //received one merge, hence decrement the mergedCounter
        mCount--;

        FATALIF(!myQueryToGLAStates.IsThere(q), "Why I am having a returned state but no GLA state container?");
        GLAStateContainer& cont = myQueryToGLAStates.Find(q);
        //            cout<<"cont Length before: "<<cont.Length()<<" mCount: "<<mCount<<endl;

        if(mCount == 0 && cont.Length() == 0){
            //we are done, append this <qid, state> to mergedStates and this query to mergedQueries
            //              cout<<endl<<GetID().getName()<<"Merge done for the query: "<<q.GetStr()<<endl;
            QueryID foo;
            GLAState mystate;
            tempGlaStates.Remove(q, foo, mystate);
            mergedStates.Insert(foo, mystate);

            //TODO: Replace with some QueryID => QueryExit map
            FOREACH_TWL(qe, whichOnes){
                if(qe.query == q){
                    QueryExit qExit = qe;
                    mergedQueries.Insert(qExit);
                    break;
                }
            } END_FOREACH;

        }
        else {
            //remove state from map and append to the GLAStateContainer as we are not done yet
            QueryID foo;
            GLAState mystate;
            tempGlaStates.Remove(q, foo, mystate);
            cont.Append(mystate);
            //            cout<<"cont Length after: "<<cont.Length()<<endl;
        }
    }

    if(MergeDone()){
        FOREACH_EM(qid, frags, rez.get_fragInfo()){
            // mark the new fragments we have to produce
            queryFragmentMap.ORAll(qid, frags);
        }END_FOREACH;

        fragmentsLeft.SuckUp(rez.get_fragInfo());
    }
}

void GLASmallWayPointImp :: FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data ) {
    PDEBUG ("GLASmallWayPointImp :: FinalizeComplete()");
    int frag;

    history.MoveToStart();
    if (history.Current().Type() == GLAHistory::type){
        FATALIF(history.RightLength () != 1, "Why do we have more than the GLA lineage?");
        GLAHistory myHistory;
        myHistory.copy(history.Current());
        frag = myHistory.get_whichFragment();
        //cout<<"Finalized recvd for fragmentNo: "<<frag<<endl;
    }

}

void GLASmallWayPointImp :: GotChunkToProcess ( CPUWorkToken & token, QueryExitContainer& whichOnes, ChunkContainer& chunk, HistoryList& lineage) {
    PDEBUG ("GLASmallWayPointImp :: GotChunkToProcess ()");

    // now build the work spec
    QueryToGLAStateMap qToGLAState;
    FOREACH_TWL(qe, whichOnes){
        QueryID foo = qe.query;
        FATALIF(!myQueryToGLAStates.IsThere(qe.query), "Did not find the entry ");
        GLAStateContainer& cont = myQueryToGLAStates.Find(qe.query);
        if (cont.Length()>0){
            GLAState state;
            cont.Remove(state);
            qToGLAState.Insert(foo, state);
        } // if not, we put no state out, the generated code will create it
    }END_FOREACH;

    QueryExitContainer whichOnesCopy;
    whichOnesCopy.copy( whichOnes );

    GLAProcessChunkWD workDesc (whichOnes, qToGLAState, chunk.get_myChunk());

    WorkFunc myFunc = GetWorkFunction( GLAProcessChunkWorkFunc::type);

    WayPointID myID = GetID();
    myCPUWorkers.DoSomeWork (myID, lineage, whichOnesCopy, token, workDesc, myFunc);
}

bool GLASmallWayPointImp :: ReceivedQueryDoneMsg( QueryExitContainer& whichOnes ) {
    PDEBUG ("GLASmallWayPointImp :: ReceivedQueryDoneMsg()");
    // extract the queries that are done, add them to the list of those to complete
    for (whichOnes.MoveToStart (); whichOnes.RightLength (); ) {
        QueryExit myExit;
        whichOnes.Remove (myExit);
        QueryID qID = myExit.query;
        FATALIF(myExit.query.IsEmpty(), "This should be valid");
        queriesToComplete.Insert (myExit);

        //initialize mergeInProgress for each query and set it to 0
        Swapify<int> val(0);
        mergeInProgress.Insert(qID, val);
    }

    // ask for a worker, if we have not already asked
    if (queriesToComplete.Length()>0)
        return true;
    else
        return false;
}

void GLASmallWayPointImp :: ProcessAckMsg (QueryExitContainer &whichOnes, HistoryList &lineage) {
    PDEBUG ("GLASmallWayPointImp :: ProcessAckMsg ()");
    // make sure that the HistoryList has one item that is of the right type
    lineage.MoveToStart ();
    FATALIF (lineage.RightLength () != 1 || !CHECK_DATA_TYPE(lineage.Current (), GLAHistory),
        "Got a bad lineage item in an ack to a GLA waypoint!");
    GLAHistory myHistory;
    myHistory.swap(lineage.Current());
    int    frag = myHistory.get_whichFragment();

    Bitstring queries; // queries that are dropped
    QueryExitContainer allComplete;
    Bitstring compQ; // completed queries
    FOREACH_TWL(el, whichOnes){
        queries.Union(el.query);
        FATALIF(!fragmentsLeft.IsThere(el.query), "Received an ack for a query I do not know about");
        int& fCount = fragmentsLeft.Find(el.query).GetData();
        fCount--;
        if (fCount == 0){ // done with this query
            compQ.Union(el.query);
            QueryExit qe=el;
            allComplete.Insert(qe);
        }
    }END_FOREACH;


    if (!compQ.IsEmpty()){
        // eliminated completed queries from mergedQueries
        QueryExitContainer newMergedQ;
        // scan mergedQueries and extract the exits that are in qryOut
        FOREACH_TWL(el, mergedQueries){
            QueryExit qe=el;
            if (!qe.query.Overlaps(compQ))
                newMergedQ.Insert(qe); // stays in mergedQueries
        }END_FOREACH;
        mergedQueries.swap(newMergedQ);
    }

    LOG_ENTRY_P(2, "Fragment %d of %s query %s PROCESSED",
                frag, GetID().getName().c_str(), queries.GetStr().c_str());

    // did we finish some queries?
    if (allComplete.Length()>0){
        SendQueryDoneMsg( allComplete );
    }

    // need more tokens to resend the dropped stuff
    GenerateTokenRequests();
}

void GLASmallWayPointImp :: ProcessDropMsg (QueryExitContainer &whichOnes, HistoryList &lineage) {
    PDEBUG ("GLASmallWayPointImp :: ProcessDropMsg ()");

    // make sure that the HistoryList has one item that is of the right type
    lineage.MoveToStart ();
    FATALIF (lineage.RightLength () != 1 || !CHECK_DATA_TYPE (lineage.Current (), GLAHistory),
        "Got a bad lineage item in a drop sent to a GLA waypoint!");

    GLAHistory myHistory;
    myHistory.swap(lineage.Current());
    int    frag = myHistory.get_whichFragment();

    cout<<"Drop for fragment: "<<frag<<endl;
    Bitstring queries; // queries that are dropped
    FOREACH_TWL(el, whichOnes){
        queries.Union(el.query);
    }END_FOREACH;

    queryFragmentMap.OROne(frag, queries);

    LOG_ENTRY_P(2, "Fragment %d of %s query %s DROPPED",
                frag, GetID().getName().c_str(), queries.GetStr().c_str());


    // need more tokens to resend the dropped stuff
    GenerateTokenRequests();
}
