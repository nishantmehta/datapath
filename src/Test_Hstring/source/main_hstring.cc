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

#include "Bitstring.h"
#include <algorithm>
#include <map>
#include "HString.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "CompressibleStorageUnit.h"
//#include "CHRISINT.h"
#include "RawStorageDesc.h"
#include "HStringIterator.h"
#include <ctime>
#include<stdio.h>

#include "WorkDescription.h"
#include "ExecEngineData.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "BString.h"
#include "BStringIterator.h"
#include "ExecEngine.h"
#include "DataPathGraph.h"
#include "WayPointConfigureData.h"
#include "CPUWorkerPool.h"
#include "Coordinator.h"
#include "Constants.h"
#include "Logging.h"
#include "MmapAllocator.h"


#include <unistd.h>
#include <dlfcn.h>

using namespace std;

#include <iostream>

#define MEMORY_SIZE 100000000

#define TOTAL_STRINGS 500000

#define MAX_STR_SIZE 150

#define STEP_SIZE 1000

ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (16);

// global coordinator. Used by waypoints that produce external
// messages such as Print
/* 0 is a bogus parameter to avoid default constructor */

Coordinator globalCoordinator(0);

/***
			This tests the compressed storage and HString classes. We store random size strings for this test.
			And we store frequent strings in dictionary
***/

void iteration_function (Column& myCol, vector<__uint64_t>& vHash, vector<__uint64_t>& vStrlen, vector<string>& vString,
													string frequent, string non_frequent)
{
/*
	// choose one frequent and non frequent string from somewhere near the end
	string frequent;
	string non_frequent;
	for (int i = TOTAL_STRINGS/2 + TOTAL_STRINGS%(TOTAL_STRINGS/4) ; i < TOTAL_STRINGS; i++) {
		if (frequent.size() == 0 && vDict[i] == true)
			frequent = vString[i];
		if (non_frequent.size() == 0 && vDict[i] == false)
			non_frequent = vString[i];
	}
*/

	HString h_freq(frequent.c_str());
	HString h_nonfreq(non_frequent.c_str());
	cout << "\nFrequent str = " << frequent.c_str();
	cout << "\nNon frequent str = " << non_frequent.c_str();

	cout << "\nStarting sanity check";

	// Iterate and check sanity of all data
	HStringIterator myIter (myCol);

	for (int i = 0; i < vHash.size()-1; i++) {
		HString h = myIter.GetCurrent();
		if (h == h_freq) cout << "\nFrequent string found";
		if (h == h_nonfreq) cout << "\nNon frequent string found";
		//		printf("\n  i = %d %lx     %lx", i, h.GetHashValue(), vHash[i]);
        if( h.GetHashValue() != vHash[i] )
            cout << "\nHash mismatch: " << h.GetHashValue() << " vs " << vHash[i];
		assert (h.GetHashValue() == vHash[i]);
		if (h.IsInDictionary() == false) {
			assert(vStrlen[i] == h.GetStrLength());
			assert(strcmp(vString[i].c_str(), h.GetString()) == 0);
		}
		myIter.Advance ();
	}
	myIter.Done(myCol);

	cout << "\nFinished sanity check";

	// Performance testing using simple string comparision method. Also add iteration code
	// to get data from column, even if not needed, just to compare time
	cout << "\n\nStarting raw string comparision method";
	HStringIterator it1 (myCol);
	clock_t t1 = clock();
	for (int i = 0; i < vHash.size()-1; i++) {
		HString h = it1.GetCurrent();
		//if (strcmp(vString[i].c_str(), non_frequent.c_str()) == 0)
		//	cout << "\nNon frequent string found";
		if (strcmp(vString[i].c_str(), frequent.c_str()) == 0)
			cout << "\nFrequent string found";
		it1.Advance();
	}
	it1.Done(myCol);
	clock_t t2 = clock();
	cout << "\nClock ticks taken using string comparision method = " << t2-t1;

	// Performance testing using HString comparision method, it is observed that likely
	// branch in HString operator== has improved the performance many times. If the same
	// branch is made unlikely, performance degradation is 5x
	HStringIterator it (myCol);
	clock_t c1 = clock();
	cout << "\n\nStarting HString comparision";
	for (int i = 0; i < vHash.size()-1; i++) {
		HString h = it.GetCurrent();
		if (h == h_freq) cout << "\nFrequent string found";
		// while benchmarking, below line must be commented. Otherwise it will be string comparision all the time
		//if (h == h_nonfreq) cout << "\nNon frequent string found";
		it.Advance();
	}
	it.Done(myCol);
	clock_t c2 = clock();
	cout << "\nClock ticks taken using HString comparision method = " << c2-c1 << endl;
}


