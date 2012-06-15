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
#include "datatypes/DSTRING.h"
#include <iostream>
#include <string.h>
#include <sstream>

/** This test creates new words (does not initialize the dictionery)
		and puts them in, verifies that they are correctly retreived and
		saves the dictionary. */

#define NUM_STRINGS 1000

using namespace std;

int main(void){
	// form the strings "String %d" and add them.
	for (unsigned int i=0; i<NUM_STRINGS; i++){
		stringstream strm;
		strm << "String" << i;
		string num;
		strm >> num;
		DString str(num.c_str(), true);
	}

	// now create the again and compare
	for (unsigned int i=0; i<NUM_STRINGS; i++){
		stringstream strm;
		strm << "String" << i;
		string num;
		strm >> num;
		DString str(num.c_str(), false);
		
		if (! (str == num.c_str()) ){
			cout << "string" << str << "not found" << endl; 
		}
	}

	DString::SaveDictionary();


	return 0;
}

