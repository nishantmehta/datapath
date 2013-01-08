dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
dnl We assume tha this file is included from AllDefs.m4 and that all
dnl the m4 libraries needed are loaded

dnl # Arguments:
dnl #
dnl # M4_WPName -- name of the waypoint
dnl # M4_GISTDesc -- list of elements in the form:
dnl #       (Query, GISTName, <unused>, constructor arguments, outputs)
dnl #   where outputs is a list of the form (att1, att2, ...)
dnl # Unused values are for compatibility with other generalized processors
dnl # (e.g. GLAs, GTs, GFs)
// module specific headers to allow separate compilation
#include <iomanip>
#include <assert.h>
#include "Errors.h"
#include <vector>
#include <utility>

extern "C"
int GISTPreProcessWorkFunc_<//>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GISTPreProcessWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExitContainer& queries = myWork.get_whichQueryExits();
    QueryToGLASContMap& receivedStates = myWork.get_receivedStates();

    // Results
    QueryToGLASContMap generatedStates;
    QueryToGLAStateMap gists;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>, <//>)<//>dnl

    FOREACH_TWL(iter, queries) {
<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_) , </dnl this is a valid query
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
            QueryID key = iter.query;
<//><//><//>m4_if(G_REQ_CONST_STATE(_Q_), 1, </dnl # this query needs constant states
            // Create the container for the constant states.
            GLAStateContainer myConstStates;

            // Generate needed constant states
            myConstStates.MoveToStart();

<//><//><//>m4_foreach(</_S_/>, G_CONST_GENERATED(_Q_), </dnl
            const TYPE(_S_) * G_STATE(_Q_)</_/>VAR(_S_) = new TYPE(_S_)<//>G_INIT_STATE(_Q_);
            {
                GLAPtr newPtr(M4_HASH_NAME(TYPE(_S_)), (void *) G_STATE(_Q_)</_/>VAR(_S_) );

                myConstStates.Append(newPtr);
            }
/>)<//>dnl

            key = iter.query;
            generatedStates.Insert( key, myConstStates );

            // Extract received states
            FATALIF(!receivedStates.IsThere(M4_QUERY_NAME(_Q_)),
                "Failed to find received states for query M4_QUERY_NAME(_Q_)");
            GLAStateContainer& myReceivedStates = receivedStates.Find(M4_QUERY_NAME(_Q_));
            myReceivedStates.MoveToStart();

            // For error messages
            int recStateIndex = 0;

<//><//><//>m4_foreach(</_S_/>, G_CONST_RECEIVE(_Q_), </dnl
            const TYPE(_S_) * G_STATE(_Q_)</_/>VAR(_S_) = NULL;
            {
                FATALIF(myReceivedStates.AtEnd(),
                    "Didn't receive enough constant states for query M4_QUERY_NAME(_Q_)");

                GLAState& tState = myReceivedStates.Current();
                GLAPtr tPtr;
                tPtr.swap(tState);

                FATALIF(tPtr.get_glaType() != M4_HASH_NAME(TYPE(_S_)),
                    "Query M4_QUERY_NAME(_Q_) received a const state of a different type than expected"
                    " in slot %d, expected type TYPE(_S_).", recStateIndex);

                G_STATE(_Q_)</_/>VAR(_S_) = (TYPE(_S_)*) tPtr.get_glaPtr();

                tPtr.swap(tState);

                myReceivedStates.Advance();
                ++recStateIndex;
            }
<//><//><//>/>)<//>dnl

            // Create the GIST state using const states
            G_TYPE(_Q_) * G_STATE(_Q_) = new G_TYPE(_Q_) ( dnl
m4_ifdef_undef(</_FIRST_/>)<//>dnl
m4_foreach(</_S_/>, G_CONST_STATES(_Q_), </dnl
<//>m4_ifndef(</_FIRST_/>, </m4_define(</_FIRST_/>, <//>)/>, </, />)<//>dnl
<//>*(GLA_STATE(_Q_)</_/>VAR(_S_))<//>dnl
/>)<//>dnl
);
/>, </dnl # this query doesn't need constant states
            // Create the GIST state using constant arguments
            G_TYPE(_Q_) * G_STATE(_Q_) = new G_TYPE(_Q_) G_INIT_STATE(_Q_);
