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

#include <algorithm>
#include <map>
#include<vector>
#include "CPUWorkerPool.h"
#include "BString.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "CompressibleStorageUnit.h"
#include "BStringIterator.h"
#include "Coordinator.h"
#include "ExecEngine.h"
#include <ctime>
#include<stdio.h>

using namespace std;

#include <iostream>

#define MEMORY_SIZE 100000000

#define TOTAL_PATTERNS 500

#define STEP_SIZE 1000

ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (16);

// global coordinator. Used by waypoints that produce external
// messages such as Print
/* 0 is a bogus parameter to avoid default constructor */

Coordinator globalCoordinator(0);

/***
			This tests the bitstring iterator and BString classes, it stores random patterns some number of times
			and check for the sanity
***/

void fragment_check_bitstring () {

	// for sanity comparisions
	vector<vector<unsigned int> > vMark;

	// global count of pattern
	__uint64_t gCount = 0;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore;

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffffffff, true);
	BStringIterator myIter (myCol, patt);
	// mark first time to get all start positions
	myIter.MarkFragment();

	MMappedStorage myStore2;
	Column myCol2 (myStore2);
	Bitstring patt2(0xffffffff, true);
	BStringIterator myIter2 (myCol2, patt2);

	vector<unsigned int> v;
	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		unsigned int pattern = rand() % (1<<30) + 100;
		unsigned int count = rand() % 10000 + 100;
		for (int k = 0; k < count; k++) { // insert pattern count number of times
			gCount++;
			Bitstring b(pattern, true);
			myIter.Insert(b);
			myIter.Advance();
			myIter2.Insert(b);
			myIter2.Advance();
			v.push_back(pattern);
			assert(pattern);
			if (gCount % 4096 == 0) {
				myIter.MarkFragment();
				vMark.push_back(v);
				v.clear();
			}
		}
	}
	//myIter.Done(myCol);
	myIter.Done();
	myIter2.Done();

	int FRAG_RANGE = 4;

	BStringIterator myIter_i2;
	myIter_i2.swap(myIter2);

	for (int k = 0; k < vMark.size(); k+=FRAG_RANGE) {
		BStringIterator myIter_i;
		myIter_i.swap(myIter);
		myIter_i.SetFragmentRange(k,k+FRAG_RANGE-1);

		int count = 0;
		int min = k+FRAG_RANGE;
		if (min > vMark.size())
			min = vMark.size();
		for (int j = k; j < min; j++) {
			count += vMark[j].size();
		}
		//printf("\n iterNumTuples = %d, markCnt = %d", myIter_i.GetNumTuples(), count);
		assert(myIter_i.GetNumTuples() == count);

		// create a vector of a range
		vector<unsigned int> val;
		for (int j = k; j < min; j++)
			for (int c = 0; c < vMark[j].size(); c++)
				val.push_back(vMark[j][c]);

		int i = 0;
		while (!myIter_i.AtEndOfColumn()) {
			Bitstring b = myIter_i.GetCurrent();
			Bitstring b2 = myIter_i2.GetCurrent();
			assert(val[i] != 0);
			//printf("\n i = %d, vMark[%d]size = %d", i, k, vMark[k].size());
			assert(i < val.size());
			//printf("\n range = (%d,%d), patt = %ld, val = %ld", k, k+FRAG_RANGE-1, b.GetInt64(), val[i]);
			assert (b2.GetInt64() == val[i]);
			assert(b.GetInt64() == b2.GetInt64());
			assert (b.GetInt64() == val[i]);
			myIter_i.Advance ();
			myIter_i2.Advance ();
			i++;
		}
		myIter_i.swap(myIter);
	}

/*
	BStringIterator myIter_i;
	myIter_i.swap(myIter);
	BStringIterator myIter_i2;
	myIter_i2.swap(myIter2);
	while (!myIter_i2.AtEndOfColumn()) {
		Bitstring b = myIter_i2.GetCurrent();
		Bitstring b2 = myIter_i2.GetCurrent();
		//printf("\n pattgood = %ld, pattbad = %ld", b.GetInt64(), b2.GetInt64());
		assert(b.GetInt64() == b2.GetInt64());
		myIter_i.Advance ();
		myIter_i2.Advance ();
	}
*/
}

