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
#ifndef _GENERATOR_H_
#define _GENERATOR_H_


#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "NumaTestMessages.h"
#include "DistributedCounter.h"

class GeneratorImp : public EventProcessorImp {
private:

  EventProcessor Adder; // the adder we have to send our number

  DistributedCounter& dCounter; // to know when all addres are done

  int mNode; // numa node of this Generator

  // function to help in generating messages
  void ComputeAndSendTime(void);


	// buffer used to generate/consume numbers
	int* buffer;

	// global counter (always increasing) for filling in ints
	int intCnt;
	
	// number of blocks sent
	int CNT;

  GeneratorImp(EventProcessor& adder, DistributedCounter& counter, int node);

  // message handler for the return OK
  MESSAGE_HANDLER_DECLARATION(NumberAdded)

  friend class Generator;
};

/////////////
// Interface class
class Generator : public EventProcessor {
public:
  Generator(EventProcessor& adder, DistributedCounter& counter, int node);
};

inline Generator::Generator(EventProcessor& adder, DistributedCounter& counter, int node){
  evProc = new GeneratorImp(adder, counter, node);
}

#endif // _GENERATOR_H_
