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
#include<iostream>
#include "TileManager.h"
#include<algorithm>
#include<iterator>
#include "TableScanID.h"
#include "ChunkID.h"
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


using namespace std;

ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (16);

// global coordinator. Used by waypoints that produce external
// messages such as Print
/* 0 is a bogus parameter to avoid default constructor */

Coordinator globalCoordinator(0);

#define NUM_BUCKETS 10
#define TOTAL_PASS_OFDATA 2

static int globalID = 0;

int main () {

	// Chunk sent to disk
	ChunkMetaData metaData;
	TableScanID lhs("lhs");
	TableScanID rhs("rhs");

	for (int pass = 0; pass < TOTAL_PASS_OFDATA; pass++) {	
		for (int bucket = 0; bucket < NUM_BUCKETS; bucket++) {

			ChunkID chkLeft1(globalID, lhs);
			ChunkID chkRight1(globalID++, rhs);
			// Send pair in same bucket
			metaData.ChunkToDisk (bucket, chkLeft1, true);
			metaData.ChunkToDisk (bucket, chkRight1, false);
		}
	}

	metaData.DoneWriting();

	// This prints all tiles but dont remove the tiles from the metadata
	metaData.PrintAllTiles();

	// Keep getting tiles until they finish and print the tiles
	vector<ChunkID> l;
	vector<ChunkID> r;
	__uint64_t bucket;
	while (!metaData.IsEmpty()) {
		// This also removes tiles from metadata
		metaData.GetTile(bucket, l, r);
		cout << "\nBucket : " << bucket;
		cout << "\nList1";
		for (int i = 0; i < l.size(); i++) {
			IDInfo li;
			l[i].getInfo(li);
			//cout << " " << (li.getIDAsString()).c_str();
			cout << " " << (li.getName()).c_str();
		}
		cout << "\nList2";
		for (int j = 0; j < r.size(); j++) {
			IDInfo li;
			r[j].getInfo(li);
			//cout << " " << (li.getIDAsString()).c_str();
			cout << " " << (li.getName()).c_str();
		}
	}

	return 0;
}

