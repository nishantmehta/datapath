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
#include <iostream>
#include "Handler.h"
#include <assert.h>
#include "Config.h"
#include <string.h>


HandlerImp::HandlerImp(void)
#ifdef DEBUG_EVPROC
: EventProcessorImp(true, "Handler") // comment to remove debug
#endif
{
  // register the messages
  RegisterMessageProcessor(DieMessage::type, &Die, 0 /* highest priority */);
  RegisterMessageProcessor(LoaderMessage::type, &ProcessMe, 1 /*priority*/);
}

// ProcessMe handler definition
MESSAGE_HANDLER_DEFINITION_BEGIN(HandlerImp, ProcessMe, LoaderMessage){
  // add check if message buffer is correct
	char* buffer = msg.buffer;
	int next = 0;
	for (int i=0; i<CHUNK_SIZE; i++){
		assert(strncmp(buffer+next, "1234 abcd", LEN-1));
		next += LEN;
  }

  // send the reply back
  AckMessage_Factory(msg.sender);
}MESSAGE_HANDLER_DEFINITION_END

// Die handler definition
MESSAGE_HANDLER_DEFINITION_BEGIN(HandlerImp, Die, DieMessage)
	evProc.Seppuku();
MESSAGE_HANDLER_DEFINITION_END