void fragment_check_column () {

	// for sanity comparisions
	vector<unsigned int> vData;
	vector<vector<unsigned int> > vMark;

	// global count of pattern
	__uint64_t gCount = 0;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore;

	// put the uncompressed storage into a column
	Column myCol (myStore);

	ColumnIterator<unsigned int> myIter (myCol);
	// mark first time to get all start positions
	myIter.MarkFragment();

	vector<unsigned int> v;
	for (int i = 1; i < 10000000; i++) {
		gCount++;
		myIter.Insert(i);
		myIter.Advance();
		v.push_back(i);
		if (gCount % (1<<16) == 0) {
			myIter.MarkFragment();
			vMark.push_back(v);
			v.clear();
		}
	}
	myIter.Done(myCol);

	for (int k = 0; k+3 < vMark.size(); k+=4) {

		ColumnIterator<unsigned int> myIter_i(myCol, k, k+3);
		int i = 0;
		for (; i < vMark[k].size(); i++) {
			assert(vMark[k][i] != 0);
			//printf("\n patt = %ld, vMark = %ld", myIter_i.GetCurrent(), vMark[k][i]);
			assert (myIter_i.GetCurrent() == vMark[k][i]);
			myIter_i.Advance ();
		}

		myIter_i.Done(myCol);
	}
}

void memleak_check_column () {

	int i = 0;
		TwoWayList<Chunk> twl;
	while (i != 500) {
	Chunk chk;
	for (int i = 0; i < 5; i++) {
		MMappedStorage myStore;
		Column myCol (myStore);
		ColumnIterator<unsigned int> myIter (myCol);
		for (int i = 1; i < 10000000; i++) {
			myIter.Insert(i);
			myIter.Advance();
		}
		myIter.Done(myCol);
		chk.SwapColumn(myCol, i);
	}
		Chunk copy;
		copy.copy(chk);
		twl.Insert(copy);
		twl.Clear();
	}
	
}

void single_pattern_read_check() {

	MMappedStorage store;
	Column myCol(store);

	int pattern = 0x1;
	unsigned long times = 10000000;

	// A hack is inserted in Bitstring to take pattern using a boolean variable.
	Bitstring patt(pattern, true);
	BStringIterator myIter (myCol, patt);

	for (unsigned long i = 0; i < times; i++) {
			myIter.Insert(patt);
			myIter.Advance();
	}
	myIter.Done(myCol);
	//myIter.Done();

	assert (times == myIter.GetNumTuples());

	//BStringIterator myIter_i (myCol, patt);
	BStringIterator myIter_i(myCol, times);
	//myIter_i.ConvertFromCol (myCol);
	myIter_i.swap(myIter);
	assert(myIter_i.GetNumTuples() == times);
	//for (unsigned long i = 0; i < times; i++) {
	for (unsigned long i = 0; i < myIter_i.GetNumTuples(); i++) {
		Bitstring b = myIter_i.GetCurrent();
		assert (b.GetInt64() == pattern);
		myIter_i.Advance ();
	}
	myIter_i.Done(myCol);
}

void single_pattern_read_check_init() {

	MMappedStorage store;
	Column myCol(store);

	int pattern = 0x1;
	unsigned long times = 10000000;

	Bitstring patt(pattern, true);
	BStringIterator myIter (myCol, patt, times);
	assert (times == myIter.GetNumTuples());
	//myIter.Done(myCol);
	myIter.Done();

	//Bitstring patt_i(0xffffffffffffffff, true);
	//BStringIterator myIter_i (myCol, patt_i);
	BStringIterator myIter_i;
	myIter_i.swap(myIter);
	assert(myIter_i.GetNumTuples() == times);
	//for (unsigned long i = 0; i < times; i++) {
	for (unsigned long i = 0; i < myIter_i.GetNumTuples(); i++) {
		Bitstring b = myIter_i.GetCurrent();
		assert (b.GetInt64() == pattern);
		myIter_i.Advance ();
	}
	myIter_i.Done(myCol);
}