void test_partialcopy() {

	// for sanity comparisions
	vector<__uint64_t> vHash_i;
	vector<__uint64_t> vStrlen_i;
	vector<string> vString_i;
	string freq;
	string non_freq;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore_i;

	// put the uncompressed storage into a column
	Column myCol_i (myStore_i);

	HStringIterator myIter_i (myCol_i);

	for (int i = 0; i < TOTAL_STRINGS; i++) {
		int l = rand() % MAX_STR_SIZE + 10;
		string s;
		while (l-- > 0)
			s += (char)(rand() % 25 + 65);
		// put every 100th string in non frequent case, not in dict
		bool isDict = (i % 100 != 0);
		HString h(s.c_str());
		if (isDict && !h.IsInDictionary()) {
			HString::AddEntryInDictionary(h); // 62nd bit of hash will be set high, hence modified
			non_freq = s.c_str();
		}
		else if (!isDict) {
			freq = s.c_str();
		}
		myIter_i.Insert(h);
		myIter_i.Advance();
		vHash_i.push_back(h.GetHashValue());
		vString_i.push_back(s.c_str());
		vStrlen_i.push_back(s.size()+1);
	}
	myIter_i.Done(myCol_i);

	iteration_function (myCol_i, vHash_i, vStrlen_i, vString_i, freq, non_freq);

	// Iterate and check sanity of all data
	HStringIterator myIter (myCol_i);

	for (int i = 0; i < vHash_i.size()/2; i++) {
		HString h = myIter.GetCurrent();
		assert (h.GetHashValue() == vHash_i[i]);
		if (h.IsInDictionary() == false) {
			assert(vStrlen_i[i] == h.GetStrLength());
			assert(strcmp(vString_i[i].c_str(), h.GetString()) == 0);
		}
		myIter.Advance ();
	}

	// Create a blank iterator and create deep copy
	HStringIterator myIter_o;
	myIter_o.CreateDeepCopy(myIter);

	for (int i = vHash_i.size()/2; i < vHash_i.size()-1; i++) {
		HString h = myIter.GetCurrent();
		assert (h.GetHashValue() == vHash_i[i]);
		if (h.IsInDictionary() == false) {
			assert(vStrlen_i[i] == h.GetStrLength());
			assert(strcmp(vString_i[i].c_str(), h.GetString()) == 0);
		}
		myIter_o.Insert(h);
		myIter_o.Advance();
		myIter.Advance ();
	}
	myIter.Done(myCol_i);
	// Create empty column and swap the actual column out
	Column myCol_o;
	myIter_o.Done(myCol_o);

	// Now check the sanity of output data recently written
	HStringIterator it (myCol_o);
	for (int i = 0; i < vHash_i.size()-1; i++) {
		HString h = it.GetCurrent();
		assert (h.GetHashValue() == vHash_i[i]);
		if (h.IsInDictionary() == false) {
			assert(vStrlen_i[i] == h.GetStrLength());
			assert(strcmp(vString_i[i].c_str(), h.GetString()) == 0);
		}
		it.Advance ();
	}
	it.Done(myCol_o);
}

void blank_iter_test () {

	HStringIterator myIter;
	for (int i = 0; i < 1000000; i++) {
		HString h = myIter.GetCurrent();
		myIter.Advance ();
	}
	Column col;
	myIter.Done (col);
}

