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
#include "MmapAllocator.h"
#include "Config.h"

GeneratorImp::GeneratorImp(EventProcessor& _adder, DistributedCounter& _dCounter,
			 int node)
  :mNode(node), dCounter(_dCounter)
#ifdef DEBUG_EVPROC
, EventProcessorImp(true,"Generator") // comment to remove debug
#endif
{
  Adder.copy(_adder);
  
  // register the message function
  RegisterMessageProcessor(TestNextMessage::type, &NumberAdded, 1 /* priority */);

	// allocate the buffer and reset counters

#ifdef SHUFFLE_NUMA // in this branch we do incorrect allocation
	buffer = (int*)mmap_alloc(NUM_INTS*sizeof(int), (mNode+3) % numaNodeCount());
#else
	buffer = (int*)mmap_alloc(NUM_INTS*sizeof(int), mNode % numaNodeCount());
#endif
	intCnt=0;
	CNT=0; 


  // send the first completion time to trigger the job
  ComputeAndSendTime();
}

void GeneratorImp::ComputeAndSendTime(void){
	CNT++;
	dCounter.Increment(1); // one more block generated
	
	// fill the buffer with increasing numbers
	for (int i=0; i<NUM_INTS; i++){
		buffer[i]=intCnt++;
	}

  // if we do not clone the interface, the method swaps the content out
  EventProcessor myInterfClone;
  myInterfClone.copy(myInterface);

  // Adder requires our clone to know whom to send message back
  TestGeneratorMessage_Factory(Adder, buffer, myInterfClone);
}

MESSAGE_HANDLER_DEFINITION_BEGIN(GeneratorImp, NumberAdded, TestNextMessage){
	// keep on forever
	evProc.ComputeAndSendTime();
}MESSAGE_HANDLER_DEFINITION_END

