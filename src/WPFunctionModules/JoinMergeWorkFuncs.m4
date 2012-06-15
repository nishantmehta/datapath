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
dnl  We assume tha this file is included from Modules.m4 and that all
dnl  the m4 libraries needed are loaded
dnl

dnl Arguments:
dnl
dnl M4_WPName -- name of the waypoint
dnl
dnl
dnl M4_Attribute_Queries_RHS -- the set of queries in which each attribute is used
dnl       elements of the form: ( attribute, serialized_QueryIDSet )
dnl              /*same structure as attribute queries */

m4_divert(-1)
include(Resources-T.m4)

dnl m4_include(Attributes.m4)
m4_include(FileLoaderFunctions.m4)
m4_include(WPLib.m4)
m4_include(WPJoinLib.m4)
m4_include(CodeGenerationFunctions.m4)
m4_divert(0)


#define USE_PQ

#ifdef USE_PQ
//#include <algorithm>
#include <vector>
#else
#include <vector>
#include<set>
#endif
#
#include "WorkDescription.h"
#include "ExecEngineData.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "BString.h"
#include "BStringIterator.h"
#include "HashTableMacros.h"

#include <string.h>

using namespace std;

#define MAX_HASH 0xffffffffffff
#define MAX_NUM_CHUNKS 128 // maximum number of chunks we can have for linear scan solution

#ifndef USE_PQ
struct HashWrapper {
	HashWrapper() : hash(MAX_HASH), chunkNo(0) {}
	__uint64_t hash:48;
	__uint64_t chunkNo:16;
};

#endif

dnl # $1 is LHS or RHS
dnl # this is the loop that finds the best entry using linear scan
dnl # minIndex$1 is set at the best position
dnl # in put assumed in hashes$1
m4_define(</FIND_MIN/>, </dnl 
	best$1= MAX_HASH;
	for (int i=0; i < num$1Chunks; i++) { 
	  HT_INDEX_TYPE hash = hashes$1[i]; // current value  
	  minIndex$1 = (best$1 > hash) ? i : minIndex$1;
	  best$1 = (best$1 > hash) ? hash: best$1; 
	}
	// remove this after debugging
	FATALIF(best$1 != MAX_HASH && WHICH_SEGMENT(best$1) !=0, "Hash out of range %ld", best$1);

/>)dnl

/*
void check_correctness(int start, int end, BStringIterator& biter, ColumnIterator<__uint64_t>& hiter) {
		assert(start == end);
		int s = 0;
		int e = 262143;
		if (start > 0) {
			s = start * 262143 + (start-1);
			e = s + 262143 + 1;
		}
		int tuple = 0;
		while (!biter.AtEndOfColumn()) {
			if (!(s < hiter.GetCurrent() && hiter.GetCurrent() <= e))
			printf("\n -------   (%d,%d)(%d,%d)  currVal = %d", start,end,s,e,hiter.GetCurrent());
			assert(s <= hiter.GetCurrent() && hiter.GetCurrent() <= e);
			hiter.Advance();
			biter.Advance();
			tuple++;
		}
		printf("\n num tuples = %d", tuple);
}
*/

#ifndef USE_PQ
// make heap comparator
struct compare_key {
        bool operator()( const HashWrapper lhs, const HashWrapper rhs )
        {
                return (rhs.hash < lhs.hash);
        }
};

struct compare_key_rev {
        bool operator()( const HashWrapper lhs, const HashWrapper rhs )
        {
                return (lhs.hash < rhs.hash);
        }
};
#endif

m4_define(</M4_ADVANCE_CALL/>, </dnl
	// here advance all columns of $1 chunk number $2
	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_$1/>, </dnl
		<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
			col$1IterVec_<//>M4_ATT_AQ(_A_)[$2].Advance();
		<//>/>)dnl
	<//>/>)dnl
	myInBStringIter$3Vec[$2].Advance();
	col$1IterVecHash[$2].Advance();
/>)dnl

