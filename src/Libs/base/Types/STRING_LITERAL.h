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
#ifndef _STRING_LITERAL_H
#define _STRING_LITERAL_H

/**
   This defines a type for string literals, which are considered by be of type
   const char *. This allows for literal strings to be handled differently than
   string variables, which is required as datapath does not use const char *
   as its string type, and thus all string literals must be converted to
   another type in order to be useful.
 */

/** Type definition for the basic type STRING_LITERAL
 *
 *  TYPE_DESC
 *      NAME(</STRING_LITERAL/>)
 *  END_DESC
 */
typedef const char* STRING_LITERAL;

#endif
