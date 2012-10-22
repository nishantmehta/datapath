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
dnl
dnl Also, the following macros should be defined
dnl M4_WPName(identifier) -- the name of the waypoint we define
dnl M4_AttDropped(list names) -- attributes droped upon completion
dnl M4_QueryDesc(list of tuples: (query, </ (agg_name, agg_type, expression), ../>)
dnl
dnl For each attribute, we assume  that the the following macros are defined
dnl ATT_TYPE_longAttName as the type of the attribute
dnl ATT_SLOT_longAttName as the clot number of the attribute
dnl
dnl For each query, we assume that the following variable is defined
dnl QINDEX_qName as the index associated with the query. The index is used to get the QueryID
dnl

// module specific headers to allow separate compilation
#include <iomanip>
#include <assert.h>
#include "Errors.h"

dnl For each of the queries, we define an object that manipulates the global state
dnl The global state is made of the individual states that make up the aggregaete
dnl for each query.
dnl The object for query Q1 will be names Q1_State
dnl The object has the following methods:
dnl     Default Constructor -- initializes the whole object
dnl     AddItem(double v1, ...) -- add an item to the state. The number of arguments
dnl             is equal to the number of aggregates in the object
dnl      AddState(Q1_State& other) -- adds the state encoded in the other to the object
dnl      ComputeAggregate() -- computes the final aggregate into a vector<double>
dnl The aggregates only support doubles but the result can be converted to INT without loss of precision

extern "C"
int GLAPreProcessWorkFunc_<//>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GLAPreProcessWD myWork;
    myWork.swap(workDescription);

    QueryExitContainer& queries = myWork.get_whichQueryExits();

    QueryToGLASContMap constStates;

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>, <//>)dnl

    FOREACH_TWL(iter, queries) {
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_) , </dnl this is a valid query
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
<//><//><//>m4_if(GLA_REQ_CONST_STATE(_Q_), 1, </dnl # this query needs constant states
            // Create the container for the constant states.
            GLAStateContainer myConstStates;

            // Generate needed constant states
            myConstStates.MoveToStart();

<//><//><//>m4_foreach(</_S_/>, GLA_CONST_GENERATED(_Q_), </dnl
            {
                TYPE(_S_) * GLA_STATE(_Q_)</_/>VAR(_S_) = new TYPE(_S_)<//>GLA_INIT_STATE(_Q_);

                GLAPtr newPtr(M4_HASH_NAME(TYPE(_S_)), (void *) GLA_STATE(_Q_)</_/>VAR(_S_) );

                myConstStates.Append(newPtr);
            }
/>)dnl
            QueryID key;

            key = iter.query;
            constStates.Insert( key, myConstStates );
/>, </dnl # this query doesn't need constant states
<//><//><//>/>)dnl
        }
<//><//>/>)dnl
<//>/>)dnl
    } END_FOREACH;

    GLAPreProcessRez myRez( constStates );
    myRez.swap(result);

    return WP_PREPROCESSING; // for PreProcess
}

extern "C"
int GLAMergeStatesWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    GLAMergeStatesWD myWork;
    myWork.swap(workDescription);

    QueryToGLASContMap& queryGLACont = myWork.get_glaStates();
    QueryExitContainer& queries = myWork.get_whichQueryExits();

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>,<//>)dnl

    QueryToGLAStateMap resultQueryGLASt;
    FOREACH_TWL(iter, queries){
        FATALIF(!queryGLACont.IsThere(iter.query), "Why did we get a query in the list but no stateContainer for it");
        GLAStateContainer& glaContainer = queryGLACont.Find(iter.query);
        GLAPtr mainState;

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
        if (iter.query == M4_QUERY_NAME(_Q_)){

            // extract first element and convert
            FATALIF(mainState.IsValid(), "Why is the state valid? Did more than 1 query fire up?");
            FATALIF(glaContainer.Length()==0, "There should be at least one state in the list");
            glaContainer.Remove(mainState); //grab first state
            GLA_TYPE(_Q_)* mainGLA = (GLA_TYPE(_Q_)*) mainState.get_glaPtr();

            // scan remainig elements, convert and call AddState
            FOREACH_TWL(g, glaContainer){
                GLAPtr localState;
                localState.swap(g);
                GLA_TYPE(_Q_)* localGLA = (GLA_TYPE(_Q_)*) localState.get_glaPtr();
                mainGLA->AddState(*localGLA);
                // localGLA eaten up. delete
                delete localGLA;
            }END_FOREACH;
dnl # result must be in mainState object at the end
        }
<//><//>/>, <//>)dnl
<//>/>)dnl
        resultQueryGLASt.Insert(iter.query, mainState);
    } END_FOREACH;

    GLAStatesRez rez(resultQueryGLASt);
    rez.swap(result);

    return WP_POST_PROCESSING; // for merge
}