void raw_check_16bit() {
	// get some data.
	int *data = (int *) mmap_alloc (MEMORY_SIZE);

	// This will iterate
	unsigned int* p = (unsigned int*)data;

	// for sanity comparisions
	vector<unsigned short> vData;
	vector<int> vCount;

	// global count of pattern
	int gCount = 0;

	//unsigned int* startHeader = p;

	// Write the header
	//p++; *p++ = 4;

	// Now write some random patterns
	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		int pattern = rand() % (1<<14) + 100;
		int count = rand() % 10000 + 100;
		gCount += count;
		for (int k = 0; k < count; k++) {
			vData.push_back(pattern);
			vCount.push_back(count);
		}
		pattern = pattern | ((count-1) << 16);
		*p = pattern;
		p++;
	}
	
	// once done writing, store the count
	//*startHeader = gCount;

	// put it into the storage
	MMappedStorage myStore (data, MEMORY_SIZE, 0);

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffff, true);
	BStringIterator myIter (myCol, patt);
	//assert(myIter.GetNumTuples() == gCount);

	for (int i = 0; i < gCount; i++) {
		Bitstring b = myIter.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter.Advance ();
	}
	myIter.Done(myCol);
}

void raw_check_32bit() {
	// get some data.
	int *data = (int *) mmap_alloc (MEMORY_SIZE);

	// This will iterate
	unsigned int* p = (unsigned int*)data;

	// for sanity comparisions
	vector<unsigned int> vData;
	vector<unsigned int> vCount;

	// global count of pattern
	__uint64_t gCount = 0;

	//unsigned int* startHeader = p;

	// Write the header
	//p++; *p++ = 8;

	// Now write some random patterns
	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		__uint64_t pattern = rand() % (1<<30) + 100;
		unsigned int count = rand() % 10000 + 100;
		gCount += count;
		for (unsigned int k = 0; k < count; k++) {
			vData.push_back((unsigned int)pattern);
			vCount.push_back(count);
		}
		pattern = pattern | (((__uint64_t)count-1) << 32);
		*((__uint64_t*)p) = pattern;
		p+=2;
	}

	// once done writing, store the count
	//*startHeader = gCount;

	// put it into the storage
	MMappedStorage myStore (data, MEMORY_SIZE, 0);

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffffffff, true);
	BStringIterator myIter (myCol, patt);
	//assert(myIter.GetNumTuples() == gCount);

	for (int i = 0; i < gCount; i++) {
		Bitstring b = myIter.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter.Advance ();
	}
	myIter.Done(myCol);
}

void raw_check_64bit() {
	// get some data.
	int *data = (int *) mmap_alloc (MEMORY_SIZE);

	// This will iterate
	unsigned int* p = (unsigned int*)data;

	// for sanity comparisions
	vector<__uint64_t> vData;
	vector<unsigned int> vCount;

	//unsigned int* startHeader = p;

	// global count of pattern
	__uint64_t gCount = 0;

	// Write the header
	//p++; *p++ = 12;

	// Now write some random patterns
	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		__uint64_t pattern = rand() % (((__uint64_t)1)<<62) + 100;
		unsigned int count = rand() % 10000 + 100;
		gCount += count;
		for (unsigned int k = 0; k < count; k++) {
			vData.push_back(pattern);
			vCount.push_back(count);
		}
		*((__uint64_t*)p) = pattern;
		p+=2;
		*p = count-1;
		p++;
	}

	// once done writing, store the count
	//*startHeader = gCount;

	// put it into the storage
	MMappedStorage myStore (data, MEMORY_SIZE, 0);

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffffffffffffffff, true);
	BStringIterator myIter (myCol, patt);
	//assert(myIter.GetNumTuples() == gCount);

	for (int i = 0; i < gCount; i++) {
		Bitstring b = myIter.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter.Advance ();
	}
	myIter.Done(myCol);
}

void insert_check_16bit() {

	// for sanity comparisions
	vector<unsigned short> vData;
	vector<unsigned short> vCount;

	// global count of pattern
	__uint64_t gCount = 0;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore;

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffff, true);
	BStringIterator myIter (myCol, patt);

	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		unsigned short pattern = rand() % (1<<14) + 100;
		unsigned int count = rand() % 10000 + 100;
		vCount.push_back(count);
		for (int k = 0; k < count; k++) { // insert pattern count number of times
			gCount++;
			Bitstring b(pattern, true);
			myIter.Insert(b);
			myIter.Advance();
			vData.push_back(pattern);
		}
	}
	assert(myIter.GetNumTuples() == gCount);
	//myIter.Done(myCol);
	myIter.Done();
	assert(myIter.GetNumTuples() == gCount);

	//Bitstring patt_i(0xffff, true);
	//BStringIterator myIter_i (myCol, patt_i);
	BStringIterator myIter_i;
	myIter_i.swap(myIter);
	assert(myIter_i.GetNumTuples() == gCount);

	for (int i = 0; i < gCount; i++) {
		Bitstring b = myIter_i.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter_i.Advance ();
	}
	myIter_i.Done(myCol);
}

