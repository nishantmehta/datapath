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

class INT {

int data;

public:

INT & operator = (const INT &rhs) {
	data = rhs.data;
	return *this;
}

INT & operator = (const int rhs) {
	data = rhs;
	return *this;
}

operator int () {
	return data;
}

int GetLength () {
	return sizeof (data);
}

};
