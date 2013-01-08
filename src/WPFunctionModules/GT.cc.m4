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
// module specific headers to allow separate compilation
#include <iomanip>
#include <assert.h>
#include "Errors.h"

extern "C"
int GTPreProcessWorkFunc_<//>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GTPreProcessWD myWork;
    myWork.swap(workDescription);

    QueryExitContainer& queries = myWork.get_whichQueryExits();

    QueryToGLASContMap constStates;

<//>M4_DECLARE_QUERYIDS(</M4_GTDesc/>, <//>)<//>dnl

    FOREACH_TWL(iter, queries) {
<//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_) , </dnl this is a valid query
        if( iter.query == M4_QUERY_NAME(_Q_) ) {
<//><//><//>m4_if(G_REQ_CONST_STATE(_Q_), 1, </dnl # this query needs constant states
            // Create the container for the constant states.
            GLAStateContainer myConstStates;

            // Generate needed constant states
            myConstStates.MoveToStart();

<//><//><//>m4_foreach(</_S_/>, G_CONST_GENERATED(_Q_), </dnl
            {
                TYPE(_S_) * G_STATE(_Q_)</_/>VAR(_S_) = new TYPE(_S_)<//>G_INIT_STATE(_Q_);

                GLAPtr newPtr(M4_HASH_NAME(TYPE(_S_)), (void *) G_STATE(_Q_)</_/>VAR(_S_) );

                myConstStates.Append(newPtr);
            }
/>)<//>dnl
            QueryID key;

            key = iter.query;
            constStates.Insert( key, myConstStates );
/>, </dnl # this query doesn't need constant states
<//><//><//>/>)<//>dnl
        }
<//><//>/>)<//>dnl
<//>/>)<//>dnl
    } END_FOREACH;

    GTPreProcessRez myRez( constStates );
    myRez.swap(result);

    return WP_PREPROCESSING; // for PreProcess
}

extern "C"
int GTProcessChunkWorkFunc_<//>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    GTProcessChunkWD myWork;
    myWork.swap(workDescription);

    Chunk& input = myWork.get_chunkToProcess();

    QueryToGLAStateMap& filters = myWork.get_filters();
    QueryToGLASContMap& constStates = myWork.get_constStates();

<//>M4_DECLARE_QUERYIDS(</M4_GTDesc/>, </M4_Attribute_Queries/>)<//>dnl

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)<//>dnl

<//>M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>, </input/>)<//>dnl

<//>M4_EXTRACT_BITMAP(</input/>)<//>dnl

    // Set up the output chunk
    Chunk output;

    // Extract the filters
m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
    G_TYPE(_Q_)* G_STATE(_Q_) = NULL;
<//>m4_if(G_REQ_CONST_STATE(_Q_), 1, </dnl
<//><//>m4_foreach(</_S_/>, G_CONST_STATES(_Q_), </dnl
    const TYPE(_S_)* G_STATE(_Q_)</_/>VAR(_S_) = NULL;
<//><//>/>)<//>dnl
<//>/>)<//>dnl

    if( queriesToRun.Overlaps(M4_QUERY_NAME(_Q_)) ) {
<//>m4_if(G_REQ_CONST_STATE(_Q_), 1, </dnl
        if( constStates.IsThere(M4_QUERY_NAME(_Q_)) ) {
            GLAStateContainer& myCont = constStates.Find(M4_QUERY_NAME(_Q_));
            myCont.MoveToStart();

<//><//>m4_foreach(</_S_/>, G_CONST_STATES(_Q_), </dnl
            {
                GLAPtr tState;
                GLAState& state = myCont.Current();
                tState.swap(state);
                G_STATE(_Q_)</_/>VAR(_S_) = (TYPE(_S_)*) tState.get_glaPtr();
                state.swap(tState);

                myCont.Advance();
            }
<//><//>/>)<//>dnl
        }
        else {
            FATAL("Why did we receive no constant states for a GT that requires them?")
        }
<//>/>)<//>dnl

        if( filters.IsThere(M4_QUERY_NAME(_Q_)) ) {
            GLAPtr tState;
            GLAState& state = filters.Find(M4_QUERY_NAME(_Q_));
            tState.swap(state);
            G_STATE(_Q_) = (G_TYPE(_Q_)*) tState.get_glaPtr();
            state.swap(tState);
        }
        else {
<//>m4_if(G_REQ_CONST_STATE(_Q_), 1, </dnl
            // Initialize new GT using constant states
            G_STATE(_Q_) = new G_TYPE(_Q_) </(/> dnl
<//><//>m4_ifdef_undef(</_FIRST_/>)<//>dnl
<//><//>m4_foreach(</_S_/>, G_CONST_STATES(_Q_), </dnl
<//><//><//>m4_ifndef(</_FIRST_/>, </m4_define(</_FIRST_/>, <//>)/>, </, />)<//>dnl
<//><//><//>*G_STATE(_Q_)</_/>VAR(_S_)<//>dnl
<//><//>/>)<//>dnl
<//></ );/>
<//>/>, </dnl
            // Initialize new GT using constant arguments
            G_STATE(_Q_) = new G_TYPE(_Q_)<//>G_INIT_STATE(_Q_);
<//>/>)<//>dnl
            GLAPtr newPtr(M4_HASH_NAME(G_TYPE(_Q_)), (void*) G_STATE(_Q_));
            QueryID qry = M4_QUERY_NAME(_Q_);
            filters.Insert(qry, newPtr);
        }
    }

    // Start columns for all possible outputs
<//>m4_ifdef_undef(</_OUTPUTS_/>M4_QUERY_NAME(_Q_))<//>dnl
<//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>G_OUTPUTS(_Q_))), </dnl
    QueryIDSet _A_<//>_Qrys(M4_QUERY_NAME(_Q_));
    MMappedStorage _A_<//>_Column_store;
    Column  _A_<//>_Column_Ocol(_A_<//>_Column_store);
    M4_COL_TYPE(_A_) _A_<//>_Column_Out(_A_<//>_Column_Ocol);
    M4_ATT_TYPE(_A_) _A_</_Out/>;// containter for value to be written
<//>m4_append(</_OUTPUTS_/>M4_QUERY_NAME(_Q_), _A_</_Out/>, </, />)<//>dnl
<//>/>)<//>dnl
/>)<//>dnl

    // this is the ouput bitstring
    MMappedStorage myStore;
    Column bitmapOut (myStore);
    BStringIterator myOutBStringIter (bitmapOut, queriesToRun);

