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
#include "FileMetadata.h"
#include "DiskArray.h"

// Without these, wont compile
#include "CPUWorkerPool.h"
#include "ExecEngine.h"
#include "Coordinator.h"

using namespace std;

// Without these, wont compile
ExecEngine executionEngine;
CPUWorkerPool myCPUWorkers (NUM_EXEC_ENGINE_THREADS);
CPUWorkerPool myDiskWorkers (0);
Coordinator globalCoordinator(0);


#define NUM_TUPLES 100
#define NUM_COL 50

int main() {

	DiskArray::LoadDiskArray();

	// New relation created
	FileMetadata meta ("Metafile", NUM_COL); // relName, numCols

	for (int i = 0; i < 5; i++) {
		meta.startNewChunk (NUM_TUPLES, NUM_COL);
		for (int j = 0; j < NUM_COL; j++) {
			meta.addColumn (1,2,3,4,5,6);
		}
		meta.finishedChunk();
	}

	return 0;
}
