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
