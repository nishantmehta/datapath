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
#ifndef P_MATCH_OING
#define P_MATCH_OING

#include <iostream>
#include <boost/xpressive/xpressive.hpp>
#include "HString.h"
#include <tr1/unordered_map>
#include <string>

/** NOTE: The dictionary accelerated pattern matcher does not seem to work particularly well. This is 
    probably due to the fact that there is a lot of allocation going on to provide the acceleration.

    TODO: switch the acceleration on only for frequent strings. Even then things might not be worth it.
*/

using namespace boost::xpressive;
using namespace std;

class PatternMatcher {

private:

	cregex mRegExp;
public:

	// Dynamic xpressive
	PatternMatcher(string regexp);

	// match function 
	bool IsMatch (HString& str);
};

inline
PatternMatcher :: PatternMatcher (string regexp) {
	// This is dynamic xpressive
	mRegExp = cregex::compile( regexp.c_str() );
}

inline
bool PatternMatcher :: IsMatch (HString& str) {
	return (regex_match( (const char*)str, mRegExp ));
}

#endif // P_MATCH_OING
