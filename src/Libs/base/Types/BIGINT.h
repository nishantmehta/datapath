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
#ifndef _BIGINT_H_
#define _BIGINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

/** Type definition for the basic type BIGINT
 *
 *  TYPE_DESC
 *      NAME(</BIGINT/>)
 *      SIMPLE_TYPE
 *  END_DESC
 *
 *  SYN_DEF(</IDENTIFIER/>, </BIGINT/>)
 */

typedef uint64_t BIGINT; // use native int

/////////////////
// Aliases

// SYN_DEF(</IDENTIFIER/>, </BIGINT/>)
typedef BIGINT IDENTIFIER;


//////////////
// Inline functions

inline void FromString(BIGINT& x, char* text){
    x=atol(text);
}

inline int ToString(const BIGINT& x, char* text){
    // add 1 for the \0
    return 1+sprintf(text,"%ld", (long int)x);
}

inline uint64_t Hash(const BIGINT val){
    return val;
}


//////////////
// Operators

// all operators defined by C++

#endif //  _BIGINT_H_