<//><//><//>/>)<//>dnl

            // Package up the GIST state put it in the map
            GLAPtr gistPtr(M4_HASH_NAME(G_TYPE(_Q_)), (void *) G_STATE(_Q_));
            GLAState gistState;
            gistPtr.swap(gistState);
            key = iter.query;

            gists.Insert(key, gistState);
        }
<//><//>/>)<//>dnl
<//>/>)<//>dnl
    } END_FOREACH;

    GISTPreProcessRez myRez(generatedStates, gists);
    myRez.swap(result);

    return WP_PREPROCESSING; // for PreProcess
}

extern "C"
int GISTNewRoundWorkFunc</_/>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GISTNewRoundWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExitContainer& queries = myWork.get_whichQueryExits();
    QueryToGLAStateMap& gists = myWork.get_gists();

    // Outputs
    QueryToGistWUContainer workUnits;
    QueryToGLAStateMap globalSchedulers;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>,<//>)<//>dnl

    FOREACH_TWL(iter, queries) {
        FATALIF(!gists.IsThere(iter.query),
            "Told to start new round for query %s with no GIST state.",
            iter.query.GetStr().c_str());

<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </dnl valid query
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
            // Unpack GIST state
            GLAState& tState = gists.Find(M4_QUERY_NAME(_Q_));
            GLAPtr tPtr;
            tPtr.swap(tState);

            FATALIF(tPtr.get_glaType() != M4_HASH_NAME(G_TYPE(_Q_)),
                "Received GIST of different type than expected for query M4_QUERY_NAME(_Q_)");

            G_TYPE(_Q_)* G_STATE(_Q_) = (G_TYPE(_Q_) *) tPtr.get_glaPtr();

            tPtr.swap(tState);

            // Get the schedulers and GLAs from the GIST
            typedef pair<GIST_LS_TYPE(_Q_)*, GIST_GLA_TYPE(_Q_)*> WorkUnit;
            typedef vector< WorkUnit > WUVector;

            WUVector gistWorkUnits;

            // Second parameter is the parallelization hint.
            G_STATE(_Q_)->PrepareRound( gistWorkUnits, NUM_EXEC_ENGINE_THREADS );

            // Pack the work units into our own data structures.
            GistWUContainer myWorkUnits;

            for( WUVector::iterator it = gistWorkUnits.begin(); it != gistWorkUnits.end(); ++it ) {
                GIST_LS_TYPE(_Q_)* localScheduler = it->first;
                GIST_GLA_TYPE(_Q_)* gla = it->second;

                GLAPtr lsPtr(M4_HASH_NAME(GIST_LS_TYPE(_Q_)), (void*) localScheduler);
                GLAPtr glaPtr(M4_HASH_NAME(GIST_GLA_TYPE(_Q_)), (void*) gla);
                GLAPtr gistPtr(M4_HASH_NAME(G_TYPE(_Q_)), (void*) G_STATE(_Q_));

                GISTWorkUnit workUnit(gistPtr, lsPtr, glaPtr);

                myWorkUnits.Insert(workUnit);
            }

            QueryID key = iter.query;
            workUnits.Insert(key, myWorkUnits);
        }
<//><//>/>)<//>dnl
<//>/>)<//>dnl
    } END_FOREACH;

    GISTNewRoundRez myResult(workUnits);
    myResult.swap(result);

    return WP_PREPARE_ROUND; // New Round
}

