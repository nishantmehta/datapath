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
dnl							 ( Query, Predicate, Constant_initializers, ( list synthesized ) )
dnl listSynthesized: list of (attName, expression)
dnl M4_Attribute_Queries -- the set of queries in which each attribute is used
dnl				elements of the form: ( attribute, serialized_QueryIDSet )
dnl
// module specific headers to allow separate compilation
// no includes for selection


#ifndef COUNT_TUPLES
//#define COUNT_TUPLES
#endif

extern "C"
int SelectionWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {
	// go to the work description and get the input chunk
	SelectionWorkDescription myWork;
	myWork.swap (workDescription);
	Chunk &input = myWork.get_chunkToProcess ();

  M4_DECLARE_QUERYIDS(</M4_QueryDesc/>,</M4_Attribute_Queries/>)dnl 

	M4_GET_QUERIES_TO_RUN(</myWork/>)dnl	

  M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl

  M4_EXTRACT_BITMAP(</input/>)dnl

dnl # create syntesized columns
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </dnl is this a valid query
<//><//><//>M4_CREATE_SYNTHESIZED(_P_)dnl
<//><//>/>, <//>)dnl						
<//>/>)dnl


	M4_START_OUTPUT_BITMAP dnl

dnl # definition of constants used in expressions
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </ dnl is this a valid query
    // constants for query M4_QUERY_NAME(_P_)
		_SEL_INITIALIZER(_P_)dnl # the initializer should have a new line
<//><//>/>, <//>)dnl						
<//>/>)dnl


  FOR_EACH_TUPLE(</input/>){
    QueryIDSet qry;		GET_QUERIES(qry);

    // extract values of attributes from streams
    M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)		

    //selection code for all the predicates
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </ dnl is this a valid query
    // do M4_QUERY_NAME(_P_)
		M4_DECLARE_SYNTHESIZED(_P_)
    if(qry.Overlaps(M4_QUERY_NAME(_P_)) && !(M4_EVAL_EXPR(_SEL_PREDICATE(_P_)) )){
      qry.Difference(M4_QUERY_NAME(_P_));		
		} else { // compute synthesized
			M4_COMPUTE_SYNTHESIZED(_P_)dnl
		}
		M4_WRITE_SYNTHESIZED(_P_)
<//><//>/>, <//>)dnl						
<//>/>)dnl

    SET_QUERIES(qry);
  }

  // finally, if there were any results, put the data back in the chunk 
<//>M4_PUTBACK_COLUMNS(</M4_Attribute_Queries/>,</input/>)
dnl # now synthesized
<//>m4_foreach(</_P_/>, </M4_QueryDesc/>, </dnl
<//><//>m4_ifval( M4_QUERY_NAME(_P_), </ dnl is this a valid query
		if (M4_QUERY_NAME(_P_).Overlaps(queriesToRun)){			
<//><//><//>M4_CLOSE_SYNTHESIZED(_P_, </input/>)dnl
    }
<//><//>/>, <//>)dnl						
<//>/>)dnl
<//>M4_PUTBACK_OUTBITMAP(</input/>);

#ifdef COUNT_TUPLES
 cout << "\nCounter ***************** = " << cnt;
#endif 
 	ChunkContainer tempResult (input);
	tempResult.swap (result);

  return 1;
}
