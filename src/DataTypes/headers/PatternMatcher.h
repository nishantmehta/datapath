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
#ifndef PATTERN_MATCHER_H
#define PATTERN_MATCHER_H

// #define DP_REGEX_LIB 2 // oing

/* We can select the regex library by setting the variable DP_REGEX_LIB
   to the name. The name has to match the name used in writing the PatternMatch-_name_.h
*/

#if DP_REGEX_LIB == 1 // xpressive
#include "PatternMatcher-xpressive.h"
#elif DP_REGEX_LIB == 2 // onig
#include "PatternMatcher-onig.h"
#else
#error "A pattern matcher library must be selected. See DataTypes/headers/PatternMatcher-*.h for choinces"
#endif

#endif //  PATTERN_MATCHER_H
