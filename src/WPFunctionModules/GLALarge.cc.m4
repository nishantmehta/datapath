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
#include <vector>
#include "Constants.h"
#include "HashFunctions.h"

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
int GLALargeProcessChunkWorkFunc_<//>M4_WPName  (WorkDescription &workDescription, ExecEngineData &result) {
    GLALargeProcessChunkWD myWork;
    myWork.swap(workDescription);
    Chunk &input = myWork.get_chunkToProcess ();

  QueryToGlobalGLAPtrMap& glaStates = myWork.get_glaStates();

  M4_DECLARE_QUERYIDS(</M4_GLADesc/>,</M4_Attribute_Queries/>)dnl

    M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

  M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl

    M4_EXTRACT_BITMAP(</input/>)dnl


  // Defining the GLA states needed
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
  GlobalGLAState * GLA_STATE(_Q_) = NULL;

  if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
    if (glaStates.IsThere(M4_QUERY_NAME(_Q_))) {
       GlobalGLAPtr& tState = glaStates.Find(M4_QUERY_NAME(_Q_));
       GLA_STATE(_Q_) = tState.get_glaPtr();
    } else {
       FATAL("GlobalGLAState not found for a query.")
    }

  }

  dnl # Create a vector of gla states representing the local segments
  dnl # Lools like this:
  dnl # vector<gla_type> state_name_local( numSegments, gla_type( constructor arguments ) );
  dnl # Note: the number of segments is defined by the system-wide constant NUM_SEGS
  vector<GLA_TYPE(_Q_)> GLA_STATE(_Q_)<//>_local( NUM_SEGS , GLA_TYPE(_Q_)<//>GLA_INIT_STATE(_Q_) );
<//>/>)dnl

dnl # definition of constants used in expressions
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
    // constants for query M4_QUERY_NAME(_Q_)
        _GLA_INITIALIZER(_Q_)dnl # the initializer should have a new line
