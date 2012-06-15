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

#include "SelectionWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Logging.h"


SelectionWayPointImp :: SelectionWayPointImp () {PDEBUG("SelectionWayPointImp :: SelectionWayPointImp ()");}
SelectionWayPointImp :: ~SelectionWayPointImp () {PDEBUG("SelectionWayPointImp :: SelectionWayPointImp ()");}

void SelectionWayPointImp :: ProcessHoppingDataMsg (HoppingDataMsg &data) {
	PDEBUG("SelectionWayPointImp :: ProcessHoppingDataMsg ()");

	// this function simply tries to get a worker to process the message
	GenericWorkToken returnVal;
	if (!RequestTokenImmediate (CPUWorkToken::type, returnVal)) {

		// if we do not get one, then we will just return a drop message to the sender
		SendDropMsg (data.get_dest (), data.get_lineage ());
		return;
	}

	// convert the token into the correct type
	CPUWorkToken myToken;
	myToken.swap (returnVal);


	ChunkID chunkID; // which chunk is this. Needed for debugging

	// if we have a chunk produced by a table waypoint log it
	data.get_lineage().MoveToStart();
	if (CHECK_DATA_TYPE(data.get_lineage().Current(), TableHistory)){
		TableHistory hLin;
		hLin.swap(data.get_lineage().Current());
		
		ChunkID& id = hLin.get_whichChunk();
		chunkID=id; // remember it

		TableScanInfo infoTS;
		id.getInfo(infoTS);

		LOG_ENTRY_P(2, "CHUNK %d of %s Processed by Selection",
								id.GetInt(), infoTS.getName().c_str()) ;
		
		// put it back;
		hLin.swap(data.get_lineage().Current());
	}

		// extract the chunk from the message
	ChunkContainer temp;
	data.get_data ().swap (temp);

	// create the work spec and get it done!
	QueryExitContainer myDestinations;
	myDestinations.copy (data.get_dest ());
	SelectionWorkDescription workDesc (chunkID, myDestinations, temp.get_myChunk ());

	// and send off the work request
	WayPointID myID;
	myID = GetID ();
	WorkFunc myFunc;
	myFunc = GetWorkFunction (SelectionWorkFunc::type);
	myCPUWorkers.DoSomeWork (myID, data.get_lineage (), data.get_dest (), myToken, workDesc, myFunc);

}
	
