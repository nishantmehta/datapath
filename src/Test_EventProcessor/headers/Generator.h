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
#include "Message.h"
#include "DistributedCounter.h"

#define REPETITIONS 1000

/** the class generates a number and sends it to an adder to be added. 
    It does this REPETITION times and then commits suicide 

    The next number is send only when the OK is received from the Adder.
*/

class GeneratorImp : public EventProcessorImp {
private:
  long int counter; // our number generator
  DistributedCounter& dCounter; // distributed counter that is thread safe. 
  // used to determine when all the generators are done to kill the Adder
  // so that the main program can finish

  EventProcessor Adder; // the adder we have to send our number

  // function to help in generating messages
  void GenerateAndSendNumber(void);

  GeneratorImp(EventProcessor& _adder, DistributedCounter& _dCounter);

  // message handler for the return OK
  MESSAGE_HANDLER_DECLARATION(NumberAdded)

  friend class Generator;
};

/////////////
// Interface class
class Generator : public EventProcessor {
public:
  Generator(EventProcessor& _adder, DistributedCounter& _dCounter);
};

inline Generator::Generator(EventProcessor& _adder, DistributedCounter& _dCounter){
  evProc = new GeneratorImp(_adder, _dCounter);
}

#endif // _GENERATOR_H_
