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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>

#include "Errors.h"
#include "DiskArrayImp.h"
#include "Constants.h"
#include "MmapAllocator.h"
#include "Hash.h"

// This is the striping hash function used to assign where a
// particular page is going to be located among its disk stripes.
// numPage represents the page number in the flat (logical) representation.
// numStripes represents the number of stripes.
// a and b are the two uniquely defined constants that determine the random patterns.

// NOTE: this function uses % so it is somewhat inefficient
//       since we use this function for pages that are large, we do not worry about
//       this inefficiency
//
// NOTE2: this striping function talks about the disk pages not MMap pages

DiskArrayImp::StripePair DiskArrayImp::StripingHash(off_t numPage) {
	StripePair ret;
	/**
		 The page that gets written is not scrambled but the stripe that writes it is.
		 The way the scrambling works is by computing another start for the mapping.
		 Ex. for the mapping (1 1) (2 2) (3 3) (4 4) with a shift of 2 becomes
		 (1 3) (2 4) (3 1) (4 2)

		 Only the shift is random and it is generated by a linear
		 method. This will essentially produce two-wise independent
		 shifts and still keep the probabiilty that a stripe gets used
		 (almost) uniform.

		 The almost is due to tiny imperfections that are less than
		 1/2^32 (or 2^64 on long).
	*/

	off_t lgPage = numPage >> meta.pageMultExp;
	off_t pgOff = numPage - (lgPage << meta.pageMultExp);

	off_t x = lgPage / meta.HDNo; // large page on stripe
	off_t y = lgPage % meta.HDNo; // stripe unshuffled

	ret.numPage = (x << meta.pageMultExp) + pgOff;
	ret.numStripe = ((PageHash(x+meta.stripeParam1)>>16)+y) % meta.HDNo;

	return ret;
}

DiskArrayImp::~DiskArrayImp() {
	//free the mutex
	pthread_mutex_destroy(&lock);

	//clear the statistics vector
	stats.clear();

	//kill the HDThreads event processors
	for (int i = 0; i < meta.HDNo; i++) {
		hds[i].Seppuku();
	}
	//and free the memory
	delete [] hds;
}

void DiskArrayImp::PrintStatistics(void){
	cerr << "TOTAL PAGES WRITTEN: " << totalPages << endl;
	cerr << "TOTAL MEMORY WRITTEN: " << (PAGES_TO_BYTES(totalPages) >> 20)  << "MB" << endl;
}

MESSAGE_HANDLER_DEFINITION_BEGIN(DiskArrayImp, ProcessDiskStatistics, DiskStatistics){
	// we got disk statistics from a disk. Compare it with the other
	// disks statistics and see how much lazier this disk is. If it is
	// overly lazy print warnings.
}MESSAGE_HANDLER_DEFINITION_END

 /** This message processes requests first in first out (modulo HDs finishing
		 in the same order; small jobs might finish slightly out of order)

		 Any controll on multiple requests has to be performed at the higher level.
 */
MESSAGE_HANDLER_DEFINITION_BEGIN(DiskArrayImp, DoDiskOperation, DiskOperation){
	// create a new distributed counter to detect when all HD threads have finished
	// The receiver of the message has to destroy it
	DistributedCounter* dCounter = new DistributedCounter(evProc.meta.HDNo);

	// we make a separate list for each harddrive
	DiskRequestDataContainer* hdRequests = new DiskRequestDataContainer[evProc.meta.HDNo];

	// we separate out the requests based on the stripe function
	for(msg.requests.MoveToStart(); !msg.requests.AtEnd(); msg.requests.Advance()){

		DiskRequestData& in = msg.requests.Current();

		// if the request has 0 size skip over it
		if (in.get_sizePages() == 0 )
			continue;

		// go through the range in the disk request and find (partial) large
		// pages corresponding to individual disks
		off_t currPage = in.get_startPage();
		char* currMem = (char*)in.get_memLoc(); // memory location where to read next request

		// we should not check that the request is aligned for writes
		// (will not be). We do not bother to check for reads either since
		// it is not essential


		off_t pgRem = in.get_sizePages(); // the number of pages remaining



//		cout << "DiskArray ID=" << msg.requestId << "page: " << currPage << " numPG:" << pgRem << " where:" << (void*)currMem << endl;

		evProc.totalPages+=pgRem;
		const off_t pageSizeLG = 1<<evProc.meta.pageMultExp;

		while (true){
			StripePair stripe = evProc.StripingHash(currPage);

			DiskRequestData out(stripe.numPage,
													(pgRem < pageSizeLG) ? pgRem : pageSizeLG,
													(void*)currMem);

			hdRequests[stripe.numStripe].Append(out);

			if (pgRem <= pageSizeLG)
				break; // done with requests

			pgRem-=pageSizeLG;
			currPage+=pageSizeLG; // if we wrote the last part we are out already
			currMem+=PAGES_TO_BYTES(pageSizeLG);

		}
	}

	FATALIF(!msg.requestor.IsValid(), "Requestor passed in DiskArray is not valid");

	// tell each disk to do its work
	// some of the requests are empty but that should be fine
	for (int i=0; i<evProc.meta.HDNo; i++){
		// have to copy the requestor otherwise swap will give us an empty one
		EventProcessor copy;
		copy.copy(msg.requestor);

		MegaJob_Factory(evProc.hds[i], msg.requestId, msg.operation, dCounter, copy, hdRequests[i]);
	}

	delete [] hdRequests;
}MESSAGE_HANDLER_DEFINITION_END

