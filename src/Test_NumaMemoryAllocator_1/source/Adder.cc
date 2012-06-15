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
#include "Adder.h"
#include "Config.h"


AdderImp::AdderImp(void)
#ifdef DEBUG_EVPROC
: EventProcessorImp(true, "Adder") // comment to remove debug
#endif
{
  sum=0;
  
  // register the messages
  RegisterMessageProcessor(DieMessage::type, &Die, 0 /* highest priority */);
  RegisterMessageProcessor(TestGeneratorMessage::type, &AddMe, 1 /*priority*/);
}

// AddMe handler definition
MESSAGE_HANDLER_DEFINITION_BEGIN(AdderImp, AddMe, TestGeneratorMessage){
  // add the numer to the sum
	int* buffer = msg.numbers;
	for (int i=0; i<NUM_INTS; i++){
		evProc.sum+=buffer[i];
  }

  // send the reply back
  TestNextMessage_Factory(msg.sender);
}MESSAGE_HANDLER_DEFINITION_END

// Die handler definition
MESSAGE_HANDLER_DEFINITION_BEGIN(AdderImp, Die, DieMessage)
	evProc.Seppuku();
MESSAGE_HANDLER_DEFINITION_END

