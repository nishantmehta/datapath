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
#ifndef _ADDER_H_
#define _ADDER_H_

#include "EventProcessor.h"
#include "NumaTestMessages.h"

/** the class adds numbers received from generators 
     When die message is received it prints the result 
*/

class AdderImp : public EventProcessorImp {
 private:
  long int sum; // the total sum the adder maintains
  
 public:
  AdderImp(void);

  // message handling funciton for the numbers
  MESSAGE_HANDLER_DECLARATION(AddMe);

  // new handler for the die message to do more stuff before death
  MESSAGE_HANDLER_DECLARATION(Die);
};

/////////////
// Interface class
class Adder : public EventProcessor {
 public:
  Adder(void);
};

inline Adder::Adder(void){
  evProc = new AdderImp();
}

#endif // _ADDER_H_