<//><//>/>, <//>)dnl
<//>/>)dnl

    FOR_EACH_TUPLE(</input/>){
        QueryIDSet qry;
        GET_QUERIES(qry);

<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
        // do M4_QUERY_NAME(_Q_)
        if (qry.Overlaps(M4_QUERY_NAME(_Q_))){
            uint64_t seg;
            seg  = GLA_TYPE(_Q_) :: Map<//>GLA_EXPRESSION(_Q_);
            // Hash it again just to ensure randomness while still ensuring
            // everything with the same map value goes to the same GLA
            seg = Hash( seg );
            seg = seg % NUM_SEGS;

            GLA_STATE(_Q_)<//>_local[seg].AddItem<//>GLA_EXPRESSION(_Q_);
        }
<//>/>)dnl

<//><//>M4_ADVANCE_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)
    }

  // Now merge the local segments into the global state.
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
  int GLA_STATE(_Q_)<//>_numSeg = NUM_SEGS;

  dnl # An array of integers, where a 1 represents a local segment that still
  dnl # needs to be merged into the global state and a 0 represents a segment
  dnl # that has already been merged.
  dnl #
  dnl # (Note: may be more efficient to use a vector of bools instead.)
  int GLA_STATE(_Q_)<//>_theseAreOk[GLA_STATE(_Q_)<//>_numSeg];
  for( int i = 0; i < GLA_STATE(_Q_)<//>_numSeg; ++i ) GLA_STATE(_Q_)<//>_theseAreOk[i] = 1;

  dnl # An integer representing the number of segments left to merge.
  int GLA_STATE(_Q_)<//>_segsLeft = GLA_STATE(_Q_)<//>_numSeg;

  dnl # While there are still segments to be merged, check out a random
  dnl # segment from the global state.
  while( GLA_STATE(_Q_)<//>_segsLeft > 0 ) {
    GLAState checkedOut;
    int whichOne = GLA_STATE(_Q_)->CheckOutOne( GLA_STATE(_Q_)<//>_theseAreOk, checkedOut );

    dnl # Check to see if the state is valid. If it is valid, merge the local
    dnl # state into the global segment, and then check back in the global segment.
    if( checkedOut.IsValid() ) {
      // Extract pointer from container
      GLAPtr tState;
      tState.swap( checkedOut );
      GLA_TYPE(_Q_)* statePtr = (GLA_TYPE(_Q_)*) tState.get_glaPtr();

      statePtr->AddState( GLA_STATE(_Q_)<//>_local[whichOne] );

      // Put it back into the container
      checkedOut.swap(tState);
      GLA_STATE(_Q_)->CheckIn( whichOne, checkedOut );
    }
    dnl # If the global segment is not valid, it has not yet been initialized.
    dnl # Simply check in the local segment instead.
    else {
      // Create persistent storage space for segment
      GLA_TYPE(_Q_)* newState = new GLA_TYPE(_Q_)<//>();
      newState->AddState( GLA_STATE(_Q_)<//>_local[whichOne] );

      // Create a new pointer to check
      GLAPtr newPtr(0, (void*) newState );
      checkedOut.swap( newPtr );
      GLA_STATE(_Q_)->CheckIn( whichOne, checkedOut );
    }

    dnl # Mark the segment we just merged as having been merged in the array and
    dnl # decrement the number of segments left to merge.
    GLA_STATE(_Q_)<//>_theseAreOk[whichOne] = 0;
    --GLA_STATE(_Q_)<//>_segsLeft;
  }
<//>/>)

dnl # Need to change this, possibly remove it
dnl # It doesn't really make much sense to tell GLAs about the chunk boundary
dnl # when we are splitting up the tuples in the chunk between them.

dnl #   // tell GLAs that need toknow about the ChunkBoundary
dnl # <//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
dnl # <//><//>m4_ifdef(GLA_CHUNKBOUNDARY_<//>GLA_TYPE(_Q_),</dnl
dnl #     if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
dnl #             GLA_STATE(_Q_)->ChunkBoundary();
dnl #     }
dnl # <//><//>/>)dnl
dnl # <//>/>)dnl

  // finally, if there were any results, put the data back in the chunk
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
<//>M4_PUTBACK_INBITMAP(</input/>);


  GlobalGLAStatesRez glaResult(glaStates);
  result.swap(glaResult);

  return 0; // for processchunk
}

extern "C"
int GLALargeFragmentCountWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
  //printf("Begin GLALargeFragmentCountWorkFunc\n");

  GLALargeFragmentCountWD myWork;
  myWork.swap(workDescription);

  QueryExitContainer& queries = myWork.get_whichQueryExits();

  QueryIDToInt& segmentIDs = myWork.get_segmentIDs();

  QueryToGlobalGLAPtrMap& glaStates = myWork.get_glaStates();

  M4_DECLARE_QUERYIDS(</M4_GLADesc/>,<//>)dnl

  FOREACH_TWL(iter, queries) {
    FATALIF(!glaStates.IsThere(iter.query), "Why did we get a query in the list but no GlobalStatePtr for it?");
    GlobalGLAPtr & tState = glaStates.Find(iter.query);
    GlobalGLAState * globalState = tState.get_glaPtr();

    FATALIF(!segmentIDs.IsThere(iter.query), "Why did we get a query in the list but no segment ID for it?");
    Swapify<int> & swapSegID = segmentIDs.Find(iter.query);
    int segID = swapSegID.GetData();

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl # this is a valid query
    if( iter.query == M4_QUERY_NAME(_Q_)) {
        GLAState & g = globalState->Peek(segID);
        GLAPtr localState;
    localState.swap(g);
    GLA_TYPE(_Q_)* localGLA = (GLA_TYPE(_Q_)*) localState.get_glaPtr();
    int numFrags = 1;

<//><//>m4_if(GLA_KIND(_Q_),fragment,</dnl
    // Set the number of fragments
    numFrags = localGLA->GetNumFragments();/>)

    globalState->SetFragmentCount( segID, numFrags );

    // Put the segment back into the global state
    localState.swap(g);
    }
<//><//>/>, <//>)dnl
<//>/>)dnl

  }END_FOREACH;

  //printf("End GLALargeFragmentCountWorkFunc\n");

  return 2; // for fragment count
}

extern "C"
int GLALargeFinalizeWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
    //printf("Begin GLALargeFinalizeWorkFunc\n");

    GLAFinalizeWD myWork;
    myWork.swap (workDescription);
    QueryToGLAStateMap& queryGLAMap = myWork.get_glaStates();

  M4_DECLARE_QUERYIDS(</M4_GLADesc/>,</M4_Attribute_Queries/>)dnl

    // set up the output chunk
    Chunk output;

    M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
  // do M4_QUERY_NAME(_Q_)
    GLA_TYPE(_Q_)* GLA_STATE(_Q_) = NULL;
  if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
        // look for the state of M4_QUERY_NAME(_Q_)
        GLAPtr state;
        GLAState& stateB = queryGLAMap.Find(M4_QUERY_NAME(_Q_));
        state.swap(stateB);
        GLA_STATE(_Q_) = (GLA_TYPE(_Q_)*) state.get_glaPtr();
        FATALIF(GLA_STATE(_Q_) == NULL, "Why do not we have a state?");
        state.swap(stateB);
  }
<//>/>)dnl

        // start columns for all possible outputs
<//>m4_foreach(</_Q_/>,</M4_GLADesc/>,</dnl
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>GLA_OUTPUTS(_Q_))),</dnl
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
  if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
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
                    <//><//>/>,</dnl
      {
                FATAL("Do not know how to deal with this type");
<//><//>/>)dnl
dnl # write the tuple
      myOutBStringIter.Insert (M4_QUERY_NAME(_Q_));
      myOutBStringIter.Advance ();

<//>m4_foreach(</_Q_/>,</M4_GLADesc/>,</dnl
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>GLA_OUTPUTS(_Q_))),</dnl
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
    if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>GLA_OUTPUTS(_Q_))),</dnl
        Column col_<//>_A_;
        _A_<//>_Column_Out.Done(col_<//>_A_);
                output.SwapColumn (col_<//>_A_, M4_ATT_SLOT(_A_));
<//><//>/>)dnl
        }
