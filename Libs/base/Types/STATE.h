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
#ifndef _STATE_H_
#define _STATE_H_

#include <cstdint>

/** Type definition for generic GLA states. This type is only used to
    trannsport states withing the same memory space between
    operators. The object the state points to MUST be treated like a
    const.

    Note: this type cannot be read from the disk or written to the
    output. A different mechanism will be used for that.

    The type in the object must be a hash of the name of the class
    used to encode the object. Any function that assumes a certain
    type must explicitly verify the correctness of the type.

    The object can be manipulated like a basic datatype. STATE objects
    do not know how to deallocate the memory they use. Other
    mechanisms have to be used to ensure correct deallocation
    (acknowledgements of data packets that contain this as members).

**/

/** Type definition for the basic type BIGINT
 *
 *  TYPE_DESC
 *      NAME(</STATE/>)
 *      SIMPLE_TYPE
 *  END_DESC
 */
class STATE {
  void* object; // the object we are pointing to
  uint64_t type; // type of object (acts like a password)

public:
  STATE():object(NULL), type(0){}
  STATE(void* _object, uint64_t _type):object(_object), type(_type){}

  void* GetObject(uint64_t password){
    return (type == password) ? object : NULL;
  }

  /** no destructor. object should not be deallocated here */
};

#endif  // _STATE_H_