m4_define(</M4_ADVANCE_SEARCH_CALL/>, </dnl
	//while (!myInBStringIter$4Vec[$2].AtEndOfColumn()) {
	if (!myInBStringIter$4Vec[$2].AtEndOfColumn()) {
	//while (!$1[$2].AtUnwrittenByte()) {
		// Now find the first tuple for which query is active
		//Bitstring curBits(myInBStringIterLhsVec[$2].GetCurrent ());
		//curBits.Intersect (queriesToRun);
		// If tuple has some active query, fill the heap
		//if (!curBits.IsEmpty()) {
			totalguys$3++;
#ifdef USE_PQ
			// set new value in place of old value
			hashes$3[minIndex$3]  = $1[minIndex$3].GetCurrent();
#else
			HashWrapper w;
			w.hash = $1[$2].GetCurrent(); // assume first column of each chunk is hash
			w.chunkNo = $2;

			minHeap$3.insert (w);
#endif
/*
			break; // while loop
		} else {
			// If this tuple don't have active query, advance all the columns of this chunk
			// including Bitstring column
			<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_$3/>, </dnl
				<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
				col$3IterVec_<//>M4_ATT_AQ(_A_)[$2].Advance();
			<//>/>)dnl
				<//>/>)dnl
				myInBStringIter$4Vec[$2].Advance();
				col$3IterVecHash[$2].Advance();
		}
*/
	}
#ifdef USE_PQ
	//make_heap(minHeap$3.begin(), minHeap$3.end(), $5());
	FIND_MIN($3)
#endif
/>)dnl

/*
	This takes two sorted list of chunks, lhs list and rhs list, and do the sort merge join.
	It maintaines a min heap of hash values on top of each list and virtually now we have
	just 2 lists to do sort merge. When some value matches from LHS and RHS heap, we need
	to checkpoint all the iterators of all the columns of all the RHS chunks including storing
	the heap, so that we can restore them all if another consecutive LHS value matches.
*/