extern "C"
int GISTDoStepsWorkFunc</_/>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GISTDoStepsWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExitContainer& queries = myWork.get_whichQueryExits();
    QueryToGistWorkUnit& workUnits = myWork.get_workUnits();

    // Outputs
    QueryToGistWorkUnit unfinishedWork;
    QueryToGLAStateMap finishedWork;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>,<//>)<//>dnl

    FOREACH_TWL(iter, queries) {
        FATALIF(!workUnits.IsThere(iter.query),
            "Did not receive a work unit for query %s.",
            iter.query.GetStr().c_str());

        // Unpack the work
        QueryID key;
        GISTWorkUnit curWork;
        workUnits.Remove(iter.query, key, curWork);
        GLAState& gistState = curWork.get_gist();
        GLAState& lsState = curWork.get_localScheduler();
        GLAState& glaState = curWork.get_gla();

        GLAPtr gistPtr;
        gistPtr.swap(gistState);

        GLAPtr lsPtr;
        lsPtr.swap(lsState);

        GLAPtr glaPtr;
        glaPtr.swap(glaState);

        bool workFinished = false;
m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//>m4_ifval(M4_QUERY_NAME(_Q_), </dnl
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
            // Extract the correct pointer types.
            FATALIF(gistPtr.get_glaType() != M4_HASH_NAME(G_TYPE(_Q_)),
                "Received GIST of incorrect type for query M4_QUERY_NAME(_Q_)");
            G_TYPE(_Q_)* G_STATE(_Q_) = ( G_TYPE(_Q_)* ) gistPtr.get_glaPtr();

            FATALIF(lsPtr.get_glaType() != M4_HASH_NAME(GIST_LS_TYPE(_Q_)),
                "Received local scheduler of incorrect type for query M4_QUERY_NAME(_Q_)");
            GIST_LS_TYPE(_Q_)* localScheduler = (GIST_LS_TYPE(_Q_)*) lsPtr.get_glaPtr();

            FATALIF(glaPtr.get_glaType() != M4_HASH_NAME(GIST_GLA_TYPE(_Q_)),
                "Received GLA of incorrect type for query M4_QUERY_NAME(_Q_)");
            GIST_GLA_TYPE(_Q_)* gla = (GIST_GLA_TYPE(_Q_)*) glaPtr.get_glaPtr();

            // Do the actual work.
            // In the future we may enhance this to allow for timeouts.
            GIST_TASK_TYPE(_Q_) task;
            while( localScheduler->GetNextTask( task ) ) {
                G_STATE(_Q_)->DoStep( task, *gla );
            }

            // Deallocate scheduler
            delete localScheduler;
            workFinished = true;
        }
<//>/>)<//>dnl
/>)<//>dnl

        if( workFinished ) {
            GLAState finishedGLA;
            glaPtr.swap(finishedGLA);

            QueryID key = iter.query;
            finishedWork.Insert(key, finishedGLA);
        }
        else {
            // Need to put the pointers back into the work unit.
            gistPtr.swap(gistState);
            lsPtr.swap(lsState);
            glaPtr.swap(glaState);

            // Insert the work unit into unfinishedWork
            QueryID key = iter.query;
            unfinishedWork.Insert(key, curWork);
        }
    } END_FOREACH;

    GISTDoStepRez myResult(unfinishedWork, finishedWork);
    myResult.swap(result);

    return WP_PROCESSING; // DoStep
}

extern "C"
int GISTMergeStatesWorkFunc</_/>M4_WPName
(WorkDescription& workDescription, ExecEngineData &result) {
    GISTMergeStatesWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryToGLASContMap& toMerge = myWork.get_glaStates();
    QueryExitContainer& queries = myWork.get_whichQueryExits();

    // Outputs
    QueryToGLAStateMap glaStates;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>, <//>)<//>dnl

    FOREACH_TWL(iter, queries) {
        FATALIF(!toMerge.IsThere(iter.query),
            "Did not receive any states to merge for query %s",
            iter.query.GetStr().c_str());

        GLAStateContainer& glaContainer = toMerge.Find(iter.query);

        FATALIF(glaContainer.Length() == 0,
            "Received a container with no states for query %s",
            iter.query.GetStr().c_str())

        glaContainer.MoveToStart();
        GLAState mainState;
        glaContainer.Remove(mainState);
        GLAPtr mainPtr;
        mainPtr.swap(mainState);

<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </dnl
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
            FATALIF(mainPtr.get_glaType() != M4_HASH_NAME(GIST_GLA_TYPE(_Q_)),
                "Received a GLA for query M4_QUERY_NAME(_Q_) of an unexpected type.");

            GIST_GLA_TYPE(_Q_)* mainGLA = (GIST_GLA_TYPE(_Q_)*) mainPtr.get_glaPtr();

            // Scan the remaining elements and merge them with the main one.
            FOREACH_TWL(g, glaContainer) {
                GLAPtr localState;
                localState.swap(g);

                FATALIF(localState.get_glaType() != M4_HASH_NAME(GIST_GLA_TYPE(_Q_)),
                    "Received a GLA for query M4_QUERY_NAME(_Q_) of an unexpected type.");

                GIST_GLA_TYPE(_Q_)* localGLA = (GIST_GLA_TYPE(_Q_)*) localState.get_glaPtr();

                mainGLA->AddState(*localGLA);

                delete localGLA;
            } END_FOREACH;
        }
<//><//>/>)<//>dnl
<//>/>)<//>dnl

        QueryID key = iter.query;
        glaStates.Insert(key, mainPtr);
    } END_FOREACH;

    GLAStatesRez myResult(glaStates);
    myResult.swap(result);

    return WP_POST_PROCESSING; // Merge States
}

