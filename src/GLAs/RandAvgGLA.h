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
#ifndef _RAND_AVERAGE_GLA_H_
#define _RAND_AVERAGE_GLA_H_

#define RA_CHUNK_SIZE 1000000

// iterator is just an int
class RandAvgGLA_Iterator{
	int pos;
	long long int start; // start position

 public:
	RandAvgGLA_Iterator(int fragment){ pos=0; start=fragment*RA_CHUNK_SIZE; }
	bool AtEnd(void){ return (pos>=RA_CHUNK_SIZE); }
	long long int GetInt(void){ return start+(pos++); }
};

class RandAvgGLA {
  long long int count; // keeps the number of tuples aggregated
  long double sum; // sum of the values
public:
  RandAvgGLA(){ count=0; sum=0.0; }

  void AddItem(const DOUBLE& x){ count++; sum+=x; }

  void AddState(RandAvgGLA& o){ count+=o.count; sum+=o.sum; }

  // returns the nubmer of fragments we need for extracting the result
  // each fragment will result in a chunk
  int GetNumFragments(void){ return 100; }
  
  // must get the object ready to produce tuples
  // and initialize the iterator
  // whoever uses this should distroy the iterator when done
  RandAvgGLA_Iterator* Finalize(int fragment){
	  RandAvgGLA_Iterator* rez = new RandAvgGLA_Iterator(fragment);
	  return rez;
  }
  
  bool GetNextResult(RandAvgGLA_Iterator* it, DOUBLE& rez){
	  if (it->AtEnd())
		  return false;
	  
	  int i = it->GetInt();
	  rez = sum/(count+i);
	  
	  return true;
  }

};

#endif // _AVERAGE_GLA_H_
