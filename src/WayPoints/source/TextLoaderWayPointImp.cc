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
#include "TwoWayList.h"
#include "TextLoaderWayPointImp.h"
#include "CPUWorkerPool.h"
#include "Stl.h"
#include "Logging.h"
#include "DictionaryManager.h"

TextLoaderWayPointImp :: TextLoaderWayPointImp () {
	PDEBUG ("TextLoaderWayPointImp :: TextLoaderWayPointImp ()");
	numStreams = 0;
	tokensRequested = 0 ;
	chunksOut = 0;
	requestCnt = 0;
}

TextLoaderWayPointImp :: ~TextLoaderWayPointImp () {
	PDEBUG ("~TextLoaderWayPointImp :: TextLoaderWayPointImp ()");
	// nothing for now
}

void TextLoaderWayPointImp :: RequestTokens(){
	PDEBUG ("TextLoaderWayPointImp :: RequestTokens ()");
	// the goal is to have around either enough tokens to process all
	// streams or ar many as to keep total number of chunks
	// build+outForWritting at 2*numStreams

	// requests to keep all loaders bussy
	int noReq = tasks.Length() - tokensRequested;

	// is that too many?
	int dblBuf = 2*numStreams - chunksOut - tokensRequested; 
	if (noReq > dblBuf)
		noReq = dblBuf;

	// queue up some more work requests
	// one for each element of the list
	for (int i=0; i<noReq; i++) {
		RequestTokenDelayOK (CPUWorkToken::type);
		tokensRequested++;
	}		
}

void TextLoaderWayPointImp :: TypeSpecificConfigure (WayPointConfigureData &configData) {
	PDEBUG ("TextLoaderWayPointImp :: TypeSpecificConfigure ()");

	// first, extract the extra config info
	TextLoaderConfigureData tempConfig;
	tempConfig.swap (configData);
	
	// store query exits
	myExits.swap(tempConfig.get_queries());

	// invent a tID for ourselves
	IDInfo info;
	GetID().getInfo(info);
	name = info.getName();
	TableScanID newID(name);
	tID.swap(newID);

	// for each string in the input, create a task

	FOREACH_STL(file, tempConfig.get_files()){
		FILE* stream = fopen(file.c_str(), "r");
		if ( stream==NULL){
			printf( "File %s could not be opened in TextLoader:", file.c_str());
			perror(NULL);
			break;
		}
		
		// set the buffer at 1M so that we read faster
		// default buffer size is really bad
		setvbuf(stream, NULL /* system allocated */, _IOFBF, 1<<20 /* 1MB */);

		LOG_ENTRY_P(1, "Loader %s Started", file.c_str());
		
		HString::DictionaryWrapper localDictionary;
		HString::CreateLocalDictionary(localDictionary);

		TextLoaderDS task(stream, file, localDictionary);
		tasks.Append(task);
		numStreams++;

	}END_FOREACH

}
	
void TextLoaderWayPointImp :: RequestGranted (GenericWorkToken &returnVal) {
	PDEBUG ("TextLoaderWayPointImp :: RequestGranted ()");

	CPUWorkToken myToken;
	myToken.swap (returnVal);

	// do we even have some work we could do?
  // this shoudl probably not happen but better safe than sorry
	if (tasks.Length() == 0) {
		GiveBackToken (returnVal);
		return;
	}

	tokensRequested--;

	// get a task out
	TextLoaderDS task;
	// remove from the beginning so that we behave like a FIFO
	tasks.MoveToStart();
	tasks.Remove(task);

	// set up the lineage... since the chunk originates here, we create the object from scratch
	QueryExitContainer myOutputExits;
	myOutputExits.copy (myExits);
	QueryExitContainer myOutputExitsCopy;
	myOutputExitsCopy.copy (myExits);


	ChunkID cID(requestCnt, tID);
	requestCnt++; // next request in future
	TextLoaderHistory myHistory (GetID (), cID, task.get_file());
	HistoryList tempList;
	tempList.Insert (myHistory);

	// set up the work description
	TextLoaderWorkDescription workDesc (task.get_stream(), task.get_file(), 
					    task.get_localDictionary(), myOutputExits);

	// now, actually get the chunk sent out!  Again, note that here we use a CPU to do this...
	// but that is just because we have a toy table scan imp
	WorkFunc myFunc = GetWorkFunction (TextLoaderWorkFunc::type);
	WayPointID temp1 = GetID ();
	myCPUWorkers.DoSomeWork (temp1, tempList, myOutputExitsCopy, myToken, workDesc, myFunc);

	// ask for more
	RequestTokens();
}

