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
#ifndef _TOPK_GLA_H_
#define _TOPK_GLA_H_

#include <algorithm>
#include <functional>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;


struct TopK_Tuple {
	INT x; 
	FLOAT topKScore;

	TopK_Tuple() {
		topKScore = 0;
	}

	TopK_Tuple(IPV4ADDR _url, FLOAT _rank) {
		url = _url;
		topKScore = _rank;
	}

	TopK_Tuple& operator=(const TopK_Tuple& _other) {
		url = _other.url;
		topKScore = _other.topKScore;

		return *this;
	}
};

// Auxiliary function to compare tuples
inline bool GreaterThenTopK(TopK_Tuple& t1, TopK_Tuple& t2) {
	return (t1.topKScore > t2.topKScore);
}

/** This class implements the computation of Top-k tuples. Input and
	*	output tuple have to be defined to be the same: Tuple.

	* The heap is maintained upside down (smallest value on top) so that
	* we can prune most insertions. If a new tuple does not compete with
	* the smallest value, we do no insertion. This pruning is crucial in
	* order to speed up inserition. If K is small w.r.t the size of the
	* data (and the data is not adversarially sorted), the effort to
	* compute Top-k is very close to O(N) with a small constant. In
	* practical terms, Top-k computation is about as cheap as
	* evaluating a condition or an expression.

	* InTuple: Tuple
	* OutTuple: Tuple

	* Assumptions: the input tuple has as member a numeric value called
	* "topKScore". What we mean by numeric is that is supports
	* conversion to double and has > comparison.
**/
template<int K /* as in top-k */>
class GLA_TopK : public GLA {
private:
	// types
	typedef vector<TopK_Tuple> TupleVector;

  TupleVector tuples;
  int pos; // position of the output iterator

	// function to force sorting so that GetNext gets the tuples in order
	void Sort() {sort_heap(tuples.begin(), tuples.end(), GreaterThenTopK);}

public:
	// constructor & destructor
	GLA_TopK() { pos = -1; }
	~GLA_TopK() {}

	// function to add an intem
	void AddItem(IPV4ADDR& _url, FLOAT& _rank);

	// take the state from ohter and incorporate it into this object
	// this is a + operator on GLA_TopK
	void AddState(GLA_TopK& other);

	// finalize the state and prepare for result extraction
	void Finalize() { Sort(); pos = 0; }

	// iterator through the content in order (can be destructive)
	bool GetNext(IPV4ADDR& _url, FLOAT& _rank) {
		if (pos == tuples.size())
			return false;
		else {
			_url = tuples[pos].url;
			_rank = tuples[pos++].topKScore;
			return true;
		}
	}

};


template<int K>
void GLA_TopK<K>::AddItem(IPV4ADDR& _url, FLOAT& _rank) {
	if (tuples.size() < K) {
			TopK_Tuple tuple(_url, _rank);
		  tuples.push_back(tuple);

			// when we have exactly K elements in the vector, organize it as a heap
			if (tuples.size() == K) {
			  make_heap(tuples.begin(), tuples.end(), GreaterThenTopK);
			}
	}
	else {
		double d = tuples.front().topKScore;

		if (_rank > d) {
			pop_heap(tuples.begin(), tuples.end(), GreaterThenTopK);
			tuples.pop_back();
			TopK_Tuple tuple(_url, _rank);
			tuples.push_back(tuple) ;
			push_heap(tuples.begin(), tuples.end(), GreaterThenTopK);
		}
	}
}

template<int K>
void GLA_TopK<K>::AddState(GLA_TopK<K>& other) {
	// go over all the contents of other and insert it into ourselves
	for(int i = 0; i < other.tuples.size(); i++) {
		AddItem(other.tuples[i].url, other.tuples[i].topKScore);
	}
}

#endif // _TOPK_GLA_H_
