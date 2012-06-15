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
dnl # the following macros should be defined when including this file
dnl # CLASS_NAME: name of the generated class
dnl # INNER_GLA: the name of the inner GLA
dnl # NameOfInnerGLA_INPUT: list of inputs for the inner gla
dnl # NameOfInnerGLA_OUTPUT: list of outputs. 

#include <vector>
#include <map>
#include "Stl.h"

class DATETIME {
     int t; 
     DATE d; 
     
public:
	int GetTime(void){ return t; }
	DATE GetDate(void){ return d; }
};

using namespace std;

#define NUM_GRANULARITIES 9
static const int numGranularities = 5;
// initial values of counters w.r.t. the finest granularity 
static int granularities[numGranularities] = { 1, 5, 60, 300, 3600 }; 


/** This class encodes a time series. It allows tracking of
    information for a given time step 

    Input is a DataTime object. Internally, it gets transformed into
    seconds, normalied from the begining of the tracked interval.

    This is designed for trading data, so time covers only the 9:30-4
    time interval

    Date filtering is performed outside.

*/

class CLASS_NAME {
private:
  // this are all constants since we never change them during execution
  const int increment; // the granularity of the time series
  // 1 means 1 second granularity, 0 is illegal.
  const int nine30; // the time in seconds for 9:30, start of trading
  const int four00; // same for 4:00
  const int sizeT; // number of bins for a tradind day


  // container
  typedef vector< INNER_GLA > TimeStructure; // datastructure to keep track of time info
  typedef map< DATE, TimeStructure > TSMap;
  TSMap stat; // the overall statistics
  /** this already sets the day as the first granularity */

public:
  /** Constructor
      
   */
  CLASS_NAME<//>(int _increment, DATETIME _start, DATETIME _end);

  /** t is the time and x the value */
  void AddItem(DATETIME t, TYPED_REF_ARGS(INNER_GLA</_INPUT/>)){
    int _t = t.GetTime(); // time in seconds for this entry
    if (_t<nine30 || _t>four00)
      return;

    int pos = (_t-nine30)/increment; // position in vector
    DATE d = t.GetDate();
    if (stat.find(d) == stat.end()){
      // if not found, initialize the vector
      vector<INNER_GLA> v;
      stat.insert(TSMap::value_type(d, v));
      stat[d].resize( sizeT );
    }
    vector<INNER_GLA>& vec = stat[d];
    vec[pos].AddItem(ARGS(INNER_GLA</_INPUT/>));
  }

  void AddState(CLASS_NAME& o){
    // scan map of o
    FOREACH_STL(el, o.stat){
      if (stat.find(el.first) != stat.end()) {     // if date in, merge the vectors
	vector< INNER_GLA > & vec = stat[el.first]; // my vector
	for (int i=0; i<sizeT; i++){
	  vec[i].AddState( el.second[i] );
	} 
      } else { // if date not in this object, get it
	stat.insert(TSMap::value_type(el.first, el.second));
      }


    }END_FOREACH;
  }

  /** in function finalize get ready for tuple production. To maintain
      the coarsness levels, we use 1 counter/level

      Coarsness levels:
      0  1  2  3  4  5  6  7    8
      1s 5s 1m 5m 1h 1d 1w 1mth 1y 

  */


  void Finalize(void){
    // initialize counters
    // count backwards
    // counting done in seconds (smallest granularity)
  }
   

  /** GetNextResult does most of the work

      On each call:L
      1. Check, from coarse to fine, if a counter is 0
      2. if yes, produce the correspondin result, reset counter, reset GLA
      3. if all other counters are not done, produce a 1s result
        Add the 1s GLA to all other counters. 
	No overflow is possible since the 1s fires up only if no counters are ready
      
      
   */
  bool GetNextResult(DATETIME& start, INT& granularity, TYPED_REF_ARGS(INNER_GLA</_OUTPUT/>)){

    // somewhere inside
    INNER_GLA someGLA; // select this from state
    someGLA.GetResult(ARGS(INNER_GLA</_OUTPUT/>));
    
  }



};

/************** INLINE FUNCTIONS **************************/
CLASS_NAME::CLASS_NAME<//>(int _increment, DATETIME _start, DATETIME _end):
increment(_increment), nine30(0), four00(1000), sizeT((four00-nine30)/increment+1)
{}


