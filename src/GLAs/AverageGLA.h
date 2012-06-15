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
#ifndef _AVERAGE_GLA_H_
#define _AVERAGE_GLA_H_

/** Information for Meta-GLAs
 * m4_define(</AverageGLA_INPUT/>, </(x, DOUBLE)/>)
 * m4_define(</AverageGLA_OUTPUT/>, </(_count, BIGINT), (_sum, DOUBLE), (avg, DOUBLE)/>)
 * m4_define(</GLA_REZTYPE_AverageGLA/>, </single/>)
 */

class AverageGLA {
  long long int count; // keeps the number of tuples aggregated
  long double sum; // sum of the values
public:
  AverageGLA(){ count=0; sum=0.0; }
  AverageGLA(BIGINT _count, DOUBLE _sum){ count=_count; sum=_sum; }

  void AddItem(const DOUBLE& x){ count++; sum+=x; }

  void AddState(AverageGLA& o){ count+=o.count; sum+=o.sum; }

  // we only support one tuple as output
  void GetResult(BIGINT& _count, DOUBLE& _sum, DOUBLE& avg){
      _count=count;
      _sum=sum;
      avg = (count>0) ? (sum/count) : 0.0;
  }

  // do some processing at chunk boundary
  void ChunkBoundary(void){
      cout << "Boundary" << endl;
  }
};

#endif // _AVERAGE_GLA_H_
