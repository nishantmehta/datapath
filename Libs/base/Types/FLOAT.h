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
#ifndef _FLOAT_H_
#define _FLOAT_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

/** Type definition for the basic type FLOAT
 *
 *  TYPE_DESC
 *      NAME(</FLOAT/>)
 *      SIMPLE_TYPE
 *  END_DESC
 *
 *  SYN_DEF(</REAL/>, </FLOAT/>)
 */
typedef float FLOAT; // use native int

/////////////////
// Aliases

typedef FLOAT REAL;


//////////////
// Inline functions

inline void FromString(FLOAT& x, char* text){
    x=atof(text);
}

inline int ToString(const FLOAT& x, char* text){
    // add 1 for the \0
    return 1+sprintf(text,"%f", x);
}

// the hash function
// interpret as int (same size)
inline uint64_t Hash(const FLOAT val){
    return *( (const unsigned int*)(&val) );
}
//////////////
// Operators

// all operators defined by C++

#endif //  _FLOAT_H_