int main () {

	cout << "\n Testing partial copy \n";
	test_partialcopy();
	cout << "\n Partial copy check success \n";
	cout << "\n Blank iterator test \n";
	blank_iter_test();
	cout << "\n Blank iterator test success \n";

	// Initialize the dictionary, for now, just rely on self created dict
	//HString::InitializeDictionary();

	// get some data. Only use CompressibleStorageUnit for this test
	int *data = (int *) mmap_alloc (MEMORY_SIZE, 1);

	// This will iterate
	__uint64_t* p = (__uint64_t*)data;

	// for sanity comparisions
	vector<__uint64_t> vHash;
	vector<__uint64_t> vStrlen;
	vector<string> vString;

	string freq;
	string non_freq;

	//HString::SaveDictionary();

	// Now write some random length strings
	for (int i = 0; i < TOTAL_STRINGS; i++) {
		int l = rand() % MAX_STR_SIZE + 10;
		string s;
		while (l-- > 0)
			s += (char)(rand() % 25 + 65);
		// put every 100th string in non frequent case, not in dict
		bool isDict = (i % 100 != 0);
		HString h(s.c_str());
		if (isDict && !h.IsInDictionary()) {
			HString::AddEntryInDictionary(h);
			non_freq = s.c_str();
		}
		else if (!isDict){
			freq = s.c_str();
		}
		*p = h.GetHashValue();
		vHash.push_back(*p);
		vString.push_back(s.c_str());
		vStrlen.push_back(s.size()+1);
		p++;
		if (!isDict && !h.IsInDictionary()) { // store string size and string if not in dict
			*p = s.size() + 1;
			p++;
			strcpy((char*)p, s.c_str());
			// we should store 8 byte aligned since we are reading that way
			int byte_align_len = BYTE_ALIGN(s.size()+1);
			assert(byte_align_len % 8 == 0);
			p += byte_align_len/8;
		}
	}

	// put it into the storage
	MMappedStorage myStore (data, MEMORY_SIZE, 0);

	// put the uncompressed storage into a column
	Column myCol (myStore);

	iteration_function (myCol, vHash, vStrlen, vString, freq, non_freq);

	// ------------------------ Now do the testing using compression -------------------

	cout << "\n\n ********************** Testing using compression **************************** ";
	// put it into the column
	Column compressThisColumn (myStore);

	// compress it
	compressThisColumn.Compress(false);

	RawStorageList compressed_list;
	compressThisColumn.GetCompressed(compressed_list);
	int size_c = compressThisColumn.GetCompressedSizeBytes();
	cout << "\nTotal compressed size = " << size_c;
	printf("\nCompression ratio = %f", (float)size_c / (float)MEMORY_SIZE);

	// iterate the compressed list
	compressed_list.MoveToStart();
	cout << "\nLength of compressed list = " << compressed_list.RightLength();
	// We got lot of compressed guys, iterate on all. For now, we are getting only one
	// depending on our compression scheme.
	while (compressed_list.RightLength()) {
		RawStorageUnit unit;
		unit = compressed_list.Current();
		cout << "\nCompressed size in bytes = " << unit.sizeInBytes;
		// create storage for compressed data
		MMappedStorage CStore (unit.data, MEMORY_SIZE, unit.sizeInBytes);
		Column col (CStore);

	//  ----- Now perform same testing which was done using uncompressed data ------
		iteration_function (col, vHash, vStrlen, vString, freq, non_freq);
		compressed_list.Advance();
	}

	cout << "\n\n*************************** Testing using Insert function ***************************** ";

	// for sanity comparisions
	vector<__uint64_t> vHash_i;
	vector<__uint64_t> vStrlen_i;
	vector<string> vString_i;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore_i;

	// put the uncompressed storage into a column
	Column myCol_i (myStore_i);

	HStringIterator myIter_i (myCol_i);

	for (int i = 0; i < TOTAL_STRINGS; i++) {
		int l = rand() % MAX_STR_SIZE + 10;
		string s;
		while (l-- > 0)
			s += (char)(rand() % 25 + 65);
		// put every 100th string in non frequent case, not in dict
		bool isDict = (i % 100 != 0);
		HString h(s.c_str());
		if (isDict && !h.IsInDictionary()) {
			HString::AddEntryInDictionary(h); // 62nd bit of hash will be set high, hence modified
			non_freq = s.c_str();
		}
		else if (!isDict) {
			freq = s.c_str();
		}
		myIter_i.Insert(h);
		myIter_i.Advance();
		vHash_i.push_back(h.GetHashValue());
		vString_i.push_back(s.c_str());
		vStrlen_i.push_back(s.size()+1);
	}
	myIter_i.Done(myCol_i);

	iteration_function (myCol_i, vHash_i, vStrlen_i, vString_i, freq, non_freq);

	// ------------------------ Now do the testing using compression after Insert -------------------

	cout << "\n\n ********************** Testing using compression after insert **************************** ";
	// put it into the column
	//Column compressThisColumn_2 (myStore_i);

	// compress it
	//compressThisColumn_2.Compress(false);
	myCol_i.Compress(false);

	RawStorageList compressed_list_2;
	//compressThisColumn_2.GetCompressed(compressed_list_2);
	myCol_i.GetCompressed(compressed_list_2);
	//int size_c_2 = compressThisColumn_2.GetCompressedSizeBytes();
	int size_c_2 = myCol_i.GetCompressedSizeBytes();
	cout << "\nTotal compressed size = " << size_c_2;
	printf("\nCompression ratio = %f", (float)size_c_2 / (float)MEMORY_SIZE);

	// iterate the compressed list
	compressed_list_2.MoveToStart();
	cout << "\nLength of compressed list = " << compressed_list_2.RightLength();
	// We got lot of compressed guys, iterate on all. For now, we are getting only one
	// depending on our compression scheme.
	while (compressed_list_2.RightLength()) {
		RawStorageUnit unit;
		unit = compressed_list_2.Current();
		cout << "\nCompressed size in bytes = " << unit.sizeInBytes;
		// create storage for compressed data
		MMappedStorage CStore (unit.data, MEMORY_SIZE, unit.sizeInBytes);
		Column col (CStore);

	//  ----- Now perform same testing which was done using uncompressed data ------
		iteration_function (col, vHash_i, vStrlen_i, vString_i, freq, non_freq);
		compressed_list_2.Advance();
	}

	cout << "\n\n*************************** Testing using Insert function ***************************** ";

	// for sanity comparisions
	vector<__uint64_t> vHash_i2;
	vector<__uint64_t> vStrlen_i2;
	vector<string> vString_i2;

	// Create brand new storage, but NEVER NEVER pass a new mmap_alloc stoarge to it
	// It will create its own while inserting. If you pass new storage, make sure
	// its not big, else it will keep on doing copy operation of the storage.
	MMappedStorage myStore_i2;

	// put the uncompressed storage into a column
	Column myCol_i2 (myStore_i2);

	HStringIterator myIter_i2 (myCol_i2);

	for (int i = 0; i < TOTAL_STRINGS / 1000; i++) {
		int l = rand() % MAX_STR_SIZE + 10;
		string s;
		while (l-- > 0)
			s += (char)(rand() % 25 + 65);
		// insert some strings many number of times, internally we will assume it is
		// frequent and has to be added into dictionary
		for (int j = 0; j < 500; j++) {
			HString h(s.c_str());
			myIter_i2.Insert(h);
			myIter_i2.Advance();
			vHash_i2.push_back(h.GetHashValue());
			vStrlen_i2.push_back(s.size() + 1);
			vString_i2.push_back(s.c_str());
			freq = s.c_str();
		}

		// Now also add some infrequent strings
		for (int j = 0; j < 50; j++) {
			l = rand() % MAX_STR_SIZE + 10;
			string s;
			while (l-- > 0)
				s += (char)(rand() % 25 + 65);
			HString h(s.c_str());
			myIter_i2.Insert(h);
			myIter_i2.Advance();
			vHash_i2.push_back(h.GetHashValue());
			vStrlen_i2.push_back(s.size() + 1);
			vString_i2.push_back(s.c_str());
			non_freq = s.c_str();
		}

	}
	myIter_i2.Done(myCol_i2);

	iteration_function (myCol_i2, vHash_i2, vStrlen_i2, vString_i2, freq, non_freq);
}
