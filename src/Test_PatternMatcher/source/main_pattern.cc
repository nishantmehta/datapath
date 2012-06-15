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
#include <iostream>
#include <PatternMatcher.h>
#include "HString.h"

using namespace std;

void test_pattern (string pattern, string regexp) {
	HString hello( pattern.c_str() );
	string regex(regexp.c_str());

	PatternMatcher patt(regex);

	if (patt.IsMatch(hello))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;

	if (patt.IsMatch(hello))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;

	HString bogus("bogus");
	if (patt.IsMatch(bogus))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;
}

void test_pattern_static (string pattern, sregex& regex) {
	HString hello( pattern.c_str() );

	PatternMatcher patt(regex);

	if (patt.IsMatch(hello))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;

	if (patt.IsMatch(hello))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;

	HString bogus("bogus");
	if (patt.IsMatch(bogus))
		cout << endl << "Pattern match successful" << endl;
	else
		cout << endl << "Failed !" << endl;
}

int main() {

	test_pattern("hello world!", "(\\w+) (\\w+)!");
	test_pattern("jhghjgjhgjkgjkh", "(\\w+) (\\w+)!");

	sregex rex = (s1= +_w) >> ' ' >> (s2= +_w) >> '!';
	test_pattern_static("hello world!", rex);
	test_pattern_static("jhghjgjhgjkgjkh", rex);


	return 0;
}