extern "C"
int GISTShouldIterateWorkFunc</_/>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    GISTShouldIterateWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExitContainer& queries = myWork.get_whichQueryExits();
    QueryToGLAStateMap& glaStates = myWork.get_glaStates();
    QueryToGLAStateMap& gists = myWork.get_gists();

    // Outputs
    QueryIDToInt fragInfo;
    QueryIDSet queriesToIterate;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>, <//>)<//>dnl

    FOREACH_TWL(iter, queries) {
        FATALIF(!gists.IsThere(iter.query),
            "Did not receive a GIST for query %s",
            iter.query.GetStr().c_str());

        GLAState& curGist = gists.Find(iter.query);

        GLAPtr curGistPtr;
        curGistPtr.swap(curGist);

<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </dnl
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
            FATALIF( curGistPtr.get_glaType() != M4_HASH_NAME(G_TYPE(_Q_)),
                "Received a GIST of an unexpected type for query M4_QUERY_NAME(_Q_)");

            G_TYPE(_Q_)* G_STATE(_Q_) = (G_TYPE(_Q_)*) curGistPtr.get_glaPtr();

            // If a GIST produced no work units for this round, we might not have
            // a GLA to tell us if we should iterate. If we don't, we will assume
            // that there will be no iteration.
            if( glaStates.IsThere(M4_QUERY_NAME(_Q_)) ) {
                GLAState& curState = glaStates.Find(M4_QUERY_NAME(_Q_));
                GLAPtr curStatePtr;
                curStatePtr.swap(curState);

                FATALIF( curStatePtr.get_glaType() != M4_HASH_NAME(GIST_GLA_TYPE(_Q_)),
                    "Received a GLA of an unexpected type for query M4_QUERY_NAME(_Q_)");
                GIST_GLA_TYPE(_Q_)* gla = (GIST_GLA_TYPE(_Q_)*) curStatePtr.get_glaPtr();
                if( gla->ShouldIterate() ) {
                    queriesToIterate.Union(M4_QUERY_NAME(_Q_));
                }

                // GLA is no longer needed, deallocate it.
                delete gla;
            }

            QueryID key = M4_QUERY_NAME(_Q_);
            int numFrags = 1;

<//><//>m4_if(G_KIND(_Q_), fragment, </dnl
            numFrags = G_STATE(_Q_)->GetNumFragments();
<//><//>/>)<//>dnl

            Swapify<int> valFrag(numFrags);
            fragInfo.Insert(key, valFrag);
        }
<//><//>/>)<//>dnl
<//>/>)<//>dnl
    } END_FOREACH;

    GISTShouldIterateRez myResult(fragInfo, queriesToIterate);
    myResult.swap(result);

    return WP_PRE_FINALIZE; // ShouldIterate
}

extern "C"
int GISTProduceResultsWorkFunc</_/>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    GISTProduceResultsWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExit& whichOne = myWork.get_whichOne();
    GLAState& gist = myWork.get_gist();

    int fragmentNo = myWork.get_fragmentNo();

    // Outputs
    Chunk output;

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>)<//>dnl

    QueryIDSet queriesToRun = whichOne.query;

    // Start columns for outputs
<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>m4_fifth(_Q_))), </dnl

    QueryIDSet _A_<//>_Qrys(M4_QUERY_NAME(_Q_));
    MMappedStorage _A_<//>_Column_store;
    Column _A_<//>_Column_Ocol(_A_<//>_Column_store);
    M4_COL_TYPE(_A_) _A_<//>_Column_Out(_A_<//>_Column_Ocol);
    M4_ATT_TYPE(_A_) _A_; // container for value
<//><//>/>)<//>dnl
<//>/>)<//>dnl

    // Output bitstring
    MMappedStorage myStore;
    Column bitmapOut(myStore);
    BStringIterator myOutBStringIter (bitmapOut, queriesToRun);

