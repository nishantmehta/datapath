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

#include "Generator.h"


GeneratorImp::GeneratorImp(EventProcessor& _adder, DistributedCounter& _dCounter)
  :dCounter(_dCounter), counter(0)
#ifdef DEBUG_EVPROC
, EventProcessorImp(true,"Generator") // comment to remove debug
#endif
{
  Adder.copy(_adder);
  
  // register the message function
  RegisterMessageProcessor(TestNextMessage::type, &NumberAdded, 1 /* priority */);

  // send the first number 
  GenerateAndSendNumber();
}

void GeneratorImp::GenerateAndSendNumber(void){
  // se send the current counter to the Adder
  // we increment it when the confirmation comes back

  // if we do not clone the interface, the method swaps the content out
  EventProcessor myInterfClone;
  myInterfClone.copy(myInterface);

  TestGeneratorMessage_Factory(Adder, counter, myInterfClone);
}

MESSAGE_HANDLER_DEFINITION_BEGIN(GeneratorImp, NumberAdded, TestNextMessage)
  evProc.counter++;
  if (evProc.counter < REPETITIONS){
    // wait a little
    //usleep(rand()%1000);

    evProc.GenerateAndSendNumber();
  } else {
		if (evProc.dCounter.Decrement(1) == 0){
			printf("I am the last Generator. I'm killing the Adder\n");
      // this is the last producer, kill the adder
			KillEvProc(evProc.Adder);
		}
		
		// we are done, we have to commit suicide
		evProc.Seppuku();
  }
MESSAGE_HANDLER_DEFINITION_END

