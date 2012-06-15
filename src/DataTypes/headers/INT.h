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
#ifndef _INT_H_
#define _INT_H_

#include <stdlib.h>
#include <stdio.h>

/** Type definition for the basic type INT */

typedef int INT; // use native int

/////////////////
// Aliases

typedef INT TINYINT;
typedef INT SMALLINT;
typedef INT INTEGER;

//////////////
// Inline functions

inline void FromString(INT& x, char* text){
    x=atoi(text);
}

inline int ToString(INT x, char* text){
    // add 1 for the \0
    return 1+sprintf(text,"%d", x);
}

// The hash function
// we just use conversion to unsigned int
inline uint64_t Hash(INT x){ return x;}

//////////////
// Operators

// all operators defined by C++

#endif //  _INT_H_
