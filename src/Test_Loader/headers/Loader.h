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
#ifndef _LOADER_H_
#define _LOADER_H_


#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "LoaderTestMessages.h"
#include "DistributedCounter.h"
#include <sys/types.h>
#include <pthread.h>

class LoaderImp : public EventProcessorImp {
private:

  EventProcessor mHandler; // the handler we have to send our chunk

  DistributedCounter& dCounter; // to know when all handlers are done

  int mNode; // numa node of this Loader

  // function to help in generating messages
  void CreateAndSendChunk(void);

	// buffer used to send chunk
	char* buffer;
	
	pthread_mutex_t mutex; // the mutex that protects the file reading

public:
  LoaderImp(EventProcessor& handler, DistributedCounter& counter, int node);

  // message handler for the return OK
  MESSAGE_HANDLER_DECLARATION(ChunkReceivedByHandler)

  friend class Handler;
};

/////////////
// Interface class
class Loader : public EventProcessor {
public:
  Loader(EventProcessor& handler, DistributedCounter& counter, int node);
};

inline Loader::Loader(EventProcessor& handler, DistributedCounter& counter, int node){
  evProc = new LoaderImp(handler, counter, node);
}

#endif // _LOADER_H_