dnl # definition of constants used in expressions
<//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
    // constants for query M4_QUERY_NAME(_Q_)
<//>_G_INITIALIZER(_Q_)
<//><//>/>)<//>dnl
<//>/>)<//>dnl

    int numTuples = 0;

    FOR_EACH_TUPLE(</input/>) {
        ++numTuples;
        QueryIDSet qry;
<//><//>GET_QUERIES(qry)

        // Access input tuple
<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>, queriesToRun)

<//><//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
        // do M4_QUERY_NAME(_Q_)
        if( queriesToRun.Overlaps(M4_QUERY_NAME(_Q_)) && qry.Overlaps(M4_QUERY_NAME(_Q_)) ) {
m4_case(G_KIND(_Q_), single, </dnl
            if ( G_STATE(_Q_)->ProcessTuple</(/>reval(</m4_args/>G_EXPRESSION(_Q_))<//>dnl
m4_foreach(</_A_/>, m4_quote(m4_defin(</_OUTPUTS_/>M4_QUERY_NAME(_Q_))))
, _A_<//>dnl
</) ) {/>
/>, multi, </dnl
            G_STATE(_Q_)->ProcessTuple G_EXPRESSION(_Q_) ;

            while( G_STATE(_Q_)->GetNextResult( m4_quote(m4_defn(</_OUTPUTS_/>M4_QUERY_NAME(_Q_))) ) ) {
/>)<//>dnl

                // Write the tuple
                myOutBStringIter.Insert( M4_QUERY_NAME(_Q_) );
                myOutBStringIter.Advance();

<//><//><//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
<//><//><//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>G_OUTPUTS(_Q_))), </dnl
                _A_<//>_Column_Out.Insert(_A_</_Out/>);
                _A_<//>_Column_Out.Advance();
<//><//><//><//>/>)<//>dnl
<//><//><//>/>)<//>dnl
            }
        }
<//><//>/>)<//>dnl

<//><//>M4_ADVANCE_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>, queriesToRun)
    }

    // Tell GTs that need to know about the chunk boundary
<//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
<//><//>m4_ifdef(G_TYPE(_Q_)</_CHUNKBOUNDARY/>, </dnl
    if( queriesToRun.Overlaps(M4_QUERY_NAME(_Q_)) ) {
        G_STATE(_Q_)->ChunkBoundary();
    }
<//><//>/>)<//>dnl
<//>/>)<//>dnl

    PROFILING2("GT", numTuples);
    PROFILING2_FLUSH;

    // finally, if there were any results, put the data back in the chunk
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
<//>M4_PUTBACK_INBITMAP(</input/>);

    // Put the output columns into the chunk
    myOutBStringIter.Done();
    output.SwapBitmap(myOutBStringIter);

<//>m4_foreach(</_Q_/>, </M4_GTDesc/>, </dnl
    if( queriesToRun.Overlaps(M4_QUERY_NAME(_Q_)) ) {
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>m4_fifth(_Q_))),</dnl
        Column col_<//>_A_;
        _A_<//>_Column_Out.Done(col_<//>_A_);
        output.SwapColumn (col_<//>_A_, M4_ATT_SLOT(_A_));
<//><//>/>)<//>dnl
    }
<//>/>)<//>dnl

    GTProcessChunkRez gfResult(filters, output);
    result.swap(gfResult);

    return WP_PROCESS_CHUNK;
}
