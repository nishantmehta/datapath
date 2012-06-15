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
#include "AggregateStates.h"
#include <pthread.h>
#include "AggStorage.h"	 
#include <iomanip>
#include <assert.h>

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

dnl # selection code for all the predicate

M4_DEFINE_AGG_STATES(</M4_Aggregates/>)dnl


extern "C"
int AggFinishUpWorkFunc_<//>M4_WPName 
(WorkDescription &workDescription, ExecEngineData &result) {

	FinishAggregate myWork;
	myWork.swap (workDescription);

  M4_DECLARE_QUERYIDS(</M4_Aggregates/>,</M4_Attribute_Queries/>)dnl 

	// set up the output chunk
	Chunk output;

	M4_GET_QUERIES_TO_RUN(</myWork/>)dnl	

  Bitstring allQueries;

  m4_foreach(</_Q_/>, </M4_Aggregates/>, </dnl
    // do M4_QUERY_NAME(_Q_)
    if (queriesToRun.Overlaps(M4_QUERY_NAME(_Q_))){
  		// find this guy's agg result
  		AggStorageMap &myMap = myWork.get_aggFuncs ();
	  	myMap.Lock ();

		QueryID foo;
		AggStorage agg;
	        myMap.Remove (M4_QUERY_NAME(_Q_), foo, agg);

		M4_QUERY_STATE_TYPE(_Q_)* content_<//>M4_QUERY_STATE_TYPE(_Q_) = ((M4_QUERY_STATE_TYPE(_Q_)*)agg.GetContent()) ;

		M4_QUERY_STATE_TYPE(_Q_)& M4_QUERY_STATE(_Q_) = *content_<//>M4_QUERY_STATE_TYPE(_Q_);
  		myMap.Unlock ();
    
      M4_COMPUTE_AGGREGATE(</_Q_/>)dnl

		allQueries.Union(M4_QUERY_NAME(_Q_));
		agg.Reset(); // to avoid errors on destructor
		delete content_<//>M4_QUERY_STATE_TYPE(_Q_);
    }
  />)dnl

	MMappedStorage bitStore;
	Column outBitCol(bitStore);
	BStringIterator outQueries (outBitCol, allQueries, 1 );
 	outQueries.Done();
	//SS outQueries.Done(outBitCol);
	//SS output.SwapBitmap(outBitCol);
	output.SwapBitmap(outQueries);

	// and get outta here!	
	ChunkContainer tempResult (output);
	tempResult.swap (result);

  // 1 has to be returned here so that no ack is sent
	return 1;
}



extern "C"
int AggOneChunkWorkFunc_<//>M4_WPName  (WorkDescription &workDescription, ExecEngineData &result) {

	AggregateOneChunk myWork;
	myWork.swap (workDescription);
	Chunk &input = myWork.get_chunkToProcess ();

  M4_DECLARE_QUERYIDS(</M4_Aggregates/>,</M4_Attribute_Queries/>)dnl 

	M4_GET_QUERIES_TO_RUN(</myWork/>)dnl	

  M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl
  
	M4_EXTRACT_BITMAP(</input/>)dnl
	M4_DEFINE_STATES(</M4_Aggregates/>)dnl


dnl # definition of constants used in expressions
<//>m4_foreach(</_Q_/>, </M4_Aggregates/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_Q_), </ dnl is this a valid query
    // constants for query M4_QUERY_NAME(_Q_)
		_AGG_INITIALIZER(_Q_)dnl # the initializer should have a new line
<//><//>/>, <//>)dnl						
<//>/>)dnl

  FOR_EACH_TUPLE(</input/>){
  
    QueryIDSet qry;
		GET_QUERIES(qry);

<//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)			
<//><//>m4_foreach(</_A_/>, </M4_Attribute_Queries/>, </dnl
<//>dnl access only valid attributes
<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
<//>/>)dnl
<//>/>)dnl
<//>m4_foreach(</_Q_/>, </M4_Aggregates/>, </dnl
    // do M4_QUERY_NAME(_Q_)
	  if (qry.Overlaps(M4_QUERY_NAME(_Q_))){
			M4_ADD_ITEM_TO_STATE(_Q_)dnl		
		}
<//>/>)dnl
	}

	// now update the aggregates
	AggStorageMap &myMap = myWork.get_aggFuncs ();
	
	myMap.Lock ();
<//>m4_foreach(</_Q_/>, </M4_Aggregates/>, </dnl
<//>// do M4_QUERY_NAME(_Q_)
       if (!myMap.IsThere(M4_QUERY_NAME(_Q_))){
       	  // state does not exist, creating empty one
	  struct M4_QUERY_STATE_TYPE(_Q_)* M4_QUERY_STATE(_Q_)<//>oldPtr 
	  	 = new M4_QUERY_STATE_TYPE(_Q_);
	 AggStorage storage(M4_QUERY_STATE(_Q_)<//>oldPtr);
	 QueryID foo = M4_QUERY_NAME(_Q_);
	 myMap.Insert( foo ,storage);
	}

	struct M4_QUERY_STATE_TYPE(_Q_)* M4_QUERY_STATE(_Q_)<//>oldPtr =
	       (M4_QUERY_STATE_TYPE(_Q_)*) myMap.Find(M4_QUERY_NAME(_Q_)).GetContent() ;

	FATALIF(M4_QUERY_STATE(_Q_)<//>oldPtr == NULL, "We just put an empty state above, how comes we do not find a state");

	// now add the new state to the old state
<//><//>m4_foreach(</_A_/>, m4_quote(M4_ATT_LIST((M4_ATT_LIST(</_Q_/>)))), </dnl
<//><//><//>M4_QUERY_STATE(_Q_)<//>oldPtr->M4_AGG_NAME(_A_).AddState(M4_QUERY_STATE(_Q_).M4_AGG_NAME(_A_));
<//>/>)dnl
<//>/>)dnl	
	myMap.Unlock ();

  // finally, if there were any results, put the data back in the chunk 
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
<//>M4_PUTBACK_INBITMAP(</input/>);

  // 0 has to be returned here for the SendAckMsg	
  return 0;
}