extern "C"
int JoinMergeWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {

int totalguysLHS = 0;
int totalguysRHS = 0;
int total = 0;
int totalhash = 0;

	double start_time = global_clock.GetTime();

	// get the input LHS and RHS chunk container from work descripton
	JoinMergeWorkDescription myWork;
	myWork.swap (workDescription);
	ContainerOfChunks &inputLHSList = myWork.get_chunksLHS ();
	ContainerOfChunks &inputRHSList = myWork.get_chunksRHS ();

	int start = myWork.get_start();
	int end = myWork.get_end();

	// get the number of chunks in each list to create vectors of this length
	int numLHSChunks = inputLHSList.Length();
	int numRHSChunks = inputRHSList.Length();

  // get the waypoint identifier
  unsigned int wayPointID = myWork.get_wayPointID ().GetID();

	vector<ColumnIterator<__uint64_t> > colLHSIterVecHash;
	vector<ColumnIterator<__uint64_t> > colRHSIterVecHash;
	colLHSIterVecHash.resize(numLHSChunks);
	colRHSIterVecHash.resize(numRHSChunks);

  // get the input bitmap out of the LHS input chunks
	vector<BStringIterator> myInBStringIterLhsVec;
	myInBStringIterLhsVec.resize(numLHSChunks); // set the vector size
	assert(inputLHSList.Length());
	inputLHSList.MoveToStart();
	int i = 0;
	while (inputLHSList.RightLength()) {
  	BStringIterator myInBStringIter;
  	inputLHSList.Current().SwapBitmap (myInBStringIter);
		myInBStringIterLhsVec[i].swap (myInBStringIter);
		myInBStringIterLhsVec[i].SetFragmentRange(start, end);
		//fprintf(stderr, "\nLHS %d", myInBStringIterLhsVec[i].GetNumTuples());
					dnl Column col1_<//>M4_ATT_AQ(_A_);
					dnl inputLHSList.Current().SwapHash(col1_<//>M4_ATT_AQ(_A_));
					dnl assert (col1_<//>M4_ATT_AQ(_A_).IsValid());
					dnl ColumnIterator<__uint64_t> iter_<//>M4_ATT_AQ(_A_)(col1_<//>M4_ATT_AQ(_A_));
					dnl colLHSIterVecHash[i].swap(iter_<//>M4_ATT_AQ(_A_));
		// get the hash now
		Column col_hash;
		inputLHSList.Current().SwapHash(col_hash);
		assert(col_hash.IsValid());
		ColumnIterator<__uint64_t> iter (col_hash, start, end);
		colLHSIterVecHash[i].swap(iter);
		//check_correctness(start, end, myInBStringIterLhsVec[i], colLHSIterVecHash[i]);
		inputLHSList.Advance ();
		i++;
	}

  // get the input bitmap out of the RHS input chunks
	vector<BStringIterator> myInBStringIterRhsVec;
	myInBStringIterRhsVec.resize(numRHSChunks); // set the vector size
	assert(inputRHSList.Length());
	inputRHSList.MoveToStart();
	i = 0;
	while (inputRHSList.RightLength()) {
		BStringIterator myInBStringIter;
		inputRHSList.Current().SwapBitmap(myInBStringIter);
		myInBStringIterRhsVec[i].swap (myInBStringIter);
		myInBStringIterRhsVec[i].SetFragmentRange(start, end);
		//fprintf(stderr, "\nRHS %d", myInBStringIterRhsVec[i].GetNumTuples());
		// get the hash now
		Column col_hash;
		inputRHSList.Current().SwapHash(col_hash);
		assert(col_hash.IsValid());
		ColumnIterator<__uint64_t> iter (col_hash, start, end);
		colRHSIterVecHash[i].swap(iter);
		//check_correctness(start, end, myInBStringIterRhsVec[i], colRHSIterVecHash[i]);
		inputRHSList.Advance ();
		i++;
	}

  // get all of the queries that are active here
	M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

	// this is the output chunk
	Chunk output;

	// create output iterators
	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_LHS_Copy/>, </dnl
		<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
			MMappedStorage store_<//>M4_ATT_AQ(_A_);
			Column col_<//>M4_ATT_AQ(_A_)(store_<//>M4_ATT_AQ(_A_));
			M4_COL_TYPE(M4_ATT_AQ(_A_)) colLHSOutIter_<//>M4_ATT_AQ(_A_)(col_<//>M4_ATT_AQ(_A_));
		<//>/>)dnl
	<//>/>)dnl

	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_RHS_Copy/>, </dnl
		<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
			MMappedStorage store_<//>M4_ATT_AQ(_A_);
			Column col_<//>M4_ATT_AQ(_A_)(store_<//>M4_ATT_AQ(_A_));
			M4_COL_TYPE(M4_ATT_AQ(_A_)) colRHSOutIter_<//>M4_ATT_AQ(_A_)(col_<//>M4_ATT_AQ(_A_));
		<//>/>)dnl
	<//>/>)dnl

	// Create output BitString
	MMappedStorage myStore;
	Column bitmapOut (myStore);
	BStringIterator myOutBStringIter (bitmapOut, queriesToRun);

	// Build input iterators vectors first
	// Define only those attributes iterator which are required by some query
	// vector contains same type of column in all chunks, each vector index represents
	// a chunk
	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_LHS/>, </dnl
		<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
			vector<M4_COL_TYPE(M4_ATT_AQ(_A_)) > colLHSIterVec_<//>M4_ATT_AQ(_A_);
			colLHSIterVec_<//>M4_ATT_AQ(_A_).resize(numLHSChunks);
		<//>/>)dnl
	<//>/>)dnl

	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_RHS/>, </dnl
		<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
			vector<M4_COL_TYPE(M4_ATT_AQ(_A_)) > colRHSIterVec_<//>M4_ATT_AQ(_A_);
			colRHSIterVec_<//>M4_ATT_AQ(_A_).resize(numRHSChunks);
		<//>/>)dnl
	<//>/>)dnl

	// Extract columns now.
	// This extracts columns if there is any query for it in this WP
	i = 0;
	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_LHS/>, </dnl
	{
		i = 0;
		inputLHSList.MoveToStart();
		while (inputLHSList.RightLength()) {
			// WP node queries intersect Translator provided queries
			QueryIDSet M4_ATT_AQ(_A_)_Qrys(M4_QUERIES_AQ(_A_), true);
			M4_EXTRACT_COLUMN_FRAGMENT(M4_ATT_AQ(_A_),inputLHSList.Current(), start, end)dnl
			<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
				if (M4_ATT_AQ(_A_)_Qrys.Overlaps(queriesToRun)){
					colLHSIterVec_<//>M4_ATT_AQ(_A_)[i].swap(M4_ATT_DATA(M4_ATT_AQ(_A_)));
				}
			<//>/>)dnl
			inputLHSList.Advance ();
			i++;
		}
	}
	<//>/>)dnl

	<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_RHS/>, </dnl
	{
		i = 0;
		inputRHSList.MoveToStart();
		while (inputRHSList.RightLength()) {
			// WP node queries intersect Translator provided queries
			QueryIDSet M4_ATT_AQ(_A_)_Qrys(M4_QUERIES_AQ(_A_), true);
			M4_EXTRACT_COLUMN_FRAGMENT(M4_ATT_AQ(_A_),inputRHSList.Current(),start,end)dnl
			<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
				if (M4_ATT_AQ(_A_)_Qrys.Overlaps(queriesToRun)){
					colRHSIterVec_<//>M4_ATT_AQ(_A_)[i].swap(M4_ATT_DATA(M4_ATT_AQ(_A_)));
				}
			<//>/>)dnl
			inputRHSList.Advance ();
			i++;
		}
	}
	<//>/>)dnl


	// Here we start the sort merge join
	// Create priority queue for hash column values
#ifdef USE_PQ
        HT_INDEX_TYPE hashesLHS[MAX_NUM_CHUNKS];
        HT_INDEX_TYPE hashesRHS[MAX_NUM_CHUNKS];	
        FATALIF(numLHSChunks >= MAX_NUM_CHUNKS, "Too many chunks for LHS in Merge");
        FATALIF(numRHSChunks >= MAX_NUM_CHUNKS, "Too many chunks for RHS in Merge");
        HT_INDEX_TYPE bestLHS = MAX_HASH; 
        HT_INDEX_TYPE bestRHS = MAX_HASH; 
	int minIndexLHS = -1; // this indicates the chunk that is the best at this point
	// this invariant is maintained throught the code
	int minIndexRHS = -1;
#else
	multiset<HashWrapper, compare_key_rev> minHeapLHS;
	multiset<HashWrapper, compare_key_rev> minHeapRHS;
#endif

	// Fill the first value of each LHS chunk in LHS heap
	for (i = 0; i < numLHSChunks; i++) {
#ifdef USE_PQ
		minIndexLHS = i;
#endif
		M4_ADVANCE_SEARCH_CALL(colLHSIterVecHash,i,LHS,Lhs)
	}

	// Fill the first value of each RHS chunk in RHS heap
	for (i = 0; i < numRHSChunks; i++) {
#ifdef USE_PQ
		minIndexRHS = i;
#endif
		M4_ADVANCE_SEARCH_CALL(colRHSIterVecHash,i,RHS,Rhs)
	}

	// Now pick one of each heap and keep comparing until one of the heap is exhausted
#ifdef USE_PQ
/*	while (hashesLHS[minIndexLHS] != MAX_HASH && hashesRHS[minIndexRHS] != MAX_HASH) {
		HT_INDEX_TYPE wl = hashesLHS[minIndexLHS];
		HT_INDEX_TYPE wr = hashesRHS[minIndexRHS]; -- INEFFICIENT */

	while (bestLHS != MAX_HASH && bestRHS != MAX_HASH) {
		HT_INDEX_TYPE wl = bestLHS;
		HT_INDEX_TYPE wr = bestRHS;

#else
	while (!minHeapLHS.empty() && !minHeapRHS.empty()) {
		HashWrapper wlT = *(minHeapLHS.begin());
		HashWrapper wrT = *(minHeapRHS.begin());
		HT_INDEX_TYPE wl = wlT.hash; 
		HT_INDEX_TYPE wr = wrT.hash; 
		int minIndexLHS = wlT.chunkNo;
		int minIndexRHS = wrT.chunkNo;
#endif

		if (wl < wr) {	
		//	printf("\n Hash val not found is = %ld, segment = %ld, slot = %ld", wl, WHICH_SEGMENT (wl), WHICH_SLOT (wl)); fflush(stdout); assert(0);

			// erase the minimum element
#ifdef USE_PQ
			hashesLHS[minIndexLHS] = MAX_HASH;
#else
			assert (!minHeapLHS.empty());
			minHeapLHS.erase(minHeapLHS.begin());
#endif

			// here advance all columns of LHS of chunk number wl.chunkNo
			M4_ADVANCE_CALL(LHS,minIndexLHS,Lhs)
			M4_ADVANCE_SEARCH_CALL(colLHSIterVecHash,minIndexLHS,LHS,Lhs)

		} else if (wl > wr) {

			// erase the minimum element
#ifdef USE_PQ
			hashesRHS[minIndexRHS] = MAX_HASH;
#else
			assert (!minHeapRHS.empty());
			minHeapRHS.erase(minHeapRHS.begin());
#endif

			// here advance all columns of RHS of chunk number wr.chunkNo
			M4_ADVANCE_CALL(RHS,minIndexRHS,Rhs)
			M4_ADVANCE_SEARCH_CALL(colRHSIterVecHash,minIndexRHS,RHS,Rhs)

		} else { // (wl == wr)

			HT_INDEX_TYPE matchingHash = wl;

			// Save checkpoint for all rhs columns before incrementing both chunks
			for (int chk = 0; chk < numRHSChunks; chk++) {
				<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_RHS/>, </dnl
					<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
					colRHSIterVec_<//>M4_ATT_AQ(_A_)[chk].CheckpointSave();
				<//>/>)dnl
					<//>/>)dnl
				myInBStringIterRhsVec[chk].CheckpointSave();
				colRHSIterVecHash[chk].CheckpointSave();
			}

			// Also save the state of the heap
#ifdef USE_PQ
		        HT_INDEX_TYPE hashesRHS_copy[MAX_NUM_CHUNKS]; 
			memcpy(hashesRHS_copy, hashesRHS, numRHSChunks*sizeof(HT_INDEX_TYPE));
			int minIndexRHS_copy = minIndexRHS;
#else
			multiset<HashWrapper, compare_key_rev> minHeapRHSCheckpoint(minHeapRHS);
#endif

#ifdef USE_PQ
//			while (hashesLHS[minIndexLHS] != MAX_HASH) {
			while (bestLHS != MAX_HASH) {	
#else
			while (!minHeapLHS.empty()) { // break from this if 2 consecutive LHS mismatches
#endif

#ifdef USE_PQ
//				HT_INDEX_TYPE wl1 = hashesLHS[minIndexLHS];
				HT_INDEX_TYPE wl1 = bestLHS;
#else
				HashWrapper wl1T = *(minHeapLHS.begin());
				HT_INDEX_TYPE wl1 = wl1T.hash;
				int minIndexLHS = wl1T.chunkNo;
#endif

				if (wl1 != matchingHash) { // next LHS dont match to previous LHS hash, first time always match
					break;

				} else { // restore everything

					// restore the RHS Column iterators to original value
					for (int chk = 0; chk < numRHSChunks; chk++) {
						<//>m4_foreach( </_A_/>, </M4_Attribute_Queries_RHS/>, </dnl
							<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
							colRHSIterVec_<//>M4_ATT_AQ(_A_)[chk].CheckpointRestore();
						<//>/>)dnl
							<//>/>)dnl
							myInBStringIterRhsVec[chk].CheckpointRestore();
							colRHSIterVecHash[chk].CheckpointRestore();
					}
					// restore the original heap state
#ifdef USE_PQ
					memcpy(hashesRHS, hashesRHS_copy,  numRHSChunks*sizeof(HT_INDEX_TYPE));
					minIndexRHS = minIndexRHS_copy;
					bestRHS = hashesRHS[minIndexRHS_copy];
#else
					minHeapRHS.clear();
					minHeapRHS = minHeapRHSCheckpoint;
#endif
				}

#ifdef USE_PQ
//				while (hashesRHS[minIndexRHS] != MAX_HASH) {
				while (bestRHS != MAX_HASH) {	
#else
				while (!minHeapRHS.empty()) {
#endif

#ifdef USE_PQ
//					HT_INDEX_TYPE wr1 = hashesRHS[minIndexRHS];
					HT_INDEX_TYPE wr1 = bestRHS;
#else
					HashWrapper wr1T = *(minHeapRHS.begin());
					HT_INDEX_TYPE wr1 = wr1T.hash;
					int minIndexRHS = wr1T.chunkNo;
#endif

					if (wl1 == wr1) { // first one will obviously match as it matched before
						// Merge all columns here for wl1.chunkNo and wr1.chunkNo after matching attributes

						// Make sure both of their bitstrings intersect
						Bitstring rez = queriesToRun;
						rez.Intersect (myInBStringIterRhsVec[minIndexRHS].GetCurrent ());
						rez.Intersect (myInBStringIterLhsVec[minIndexLHS].GetCurrent ());

						// This contains union of all translater queries
						Bitstring uni = 0;
						Bitstring qBits;
						totalhash++;


						// Do the actual comparision
						bool anyOneMatch = false;
						<//>m4_foreach( </_A_/>, </M4_Queries_Attribute_Comparision/>, </dnl
							// See if any query in query class is eligible for this comparision
							qBits = QueryIDSet(m4_first(_A_), true);
							qBits.Intersect(rez);
							m4_define(</list/>, </m4_second(_A_)/>, <//>)
							if (
							/*	!qBits.IsEmpty () && */
							<//>m4_foreach( </_B_/>, m4_quote(reval(</m4_args/>list)), </dnl
								colLHSIterVec_<//>m4_first(_B_)[minIndexLHS].GetCurrent() == colRHSIterVec_<//>m4_second(_B_)[minIndexRHS].GetCurrent() &&
							<//>/>)dnl
								1 ) {
								anyOneMatch = true;
								uni.Union (qBits);
							}
						<//>/>)dnl

						if (anyOneMatch) {
							// fill the output iterators
							<//>m4_foreach( </_C_/>, </M4_Attribute_Queries_LHS_Copy/>, </dnl
								<//>M4_IFVALID_ATT(m4_first(_C_), </dnl
								//if (rez.Overlaps(m4_second(_C_))) { Should we check translator queries?
									colLHSOutIter_<//>m4_first(_C_).Insert(colLHSIterVec_<//>m4_first(_C_)[minIndexLHS].GetCurrent());
									colLHSOutIter_<//>m4_first(_C_).Advance();
								//}
								<//>/>)dnl
							<//>/>)dnl

							<//>m4_foreach( </_C_/>, </M4_Attribute_Queries_RHS_Copy/>, </dnl
								<//>M4_IFVALID_ATT(m4_first(_C_), </dnl
								//if (rez.Overlaps(m4_second(_C_))) {
									colRHSOutIter_<//>m4_first(_C_).Insert(colRHSIterVec_<//>m4_first(_C_)[minIndexRHS].GetCurrent());
									colRHSOutIter_<//>m4_first(_C_).Advance();
								//}
								<//>/>)dnl
							<//>/>)dnl
							myOutBStringIter.Insert (uni);
							total++;
							myOutBStringIter.Advance ();
						} else {
							//Bitstring b(0, true);
							//myOutBStringIter.Insert (b);
							//myOutBStringIter.Advance ();
						}
						
						// erase the minimum element
#ifdef USE_PQ
						hashesRHS[minIndexRHS] = MAX_HASH;
#else
						minHeapRHS.erase(minHeapRHS.begin());
#endif

						// here advance all columns of RHS of chunk number wr1.chunkNo
						M4_ADVANCE_CALL(RHS,minIndexRHS,Rhs)
						M4_ADVANCE_SEARCH_CALL(colRHSIterVecHash,minIndexRHS,RHS,Rhs)
					} else {
						break;
					}
				}

				// erase the minimum element
#ifdef USE_PQ
				hashesLHS[minIndexLHS] = MAX_HASH;
#else
				minHeapLHS.erase(minHeapLHS.begin());
#endif

				// here advance all columns of LHS of chunk number wl1.chunkNo
				M4_ADVANCE_CALL(LHS,minIndexLHS,Lhs)
				M4_ADVANCE_SEARCH_CALL(colLHSIterVecHash,minIndexLHS,LHS,Lhs)
			}
		}
	}

	// fill the output iterators
	<//>m4_foreach( </_C_/>, </M4_Attribute_Queries_LHS_Copy/>, </dnl
		<//>M4_IFVALID_ATT(m4_first(_C_), </dnl
				Column collhs_<//>m4_first(_C_);
				colLHSOutIter_<//>m4_first(_C_).Done(collhs_<//>m4_first(_C_));
				output.SwapColumn (collhs_<//>m4_first(_C_), M4_ATT_SLOT(m4_first(_C_)));
		<//>/>)dnl
	<//>/>)dnl

	<//>m4_foreach( </_C_/>, </M4_Attribute_Queries_RHS_Copy/>, </dnl
		<//>M4_IFVALID_ATT(m4_first(_C_), </dnl
				Column colrhs_<//>m4_first(_C_);
				colRHSOutIter_<//>m4_first(_C_).Done(colrhs_<//>m4_first(_C_));
				output.SwapColumn (colrhs_<//>m4_first(_C_), M4_ATT_SLOT(m4_first(_C_)));
		<//>/>)dnl
	<//>/>)dnl

	//myOutBStringIter.Done (bitmapOut);
	myOutBStringIter.Done ();
//	printf("\nTuples %d %d %d %d %d", myOutBStringIter.GetNumTuples(), total, totalguysLHS, totalguysRHS, totalhash);
//	fflush(stdout);

	PROFILING(start_time, "M4_WPName", "Merge", "%d\t%d", totalhash, total); 

	//output.SwapBitmap (bitmapOut);
	output.SwapBitmap (myOutBStringIter);
/*
	MMappedStorage st;
	Column co (st);
	Bitstring patt(0xf, true);
	BStringIterator iter(co, patt, 0);
	output.SwapBitmap(iter);
*/
	// and give back the result
	ChunkContainer tempResult (output);
	tempResult.swap (result);

						   return 0; // have to return something
}

//}
