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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Loader.h"
#include "NumaMemoryAllocator.h"
#include "Config.h"

LoaderImp::LoaderImp(EventProcessor& _handler, DistributedCounter& _dCounter,
			 int node)
  :mNode(node), dCounter(_dCounter)
#ifdef DEBUG_EVPROC
, EventProcessorImp(true,"Loader") // comment to remove debug
#endif
{
  mHandler.copy(_handler);
	pthread_mutex_init(&mutex,NULL);
  
  // register the message function
  RegisterMessageProcessor(AckMessage::type, &ChunkReceivedByHandler, 1 /* priority */);

	buffer = (char*)mmap_alloc(CHUNK_SIZE * LEN);

  // send the first completion time to trigger the job
  CreateAndSendChunk();
}

void LoaderImp::CreateAndSendChunk(void){
	dCounter.Increment(1); // one more block generated

	pthread_mutex_lock(&mutex);

	FILE* fd = fopen(FILENAME, "r");
  if (fd == NULL){
    return;
  }

	// Read the file and fill the data in memory allocated
	size_t read = 0;
	for (int i=0; i<CHUNK_SIZE; i++){
    char* rez = fgets(buffer+read, strlen(STR), fd);
    assert (rez!=NULL); // no defects in file
		read += LEN;
	}

	fclose(fd);

	pthread_mutex_unlock(&mutex);

  // if we do not clone the interface, the method swaps the content out
  EventProcessor myInterfClone;
  myInterfClone.copy(myInterface);

  // Handler requires our clone to know whom to send message back
  LoaderMessage_Factory(mHandler, buffer, myInterfClone);
}

MESSAGE_HANDLER_DEFINITION_BEGIN(LoaderImp, ChunkReceivedByHandler, AckMessage){
	// keep on forever
	evProc.CreateAndSendChunk();
}MESSAGE_HANDLER_DEFINITION_END