void TextLoaderWayPointImp :: ProcessHoppingUpstreamMsg (HoppingUpstreamMsg &message) {
	PDEBUG ("TextLoaderWayPointImp :: ProcessHoppingUpstreamMsg ()");

	FATALIF (!CHECK_DATA_TYPE (message.get_msg (), StartProducingMsg),
		"Strange, why did a text loader get a HUS of a type that was not 'Start Producing'?");

	// access the content of the message
	StartProducingMsg myMessage;
	message.get_msg ().swap (myMessage);

	// see what query we are asked to start
	QueryExit &queryToStart = myMessage.get_whichOne ();
	cout << "About to start ";
	queryToStart.Print ();
	cout << "\n";
	
	RequestTokens();
}

void TextLoaderWayPointImp :: ProcessDropMsg (QueryExitContainer &whichExits, HistoryList &lineage) {

	PDEBUG ("TextLoaderWayPointImp :: ProcessDropMsg ()");
	WARNING("Got a killed chunk. This is not good since we cannot reproduce chunks for now");

}

void TextLoaderWayPointImp :: DoneProducing (QueryExitContainer &whichOnes, HistoryList &history, 
																						 int result, ExecEngineData& data) {
	PDEBUG ("TextLoaderWayPointImp :: DoneProducing ()");
	TextLoaderHistory myHistory;
	history.MoveToStart();
	myHistory.swap(history.Current());
	string file = myHistory.get_file();
	// put it back
	myHistory.swap(history.Current());

	TextLoaderResult tempResult;
	tempResult.swap(data);

	// form the chunk that goes out
	ChunkContainer chkCont(tempResult.get_myChunk());
	// put it into data (what should have been)
	data.swap(chkCont);
	chunksOut++; // one chunk out
	
	// if the result is 1, the stream exhausted the input
	if (result == 1) {
		numStreams--; // one stream done

		// close the stream
		fclose(tempResult.get_stream());

		LOG_ENTRY_P(2, "Text Loader %s finished processing file %s.",
								name.c_str(), file.c_str()) ;

	} else {
		// just take the returned stream back to the tasks list
		TextLoaderDS task(tempResult.get_stream(), file, 
											tempResult.get_localDictionary());
		tasks.Append(task);
		
	}

	RequestTokens();

}

void TextLoaderWayPointImp :: ProcessAckMsg (QueryExitContainer &whichExits, HistoryList &lineage) {
	PDEBUG ("TextLoaderWayPointImp :: ProcessAckMsg ()");

	// make sure that the HistoryList has one item that is of the right type
	lineage.MoveToStart ();
	FATALIF (lineage.RightLength () != 1 || !CHECK_DATA_TYPE (lineage.Current (), TextLoaderHistory),
		"Got a bad lineage item in an ack to a table scan waypoint!");

	// get the history out
	TextLoaderHistory myHistory;
	lineage.Remove (myHistory);

	chunksOut--; // one chunk less flying

	// are we done with everything?
	if (chunksOut == 0 && numStreams == 0) {
		QueryExitContainer allCompleteCopy;
		allCompleteCopy.copy (myExits);
		QueryDoneMsg qDone (GetID (), myExits); // do not need my exits anymore
		HoppingDownstreamMsg myOutMsg (GetID (), allCompleteCopy, qDone);
		SendHoppingDownstreamMsg (myOutMsg);
		
		LOG_ENTRY_P(2, "Text Loader %s finished processing ALL files.",
								name.c_str()) ;

		// make sure we merge the local dictionaries and flush
		HString::SaveDictionary();
		DictionaryManager::Flush();
	}
	
}