extern "C"
int GLAPreFinalizeWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    GLAPreFinalizeWD myWork;
    myWork.swap(workDescription);

    QueryToGLAStateMap& queryGLAStates = myWork.get_glaStates();
    QueryToGLASContMap& queryConstStates = myWork.get_constStates();
    QueryExitContainer& queries = myWork.get_whichQueryExits();

    QueryIDToInt fragments; // fragments for the GLAs that need it
    QueryIDSet iterateMap; // Whether or not each GLA needs to iterate after producing output (if any)

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>,<//>)dnl

    FOREACH_TWL(iter, queries){
        FATALIF(!queryGLAStates.IsThere(iter.query), "Why did we get a query in the list but no stateContainer for it");
        GLAState& curState = queryGLAStates.Find(iter.query);

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
        if (iter.query == M4_QUERY_NAME(_Q_)){
            GLAPtr localState;
            localState.swap(curState);
            GLA_TYPE(_Q_)* localGLA = (GLA_TYPE(_Q_)*) localState.get_glaPtr();
            localState.swap(curState);
dnl # if the GLA has fragments, add them to fragments map
dnl # otherwise insert default number of fragments for every query.
            QueryID foo = M4_QUERY_NAME(_Q_);
            Swapify<int> val(1);

<//><//>m4_if(GLA_KIND(_Q_),fragment,</
            //This is special GLA with fragments enable
            Swapify<int> valFrag(localGLA->GetNumFragments());
            val.swap(valFrag);
<//><//>/>)
            fragments.Insert(foo, val);

<//><//>m4_if(GLA_ITERABLE(_Q_), 1, </dnl
m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
            TYPE(_S_) * GLA_STATE(_Q_)</_/>VAR(_S_) = NULL;
/>)dnl
            if( queryConstStates.IsThere( iter.query ) ) {
                GLAStateContainer & myCont = queryConstStates.Find( iter.query );
                myCont.MoveToStart();
m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
                {
                    GLAState& tempState = myCont.Current();
                    GLAPtr tPtr;
                    tPtr.swap(tempState);
                    GLA_STATE(_Q_)</_/>VAR(_S_) = (TYPE(_S_)*) tPtr.get_glaPtr();
                    tPtr.swap(tempState);
                }
/>)dnl
            } else {
                FATAL("Why did we get no constant states for an iterable query?");
            }

            bool iterateRet = localGLA->ShouldIterate(<//>dnl
m4_ifdef_undef(</_FIRST_/>)dnl
m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
<//>m4_ifndef(</_FIRST_/>, </m4_define(</_FIRST_/>, <//>)/>, </, />)dnl
<//>*GLA_STATE(_Q_)</_/>VAR(_S_)<//>dnl
/>)dnl
);

            if( iterateRet )
                iterateMap.Union(iter.query);
/>)dnl
        }
<//><//>/>, <//>)dnl
<//>/>)dnl
    } END_FOREACH;

    GLAStatesFrRez rez(queryGLAStates, queryConstStates, fragments, iterateMap);
    rez.swap(result);

    return WP_PRE_FINALIZE; // for PreFinalize
}

