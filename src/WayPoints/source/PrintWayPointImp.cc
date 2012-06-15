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

#include "PrintWayPointImp.h"
#include "CPUWorkerPool.h"
#include "EEExternMessages.h"
#include "EventProcessor.h"

extern EventProcessor globalCoordinator; /* who to contact when we
																					 fihish some queries */

PrintWayPointImp :: PrintWayPointImp () {PDEBUG ("PrintWayPointImp :: PrintWayPointImp ()");}
PrintWayPointImp :: ~PrintWayPointImp () {PDEBUG ("PrintWayPointImp :: PrintWayPointImp ()");}

void PrintWayPointImp :: TypeSpecificConfigure (WayPointConfigureData &configData) {
	PDEBUG ("PrintWayPointImp :: TypeSpecificConfigure ()");

	// load up the list where we will place done queries
	PrintConfigureData tempConfig;
	tempConfig.swap (configData);

	// tell people that we are ready to go with our queries... these messages will
	// eventually make it down to the table scan, which will begin producing data
	QueryExitContainer endingOnes;
	GetEndingQueryExits (endingOnes);
	QueryToFileInfoMap queriesInfo;
	queriesInfo.swap(tempConfig.get_queriesInfo());

	for (endingOnes.MoveToStart (); endingOnes.RightLength (); endingOnes.Advance ()) {

		// get the meta data
		QueryExit tempExit = endingOnes.Current (), tempExitCopy = endingOnes.Current ();
#ifdef DEBUG
		cout << "Print: Asking for ";
		tempExit.Print ();
		cout << " to start.\n";
#endif  // DEBUG
		
		// Open the fine and write the header for the query
		pair<string,string>& info = queriesInfo.Find(tempExit.query).GetData();
		string& fName = info.first;
		string& fHeader = info.second;
		FILE* str = fopen(fName.c_str(), "w");
		if ( str==NULL){
			printf( "File %s could not be opened in PRINT:", fName.c_str());
			perror(NULL);
			break;
		}
		fprintf(str, "%s", fHeader.c_str());
		FileObj fileObj(str);
		QueryID query=tempExit.query;
		streams.Insert(query, fileObj);

		WayPointID myID = GetID (), myIDCopy = GetID ();

		// create the actual notification fiest
		StartProducingMsg startProducing (myID, tempExit);		

		// now wrap it up in a hopping upstream message
		HoppingUpstreamMsg myMessage (myIDCopy, tempExitCopy, startProducing);
		SendHoppingUpstreamMsg (myMessage);
	}
}

void PrintWayPointImp :: DoneProducing (QueryExitContainer &whichOnes, HistoryList &history, 
																				int result, ExecEngineData& data) {
	PDEBUG ("PrintWayPointImp :: DoneProducing()");
	
	// send an ack message back down through the graph to let them know we are done
	SendAckMsg (whichOnes, history);
}

void PrintWayPointImp :: ProcessHoppingDownstreamMsg (HoppingDownstreamMsg &message) {
	PDEBUG ("PrintWayPointImp :: ProcessHoppingDownstreamMsg()");

	// see if we have a query done message
	if (CHECK_DATA_TYPE (message.get_msg (), QueryDoneMsg)) {

		// do the cast via a swap
		QueryDoneMsg temp;
		temp.swap (message.get_msg ());
		
		// send a message to the coordinator that we are done
		QueryExitContainer whichOnesC;
		whichOnesC.copy(temp.get_whichOnes());
		QueriesDoneMessage_Factory(globalCoordinator, whichOnesC);

		// Find the queries terminating here and send a message to the
		// global coordinator letting him know
		// we assume that all query exits with out waypoint id end here
		// TODO: get paranoid and check this with the myExits list
		QueryExitContainer endingOnes;
		FOREACH_TWL(qe, temp.get_whichOnes ()){
			if (qe.exit == GetID()){
				QueryExit tmp = qe;
				endingOnes.Append(tmp);
				
				// close the files
				QueryID tmp2 = qe.query;
				QueryID dummy;
				FileObj fileObj;
				streams.Remove(tmp2, dummy, fileObj);
				fclose(fileObj.GetData());
			}
		}END_FOREACH;
		
		// this was the source of a big bug!  The cleaner gets its query done
		// messages from the various joins as they finish
		//
		// now, send the fact that all of these queries are done to the cleaner
		//WayPointID cleaner ("Cleaner");
		//QueryDoneMsg allDone (GetID (), endingOnes);
		//DirectMsg toCleaner (cleaner, allDone);
		//SendDirectMsg (toCleaner);
		
	} else {
		FATAL ("Why did I get some hopping downstream message that was not a query done message?\n");
	}

}

void PrintWayPointImp :: ProcessHoppingDataMsg (HoppingDataMsg &data) {
	PDEBUG ("PrintWayPointImp :: ProcessHoppingDataMsg()");

	// request a work token to actually run the print
	GenericWorkToken returnVal;
	if (!RequestTokenImmediate (CPUWorkToken::type, returnVal)) {
		
		// if we do not get one, then we will just return a drop message to the sender
		SendDropMsg (data.get_dest (), data.get_lineage ());
		return;
	}

	// convert the token into the correct type
	CPUWorkToken myToken;
	myToken.swap (returnVal);

	// extract the chunk we need to print 
	ChunkContainer temp;
	data.get_data ().swap (temp);

	// fileDescriptors of all queries involved
	QueryToFileMap streamsOut;
	

	// create the work spec that we will actuall have executed
	QueryExitContainer whichOnes;
	whichOnes.copy (data.get_dest ());
	FOREACH_TWL(el, whichOnes){
	  QueryID query=el.query;
	  FileObj file; 
	  file.copy(streams.Find(query));
	  streamsOut.Insert(query, file);
	}END_FOREACH;

	PrintWorkDescription workDesc (whichOnes, streamsOut, temp.get_myChunk ());

	// now actually get the work taken care of!
	WayPointID myID;
	myID = GetID ();
	WorkFunc myFunc = GetWorkFunction (PrintWorkFunc::type);
	myCPUWorkers.DoSomeWork (myID, data.get_lineage (), data.get_dest (), myToken, workDesc, myFunc);
}

