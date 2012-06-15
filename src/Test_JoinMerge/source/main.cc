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
#include "BStringIterator.h" // has to be first since compilation
                             // braks otherwise. God knows why
#include "ExecEngine.h"
#include "DataPathGraph.h"
#include "WayPointConfigureData.h"
#include "CPUWorkerPool.h"
#include "Coordinator.h"
#include "Constants.h"
#include "Logging.h"
#include "Bitstring.h"

#include "WorkDescription.h"
#include "ExecEngineData.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "BString.h"
#include "BStringIterator.h"

#include <unistd.h>
#include <dlfcn.h>

#define LHS_COL_LENGTH 5000000
#define RHS_COL_LENGTH 4000000

#define NUM_CHUNKS_LHS 5
#define NUM_COLS_LHS 3
#define NUM_CHUNKS_RHS 5
#define NUM_COLS_RHS 3
//#include "JoinMergeWorkFuncs"

// all of the event global event processor classes go here... note that since I
// just have a "pretend" disk setup (where chunks are created entirely using CPU)
// I use CPU workers to handle the "disk"... obviously, this will change when the
// "real" disk is added in
ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (16);

// global coordinator. Used by waypoints that produce external
// messages such as Print
/* 0 is a bogus parameter to avoid default constructor */

Coordinator globalCoordinator(0);

extern int JoinMergeFunc_WPJoin (WorkDescription &workDescription, ExecEngineData &result);

int main () {

	// Let's create LHS chunks where each chunk contains columns considering first column
	// for every chunk has hash value
	ContainerOfChunks lhsChunks;
	for (int i = 0; i < NUM_CHUNKS_LHS; i++) { // chunks
		Chunk chunk;
		for (int j = 0; j < NUM_COLS_LHS; j++) { // columns
			MMappedStorage myStore;
			Column myCol (myStore);
			ColumnIterator <int> myIter (myCol);
			// We test using first column starting 100 to LHS_COL_LENGTH
			for (int k = i*j*100, p = 0; p < LHS_COL_LENGTH; k++, p++) { // entries in a column
				myIter.Insert (k);
				//if (p != LHS_COL_LENGTH-1)
					myIter.Advance ();
			}
			myIter.Done (myCol);
			chunk.SwapColumn (myCol, j); // column goes in
		}
		// put BitMap in
		MMappedStorage myStore;
		Column myCol (myStore);
		Bitstring pattern(0xffffffff, true);
		BStringIterator myIter (myCol, pattern, LHS_COL_LENGTH);
		//myIter.Done(myCol);
		myIter.Done();

		//chunk.SwapBitmap (myCol);
		chunk.SwapBitmap (myIter);
		lhsChunks.Append (chunk);
	}

	ContainerOfChunks rhsChunks;
	for (int i = 0; i < NUM_CHUNKS_RHS; i++) { // chunks
		Chunk chunk;
		for (int j = 0; j < NUM_COLS_RHS; j++) { // columns
			MMappedStorage myStore;
			Column myCol (myStore);
			ColumnIterator <int> myIter (myCol);
			// We test using first column starting 100+LHS_COL_LENGTH-1 to RHS_COL_LENGTH
			// Essentially we will have 5 chunks with same data and we have 5 entries in
			// RHS to match with one in LHS
			for (int k = (i*j)*100+LHS_COL_LENGTH-1, p = 0; p < RHS_COL_LENGTH; k++, p++) { // entries in a column
				myIter.Insert (k);
				//if (p != RHS_COL_LENGTH-1)
					myIter.Advance ();
			}
			myIter.Done (myCol);
			chunk.SwapColumn (myCol, j); // column goes in
		}
		// put BitMap in
		MMappedStorage myStore;
		Column myCol (myStore);
		Bitstring pattern(0xffffffff, true);
		BStringIterator myIter (myCol, pattern, RHS_COL_LENGTH);
		//myIter.Done (myCol);
		myIter.Done ();

		//chunk.SwapBitmap (myCol);
		chunk.SwapBitmap (myIter);
		rhsChunks.Append (chunk);
	}

/*
	int waypointID = 0;
	QueryExitContainer qe;
	JoinMergeWorkDescription workDesc (waypointID, qe, lhsChunks, rhsChunks);
	WorkFunc myFunc = GetWorkFunction (JoinMergeWorkFunc::type);
	WayPointID myID;
	HistoryList history;
	QueryExitContainer dest;
	ExecEngineData _data;
	HoppingDataMsg data (myID, dest, history, _data);
	myCPUWorkers.DoSomeWork (myID, data.get_lineage (), data.get_dest (), myToken, workDesc, myFunc);
*/


/*
	// Now generate and load the .so
	string dirPath(".");
	//string objects("JoinMergeWorkFuncs.o");
	//string call = "./generate.sh " + dirPath + " \"" + objects + '\"' + " 1>&2";
	string call = "./generate.sh ";
	int sysret = system(call.c_str());
	FATALIF(sysret != 0, "Unable to do the code generation on directory %s!", dirPath.c_str());
	string modFname = dirPath + "/Generated.so";
	void* module = NULL;
	module = dlopen(modFname.c_str(), RTLD_LAZY);
	if (module == NULL){
		FATAL("Unable to load generated code from file %s! \nThe error is %s\n", modFname.c_str(), dlerror());
	}
*/
	//wFunc = (WorkFunc) dlsym(module, "JoinMergeFunc_WPJoin");


	int waypointID = 0;
	QueryID query(0xffffffff, true);
	WayPointID exit("joinwp_test");
	QueryExit qe(query, exit);
	QueryExitContainer qec;
	qec.Append(qe);
	JoinMergeWorkDescription workDesc (waypointID, qec, lhsChunks, rhsChunks);
	ExecEngineData result;
	//JoinMergeFunc (workDesc, result);
	JoinMergeFunc_WPJoin (workDesc, result);
//	((WorkFunc) dlsym(module, "JoinMergeFunc_WPJoin"))(workDesc, result);
	ChunkContainer tempResult;
	tempResult.swap(result);
	Chunk& rezChk = tempResult.get_myChunk();
	//Column bitMap;
	//rezChk.SwapBitmap(bitMap);
	//BStringIterator BIter(bitMap);
	BStringIterator BIter;
	rezChk.SwapBitmap(BIter);
	vector<ColumnIterator<int> > ColVec;
	ColVec.resize(rezChk.GetNumOfColumns());
	for (int i = 0; i < rezChk.GetNumOfColumns(); i++) {
		Column Col;
		rezChk.SwapColumn (Col, i);
		ColumnIterator<int> iter(Col);
		ColVec[i].swap(iter);
	}

	while (!BIter.AtUnwrittenByte()) {
	//while (!ColVec[0].AtUnwrittenByte()) {
		//if (!(BIter.GetCurrent() == 0)) {
			for (int j = 0; j < rezChk.GetNumOfColumns(); j++) {
				cout << " " << ColVec[j].GetCurrent();
				ColVec[j].Advance();
			}
			cout << "\n";
		//}
		BIter.Advance();
	}
	return 0;
}
