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
#ifndef _COUNT_GLA_H_
#define _COUNT_GLA_H_

/** Info for the meta-GLAs
    m4_define(</CountGLA_INPUT/>,</(dummy, INT)/>)dnl
    m4_define(</CountGLA_OUTPUT/>,</(_count, BIGINT)/>)dnl
 */

class CountGLA {
  long long int count; // keeps the number of tuples aggregated
public:
  CountGLA(){ count=0; }
  void AddItem(int dummy){ count++; }
  void AddState(CountGLA& o){ count+=o.count; }

  // we only support one tuple as output
  void GetResult(BIGINT& _count){
	  _count=count;
  }
};

#endif // _COUNT_GLA_H_
