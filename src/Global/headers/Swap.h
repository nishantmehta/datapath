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
#ifndef _SWAP_H_
#define _SWAP_H_

/**
	This header contains macro definitions to streamline the swapping of data.
	The main macro defined is SWAP(a,b) that swaps the content of a and b as
	long as they are of the same type and they support the = operator.

	Notice that the type of the a and b is not passed and it is infered using typeof()
	operator.
*/

#if __cplusplus >= 201103L
#define SWAP(a,b) { \
    decltype(a) tmp = ( a ); \
    ( a ) = ( b ); \
    ( b ) = tmp; \
}
#else
#define SWAP(a,b) { \
    typeof(a) tmp = ( a ); \
    ( a ) = ( b ); \
    ( b ) = tmp; \
}
#endif


/* Macro to swap classes using memmove.

   WARNING: this macro introduces a horrible bug if the class contains
   any STL datastructres. Use explicit SWAP.
*/

#define SWAP_memmove(class_name, object)		\
  char storage[sizeof (class_name)];			\
  memmove (storage, this, sizeof (class_name));		\
  memmove (this, &object, sizeof (class_name));		\
  memmove (&object, storage, sizeof (class_name));


#endif //_SWAP_H_