extern "C"
int GLAFinalizeWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {

    GLAFinalizeWD myWork;
    myWork.swap (workDescription);
    QueryExit whichOne = myWork.get_whichQueryExit();
    GLAState& glaState = myWork.get_glaState();

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>,</M4_Attribute_Queries/>)dnl

    // set up the output chunk
    Chunk output;

    QueryIDSet queriesToRun = whichOne.query;

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
    // do M4_QUERY_NAME(_Q_)
    GLA_TYPE(_Q_)* GLA_STATE(_Q_) = NULL;
    if (whichOne.query == M4_QUERY_NAME(_Q_)){
        // look for the state of M4_QUERY_NAME(_Q_)
        GLAPtr state;
        state.swap(glaState);
        GLA_STATE(_Q_) = (GLA_TYPE(_Q_)*) state.get_glaPtr();
        FATALIF(GLA_STATE(_Q_) == NULL, "Why do not we have a state?");
    }
<//>/>)dnl

    // start columns for all possible outputs
<//>m4_foreach(</_Q_/>,</M4_GLADesc/>,</dnl
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>m4_fifth(_Q_))),</dnl
    QueryIDSet _A_<//>_Qrys(M4_QUERY_NAME(_Q_));
    MMappedStorage _A_<//>_Column_store;
    Column  _A_<//>_Column_Ocol(_A_<//>_Column_store);
    M4_COL_TYPE(_A_) _A_<//>_Column_Out(_A_<//>_Column_Ocol);
    M4_ATT_TYPE(_A_) _A_;// containter for value to be written
<//><//>/>)dnl
<//>/>)dnl

    // this is the ouput bitstring
    MMappedStorage myStore;
    Column bitmapOut (myStore);
    BStringIterator myOutBStringIter (bitmapOut, queriesToRun);