<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
    if( whichOne.query == M4_QUERY_NAME(_Q_) ) {

        // Extract the GIST state
        GLAPtr gistPtr;
        gistPtr.swap(gist);

        FATALIF(gistPtr.get_glaType() != M4_HASH_NAME(G_TYPE(_Q_)),
            "GIST producing results is of incorrect type for query M4_QUERY_NAME(_Q_)");

        G_TYPE(_Q_)* G_STATE(_Q_) = (G_TYPE(_Q_)*) gistPtr.get_glaPtr();

<//><//>m4_case(G_KIND(_Q_),single, </dnl
        {
            G_STATE(_Q_)->GetResult<//>G_OUTPUTS(_Q_);
/>, multi, </dnl
        G_STATE(_Q_)->Finalize();
        while( G_STATE(_Q_)->GetNextResult G_OUTPUTS(_Q_) ) {
/>, fragment, </dnl
        G_TYPE(_Q_)</_Iterator/>* G_TYPE(_Q_)</_It/>
            = G_STATE(_Q_)->Finalize( fragmentNo );
        while( G_STATE(_Q_)->GetNextResult( G_TYPE(_Q_)</_It/>, reval(</m4_args/>GLA_OUPUTS(_Q_))) ) {
/>, state, </dnl
        {
m4_if(G_FINALIZE_AS_STATE(_Q_), 1, </dnl
            G_STATE(_Q_)->FinalizeState();
/>)<//>dnl
            reval(</m4_args/>m4_fifth(_Q_)) = STATE((void*)G_STATE(_Q_), M4_HASH_NAME(G_TYPE(_Q_)));
/>, </dnl
    m4_fatal(Do not know how to deal with output type of GLA GLA_TYPE(_Q_) [GLA_KIND(_Q_)])
<//><//>/>)<//>dnl
            // Advance the columns
            myOutBStringIter.Insert(M4_QUERY_NAME(_Q_));
            myOutBStringIter.Advance();

<//>m4_foreach(</_Q2_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>m4_fifth(_Q2_))), </dnl

            _A_<//>_Column_Out.Insert(_A_);
            _A_<//>_Column_Out.Advance();
<//><//>/>)<//>dnl
<//>/>)<//>dnl
        }
m4_if( G_KIND(_Q_), fragment, </dnl
        // Delete the iterator;
        delete G_TYPE(_Q_)</_It/>;
/>)<//>dnl
    }
<//>/>)<//>dnl

    myOutBStringIter.Done();
    output.SwapBitmap(myOutBStringIter);

    // Write columns
<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
    if( whichOne.query == M4_QUERY_NAME(_Q_) ) {
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>m4_fifth(_Q_))), </dnl
        Column col_<//>_A_;
        _A_<//>_Column_Out.Done(col_<//>_A_);
        output.SwapColumn(col_<//>_A_, M4_ATT_SLOT(_A_));

<//><//>/>)<//>dnl
    }
<//>/>)<//>dnl

    ChunkContainer tempResult(output);
    tempResult.swap(result);

    return WP_FINALIZE; // ProduceResults
}

extern "C"
int GISTProduceStateWorkFunc</_/>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    GISTProduceResultsWD myWork;
    myWork.swap(workDescription);

    // Inputs
    QueryExit& whichOne = myWork.get_whichOne();
    GLAState& gist = myWork.get_gist();

<//>M4_DECLARE_QUERYIDS(</M4_GISTDesc/>, <//>)<//>dnl

<//>m4_foreach(</_Q_/>, </M4_GISTDesc/>, </dnl
<//><//>m4_if(G_FINALIZE_AS_STATE(_Q_), </dnl
    // do M4_QUERY_NAME(_Q_)
    if (whichOne.query == M4_QUERY_NAME(_Q_)){
        // look for the state of M4_QUERY_NAME(_Q_)
        GLAPtr state;
        state.swap(gist);

        FATALIF(state.get_glaType() != M4_HASH_NAME(G_TYPE(_Q_)),
            "Got GIST of unexpected type when finalizing as state for query M4_QUERY_NAME(_Q_)");

        G_TYPE(_Q_)* G_STATE(_Q_) = (G_TYPE(_Q_)*) state.get_glaPtr();
        G_STATE(_Q_)->FinalizeState();
        state.swap(gist);
    }
<//><//>/>, <//>)<//>dnl
<//>/>)<//>dnl

    WayPointID myID = WayPointID::GetIdByName("M4_WPName");
    StateContainer stateCont( myID, whichOne, gist );
    stateCont.swap(result);

    return WP_FINALIZE; // Produce Results
}
