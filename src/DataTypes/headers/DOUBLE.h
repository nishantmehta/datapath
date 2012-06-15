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
#ifndef _DOUBLE_H_
#define _DOUBLE_H_

#include <stdlib.h>
#include <stdio.h>

/** Type definition for the basic type DOUBLE */

typedef double DOUBLE; // use native int

/////////////////
// Aliases

// NO ALIASES

//////////////
// Inline functions

inline void FromString(DOUBLE& x, char* text){
    x=atof(text);
}

inline int ToString(DOUBLE x, char* text){
    // add 1 for the \0
    return 1+sprintf(text,"%1.15g", x);
}


// the hash function
// reinterpret bits as 64 bit int
inline uint64_t Hash(DOUBLE val){
    return   *((uint64_t*)(&val));
}
//////////////
// Operators

// all operators defined by C++

#endif //  _DOUBLE_H_