dnl # TODO: extract the states that need to be extracted
dnl # get the queries out of queriesToRun

    // Extract results
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
    if (whichOne.query == M4_QUERY_NAME(_Q_)){
<//><//>m4_if(GLA_OUTPUT_TYPE(_Q_), chunk, </dnl
<//><//>m4_case(GLA_KIND(_Q_),single,</dnl
        {
            // extract tuple
            GLA_STATE(_Q_)->GetResult<//>GLA_OUTPUTS(_Q_);
<//><//>/>,</multi/>,</dnl
        GLA_STATE(_Q_)->Finalize();
        while (    GLA_STATE(_Q_)->GetNextResult(reval(</m4_args/>GLA_OUTPUTS(_Q_))) )    {
<//><//>/>,</fragment/>,</dnl
        int GLA_TYPE(_Q_)<//>_fragment = myWork.get_fragmentNo();
        GLA_TYPE(_Q_)<//>_Iterator* GLA_TYPE(_Q_)<//>_It
             = GLA_STATE(_Q_)->Finalize(GLA_TYPE(_Q_)<//>_fragment);
        while (    GLA_STATE(_Q_)->GetNextResult(GLA_TYPE(_Q_)<//>_It, reval(</m4_args/>GLA_OUTPUTS(_Q_))) )    {
<//><//>/>,</state/>, </dnl
<//><//>dnl we place the result in the first attribute	
    {
m4_if(G_FINALIZE_AS_STATE(_Q_), 1, </dnl
        G_STATE(_Q_)->FinalizeState();
/>)dnl
        reval(</m4_args/>m4_fifth(_Q_)) = STATE((void*)GLA_STATE(_Q_), M4_HASH_NAME(GLA_TYPE(_Q_)));
<//><//>/>,</dnl
        {
            m4_fatal(Do not know how to deal with output type of GLA GLA_TYPE(_Q_));
<//><//>/>)dnl
<//><//>/>, </dnl
        {
        FATAL("Chunk finalize function called for query M4_QUERY_NAME(_Q_), has a result type of GLA_OUTPUT_TYPE(_Q_)");
<//><//>/>)dnl
dnl # write the tuple
            myOutBStringIter.Insert (M4_QUERY_NAME(_Q_));
            myOutBStringIter.Advance ();

<//>m4_foreach(</_Q_/>,</M4_GLADesc/>,</dnl
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>m4_fifth(_Q_))),</dnl
            _A_<//>_Column_Out.Insert (_A_);
            _A_<//>_Column_Out.Advance();
<//><//>/>)dnl
<//>/>)dnl
        }
    }
<//>/>)dnl

    myOutBStringIter.Done();
    output.SwapBitmap(myOutBStringIter);
// write columns
<//>m4_foreach(</_Q_/>,</M4_GLADesc/>,</dnl
    if (whichOne.query == M4_QUERY_NAME(_Q_)){
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>m4_fifth(_Q_))),</dnl
        Column col_<//>_A_;
        _A_<//>_Column_Out.Done(col_<//>_A_);
        output.SwapColumn (col_<//>_A_, M4_ATT_SLOT(_A_));
<//><//>/>)dnl
    }
<//>/>)dnl
    // and get outta here!
    ChunkContainer tempResult (output);
    tempResult.swap (result);
    return WP_FINALIZE; // for finalize
}

extern "C"
int GLAFinalizeStateWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {
    GLAFinalizeWD myWork;
    myWork.swap (workDescription);
    QueryExit& whichOne = myWork.get_whichQueryExit();
    GLAState& glaState = myWork.get_glaState();

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>,</M4_Attribute_Queries/>)dnl

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_if(GLA_FINALIZE_AS_STATE(_Q_), </dnl
    // do M4_QUERY_NAME(_Q_)
    if (whichOne.query == M4_QUERY_NAME(_Q_)){
        // look for the state of M4_QUERY_NAME(_Q_)
        GLAPtr state;
        state.swap(glaState);
        GLA_TYPE(_Q_)* GLA_STATE(_Q_) = (GLA_TYPE(_Q_)*) state.get_glaPtr();
        GLA_STATE(_Q_)->FinalizeState();
        state.swap(glaState);
    }
<//><//>/>, <//>)dnl
<//>/>)dnl

    WayPointID myID = WayPointID::GetIdByName("M4_WPName");
    StateContainer stateCont( myID, whichOne, glaState );
    stateCont.swap(result);
    return WP_FINALIZE; // for finalize
}

extern "C"
int GLAProcessChunkWorkFunc_<//>M4_WPName  (WorkDescription &workDescription, ExecEngineData &result) {

    GLAProcessChunkWD myWork;
    myWork.swap(workDescription);
    Chunk &input = myWork.get_chunkToProcess ();

    QueryToGLAStateMap& glaStates = myWork.get_glaStates();
    QueryToGLASContMap& constStates = myWork.get_constStates();
    QueryToGLAStateMap& garbageStates = myWork.get_garbageStates();

<//>M4_DECLARE_QUERYIDS(</M4_GLADesc/>,</M4_Attribute_Queries/>)dnl

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

<//>M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl

<//>M4_EXTRACT_BITMAP(</input/>)dnl

    // Garbage collect old states, if there are any.
m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
    {
        QueryID curID = M4_QUERY_NAME(_Q_);
        if( garbageStates.IsThere( curID ) ) {
            GLAState& curState = garbageStates.Find( curID );
            GLAPtr curPtr;
            curPtr.swap(curState);

            GLA_TYPE(_Q_)* garbage = (GLA_TYPE(_Q_)*) curPtr.get_glaPtr();
            delete garbage;
        }
    }
/>)dnl

    // Defining the GLA states needed
    // for each one we will look for an existing state.
    // If we find none, we create a state from scratch
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
    GLA_TYPE(_Q_)* GLA_STATE(_Q_) = NULL;
m4_if(GLA_REQ_CONST_STATE(_Q_), 1, </dnl
<//>m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
    const TYPE(_S_) * GLA_STATE(_Q_)</_/>VAR(_S_) = NULL;
<//>/>)dnl
/>)dnl
    if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){ m4_if(GLA_REQ_CONST_STATE(_Q_), 1, </dnl
        if( constStates.IsThere(M4_QUERY_NAME(_Q_)) ) {
            GLAStateContainer& myCont = constStates.Find(M4_QUERY_NAME(_Q_));
            myCont.MoveToStart();
m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
            {
                GLAPtr tState;
                GLAState& state = myCont.Current();;
                tState.swap(state);
                GLA_STATE(_Q_)</_/>VAR(_S_) = (TYPE(_S_)*) tState.get_glaPtr();
                state.swap(tState);

                myCont.Advance();
            }
/>)dnl
        }
        else {
            FATAL("Why did we receive no const states for a GLA that requires them?");
        }
/>)dnl
        if (glaStates.IsThere(M4_QUERY_NAME(_Q_))) {
            GLAPtr tState;
            GLAState& state = glaStates.Find(M4_QUERY_NAME(_Q_));
            tState.swap(state);
            GLA_STATE(_Q_) = (GLA_TYPE(_Q_)*) tState.get_glaPtr();
            state.swap(tState); // put it back in container
        } else {
m4_if(GLA_REQ_CONST_STATE(_Q_), 1, </dnl
            // Const states
            GLA_STATE(_Q_) = new GLA_TYPE(_Q_)</(/>dnl
<//>m4_ifdef_undef(</_FIRST_/>)dnl
<//>m4_foreach(</_S_/>, GLA_CONST_STATES(_Q_), </dnl
<//><//>m4_ifndef(</_FIRST_/>, </m4_define(</_FIRST_/>, <//>)/>, </, />)dnl
<//><//>*GLA_STATE(_Q_)</_/>VAR(_S_)<//>dnl
<//>/>)dnl
<//></);/>
/>, </dnl
            // GLA_INIT_STATE
            GLA_STATE(_Q_) = new GLA_TYPE(_Q_)GLA_INIT_STATE(_Q_);
/>)dnl
            GLAPtr newPtr(M4_HASH_NAME(GLA_TYPE(_Q_)), (void*)GLA_STATE(_Q_));
            QueryIDSet  qry=M4_QUERY_NAME(_Q_);
            glaStates.Insert(qry, newPtr); // put new state in glaStates (returned)
        }
    }
<//>/>)dnl

dnl # definition of constants used in expressions
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
    // constants for query M4_QUERY_NAME(_Q_)
<//>_GLA_INITIALIZER(_Q_)dnl # the initializer should have a new line
<//><//>/>, <//>)dnl
<//>/>)dnl

    int numTuples = 0;

    FOR_EACH_TUPLE(</input/>){
	numTuples++;
        QueryIDSet qry;
<//><//>GET_QUERIES(qry);

<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)
<//><//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
        // do M4_QUERY_NAME(_Q_)
        if (qry.Overlaps(M4_QUERY_NAME(_Q_))){
            GLA_STATE(_Q_)->AddItem<//>GLA_EXPRESSION(_Q_);
        }
<//>/>)dnl

<//><//>M4_ADVANCE_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)
    }

    // tell GLAs that need toknow about the ChunkBoundary
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifdef(GLA_CHUNKBOUNDARY_<//>GLA_TYPE(_Q_),</dnl
    if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
        GLA_STATE(_Q_)->ChunkBoundary();
    }
<//><//>/>)dnl
<//>/>)dnl

	PROFILING2("GLA", numTuples);
	PROFILING2_FLUSH;

    // finally, if there were any results, put the data back in the chunk
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
<//>M4_PUTBACK_INBITMAP(</input/>);

    GLAStatesRez glaResult(glaStates);
    result.swap(glaResult);

    return WP_PROCESS_CHUNK; // for processchunk
}


