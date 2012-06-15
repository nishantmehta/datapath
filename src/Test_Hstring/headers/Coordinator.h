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
#ifndef _COORDINATOR_H_
#define _COORDINATOR_H_






// include the base class definition
#include "EventProcessor.h"

// include the implementation definition
#include "CoordinatorImp.h"

/** Class to provide an interface to CoordinatorImp class.

    See CoordinatorImp.h for a description of the functions 
    and behavior of the class
*/
class Coordinator : public EventProcessor {
public:

	
  // constructor (creates the implementation object)
  Coordinator(int _bogus){
    evProc = new CoordinatorImp(_bogus);
  }

	
  // default constructor
  Coordinator(void){
    evProc = NULL;
  }


  // the virtual destructor
  virtual ~Coordinator(){}
};



#endif // _COORDINATOR_H_