<//>/>)dnl
    // and get outta here!
    ChunkContainer tempResult (output);
    tempResult.swap (result);

    //printf("End GLALargeFinalizeWorkFunc\n");

    return 3; // for finalize
}

extern "C"
int GLALargeDeallocateWorkFunc_<//>M4_WPName
(WorkDescription &workDescription, ExecEngineData &result) {
  //printf("Begin GLALargeDeallocateWorkFunc\n");

  GLALargeDeallocateWD myWork;
  myWork.swap(workDescription);

  QueryToGLASContMap& queryGLACont = myWork.get_glaStates();
  QueryExitContainer& queries = myWork.get_whichQueryExits();

  M4_DECLARE_QUERYIDS(</M4_GLADesc/>,<//>)dnl

  FOREACH_TWL(iter, queries) {
    FATALIF(!queryGLACont.IsThere(iter.query), "Why did we get a query in the list but no stateContainer for it?");
    GLAStateContainer& glaContainer = queryGLACont.Find(iter.query);

<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl # this is a valid query
    if( iter.query == M4_QUERY_NAME(_Q_)) {
      // Extract each element and convert, then delete
      FOREACH_TWL( g, glaContainer ) {
        GLAPtr localState;
    localState.swap(g);
    GLA_TYPE(_Q_)* localGLA = (GLA_TYPE(_Q_)*) localState.get_glaPtr();
    delete localGLA;
      }
    }
<//><//>/>, <//>)dnl
<//>/>)dnl

  }END_FOREACH;

  //printf("End GLALargeDeallocateWorkFunc\n");

  return 4; // for deallocate
}

dnl # No seriously, why is this happening. Before adding these
dnl # next five lines, the number of opening and closing braces were
dnl # perfectly matched, but it won't compile without these extras.
<//>m4_foreach(</_Q_/>, </M4_GLADesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl # this is a valid query
}
<//><//>/>, <//>)dnl
<//>/>)dnl
