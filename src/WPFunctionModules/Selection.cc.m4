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
dnl We assume tha this file is included from Modules.m4 and that all
dnl the m4 libraries needed are loaded
dnl

dnl Arguments:
dnl
dnl M4_WPName -- name of the waypoint
dnl
dnl M4_QueryDesc -- list of elements of the form:
dnl     ( Query, FilterName, ( list synthesized ), constructor arguments, <reserved>, Predicate, constants )
dnl listSynthesized: list of (attName, expression)
dnl M4_Attribute_Queries -- the set of queries in which each attribute is used
dnl                elements of the form: ( attribute, serialized_QueryIDSet )
dnl
// module specific headers to allow separate compilation
#include "GLAData.h"
#include "Errors.h"

#ifndef COUNT_TUPLES
//#define COUNT_TUPLES
#endif

extern "C"
int SelectionPreProcessWorkFunc_<//>M4_WPName
(WorkDescription& workDescription, ExecEngineData& result) {
    SelectionPreProcessWD myWork;
    myWork.swap(workDescription);

    QueryExitContainer& queries = myWork.get_whichQueryExits();

    QueryToGLASContMap constStates;

<//>M4_DECLARE_QUERYIDS(</M4_QueryDesc/>, <//>)dnl

    FOREACH_TWL(iter, queries) {
<//>m4_foreach(</_Q_/>, </M4_QueryDesc/>, </dnl
<//>m4_if(SEL_IS_GF(_Q_), 1, </dnl
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
/>)dnl
            QueryID key;

            key = iter.query;
            constStates.Insert( key, myConstStates );
/>, </dnl # this query doesn't need constant states
<//><//><//>/>)dnl
        }
<//><//>/>)dnl
<//>/>)dnl
<//>/>)dnl
    } END_FOREACH;

    SelectionPreProcessRez myRez( constStates );
    myRez.swap(result);

    return -1; // for PreProcess
}

extern "C"
int SelectionProcessChunkWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {
    // go to the work description and get the input chunk
    SelectionProcessChunkWD myWork;
    myWork.swap (workDescription);
    Chunk &input = myWork.get_chunkToProcess ();
    QueryToGLASContMap& constStates = myWork.get_constStates();

<//>M4_DECLARE_QUERYIDS(</M4_QueryDesc/>,</M4_Attribute_Queries/>)dnl

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

<//>M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl

<//>M4_EXTRACT_BITMAP(</input/>)dnl

dnl # create syntesized columns
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </dnl is this a valid query
<//><//><//>M4_CREATE_SYNTHESIZED(_P_)dnl
<//><//>/>, <//>)dnl
<//>/>)dnl

dnl # Create any GFs that are needed.
m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//>m4_ifval( M4_QUERY_NAME(_P_), </dnl
<//><//>m4_if(SEL_IS_GF(_P_), 1, </dnl
<//><//><//>m4_if(G_REQ_CONST_STATE(_P_), 1, </dnl
    // Extracting constant states for query M4_QUERY_NAME(_P_)
    FATALIF(!constStates.IsThere(M4_QUERY_NAME(_P_)), "No constant state container found for query M4_QUERY_NAME(_P_).");
    GLAStateContainer& curStates = constStates.Find(M4_QUERY_NAME(_P_));
    curStates.MoveToStart();

<//><//><//><//>m4_foreach(</_S_/>, m4_quote(G_CONST_STATES(_P_)), </dnl
    const TYPE(_S_)* G_STATE(_P_)</_/>VAR(_S_) = NULL;
    {
        FATALIF(curStates.RightLength() < 1, "Not enough constant states available for query M4_QUERY_NAME(_P_)");
        GLAState& tState = curStates.Current();
        GLAPtr tPtr;
        tPtr.swap(tState);
        G_STATE(_P_)</_/>VAR(_S_) = tPtr.get_glaPtr();
        tPtr.swap(tState);
    }

<//><//><//><//>/>)dnl
<//><//><//>/>)dnl
<//><//><//>m4_if(G_REQ_CONST_STATE(_P_), 1, </dnl
    // Create state for query M4_QUERY_NAME(_P_) using constant state
    G_TYPE(_P_) G_STATE(_P_) </(/> dnl
<//><//><//><//>m4_ifdef_undef(</__FIRST__/>)dnl
<//><//><//><//>m4_foreach(</_S_/>, m4_quote(G_CONST_STATES(_P_)), </dnl
m4_ifndef(</__FIRST__/>, </m4_define(</__FIRST__/>, <//>)/>, </, />)<//>dnl
G_STATE(_P_)</_/>VAR(_S_)<//>dnl
<//><//><//><//>/>)dnl
</);/>
<//><//><//>/>, </dnl
    // Create state for query M4_QUERY_NAME(_P_) using constant arguments
    G_TYPE(_P_) G_STATE(_P_) G_INIT_STATE(_P_)<//>;
<//><//><//>/>)dnl
<//><//>/>)dnl
<//>/>)dnl
/>)dnl

<//>M4_START_OUTPUT_BITMAP<//>dnl

dnl # definition of constants used in expressions
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </dnl is this a valid query
    // constants for query M4_QUERY_NAME(_P_)
<//>_SEL_INITIALIZER(_P_)dnl # the initializer should have a new line
<//><//>/>, <//>)dnl
<//>/>)dnl
	
    FOR_EACH_TUPLE(</input/>){
        QueryIDSet qry;
<//><//>GET_QUERIES(qry)

        // extract values of attributes from streams
<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)

        //selection code for all the predicates
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </dnl is this a valid query
        // do M4_QUERY_NAME(_P_)
<//><//>M4_DECLARE_SYNTHESIZED(_P_)
        if(qry.Overlaps(M4_QUERY_NAME(_P_)) && dnl
!(m4_if(SEL_IS_GF(_P_), 1, </G_STATE(_P_).Filter/>)<//>M4_EVAL_EXPR(_SEL_PREDICATE(_P_)) )){
            qry.Difference(M4_QUERY_NAME(_P_));
        } else { // compute synthesized
<//><//><//>M4_COMPUTE_SYNTHESIZED(_P_)dnl
        }

<//><//>M4_WRITE_SYNTHESIZED(_P_)
<//><//>/>, <//>)dnl
<//>/>)dnl

<//><//>SET_QUERIES(qry)
<//><//>M4_ADVANCE_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>, queriesToRun)
    }

    // finally, if there were any results, put the data back in the chunk
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
dnl # now synthesized
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </dnl is this a valid query
    if (M4_QUERY_NAME(_P_).Overlaps(queriesToRun)){
<//><//>M4_CLOSE_SYNTHESIZED(_P_, </input/>)dnl
    }
<//><//>/>, <//>)dnl
<//>/>)dnl
<//>M4_PUTBACK_OUTBITMAP(</input/>)

    PROFILING2("sel", input.GetNumTuples());
    PROFILING2_FLUSH;


#ifdef COUNT_TUPLES
    cout << "\nCounter ***************** = " << cnt;
#endif
    ChunkContainer tempResult (input);
    tempResult.swap (result);

    return 0; // For Process Chunk
}
