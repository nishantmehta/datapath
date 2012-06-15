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
#include<iostream>
#include "Column.h"
#include "ColumnReadWrite.h"
#include "MMappedStorage.h"
#include "StorageUnit.h"
#include "NumaMemoryAllocator.h"
#include <assert.h>

#define SIZE 1000000

using namespace std;

class A {

public:

	int a;
	int b;

	A (int x = 0, int y = 0): a(x), b(y) {}

	void Serialize(ColumnReadWrite& obj) {
		obj.write((char*) this, sizeof(*this));
	}

	void Deserialize(ColumnReadWrite& obj) {
		obj.read((char*) this, sizeof(*this));
	}

	bool operator==(const A& rhs) {

		assert (a == rhs.a);
		assert (b == rhs.b);

		return true;
	}
};

class B {

public:

	int b;

	A* ptr;

	B (int x = 0, A* p = NULL): b(x), ptr(p) {}

	~B() {if (ptr) free(ptr);}

	void Serialize(ColumnReadWrite& obj) {
		obj.write((char*) (&b), sizeof(b));
		ptr->Serialize(obj);
	}

	void Deserialize(ColumnReadWrite& obj) {
		obj.read((char*) (&b), sizeof(b));

		if (ptr) free(ptr), ptr=NULL;
		ptr = (A*)malloc(sizeof(A));

		ptr->Deserialize(obj);
	}

	bool operator==(const B& rhs) {

		assert (b == rhs.b);
		assert (*ptr == *(rhs.ptr));

		return true;
	}
};


int main() {

  MMappedStorage myStore;

  Column myCol (myStore);

  ColumnReadWrite myIter (myCol);

	for (int i = 0; i < SIZE; i++) {
		A* p = (A*)malloc(sizeof(A));
		p->a = i*2; p->b = i*3;
		B b(i, p);
		b.Serialize(myIter);
	}
	
  myIter.Done (myCol);

  ColumnReadWrite myIt (myCol);

	for (int i = 0; i < SIZE; i++) {
		A* p = (A*)malloc(sizeof(A));
		p->a = i*2; p->b = i*3;
		B b(i, p);

		B nb;
		nb.Deserialize(myIt);
		(b == nb);
	}

  myIt.Done (myCol);

	return 0;
}