void insert_check_32bit() {

	// for sanity comparisions
	vector<unsigned int> vData;
	vector<unsigned int> vCount;

	// global count of pattern
	__uint64_t gCount = 0;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore;

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffffffff, true);
	BStringIterator myIter (myCol, patt);

	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		unsigned int pattern = rand() % (1<<30) + 100;
		unsigned int count = rand() % 10000 + 100;
		vCount.push_back(count);
		for (int k = 0; k < count; k++) { // insert pattern count number of times
			gCount++;
			Bitstring b(pattern, true);
			myIter.Insert(b);
			myIter.Advance();
			vData.push_back(pattern);
		}
	}
	//myIter.Done(myCol);
	myIter.Done();

	//Bitstring patt_i(0xffffffff, true);
	//BStringIterator myIter_i (myCol, patt_i);
	BStringIterator myIter_i;
	myIter_i.swap(myIter);
	assert(myIter_i.GetNumTuples() == gCount);

	int i = 0;
	for (; i < gCount/2; i++) {
		Bitstring b = myIter_i.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter_i.Advance ();
	}

	BStringIterator mySwapIter;
	mySwapIter.swap(myIter_i);
	for (i = gCount/2; i < gCount; i++) {
		Bitstring b = mySwapIter.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		mySwapIter.Advance ();
	}
	mySwapIter.Done(myCol);
}

void insert_check_64bit() {

	// for sanity comparisions
	vector<__uint64_t> vData;
	vector<unsigned int> vCount;

	// global count of pattern
	__uint64_t gCount = 0;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore;

	// put the uncompressed storage into a column
	Column myCol (myStore);

	Bitstring patt(0xffffffffffffffff, true);
	BStringIterator myIter (myCol, patt);

	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		__uint64_t pattern = rand() % (((__uint64_t)1)<<62) + 100;
		unsigned int count = rand() % 10000 + 100;
		vCount.push_back(count);
		for (int k = 0; k < count; k++) { // insert pattern count number of times
			gCount++;
			Bitstring b(pattern, true);
			myIter.Insert(b);
			myIter.Advance();
			vData.push_back(pattern);
		}
	}
	//myIter.Done(myCol);
	myIter.Done();

	//Bitstring patt_i(0xffffffffffffffff, true);
	//BStringIterator myIter_i (myCol, patt_i);
	BStringIterator myIter_i;
	myIter_i.swap(myIter);
	assert(myIter_i.GetNumTuples() == gCount);

	for (int i = 0; i < gCount; i++) {
		Bitstring b = myIter_i.GetCurrent();
		assert(vData[i] != 0);
		assert (b.GetInt64() == vData[i]);
		myIter_i.Advance ();
	}
	myIter_i.Done(myCol);
}

void blank_iter_check () {

	BStringIterator myIter;
	for (int i = 0; i < TOTAL_PATTERNS; i++) {
		const Bitstring& b = myIter.GetCurrent();
		Bitstring b1 = myIter.GetCurrent();
		myIter.Advance ();
	}
	Column col;
	myIter.Done (col);
}

int main () {
	memleak_check_column();
	return 0;
	fragment_check_bitstring();
	fragment_check_column();
	cout << "\nChecking single pattern read";
	single_pattern_read_check();
	single_pattern_read_check_init();
	cout << "\nSingle pattern read successful";
	cout << "\nStarting 16 bit raw pattern check";
	raw_check_16bit();
	cout << "\n 16 bit pattern check is success";
	cout << "\nStarting 32 bit raw pattern check";
	raw_check_32bit();
	cout << "\n 32 bit pattern check is success";
	cout << "\nStarting 64 bit raw pattern check";
	raw_check_64bit();
	cout << "\n 64 bit pattern check is success";
	cout << "\nStarting 16 bit insert pattern check";
	insert_check_16bit();
	cout << "\n 16 bit pattern check after insert is success";
	cout << "\nStarting 32 bit insert pattern check";
	insert_check_32bit();
	cout << "\n 32 bit pattern check after insert is success";
	cout << "\nStarting 64 bit insert pattern check";
	insert_check_64bit();
	cout << "\n 64 bit pattern check after insert is success";
	cout << "\nStarting blank iterator check";
	blank_iter_check();
	cout << "\n blank iterator check success\n";

	return 0;
}
