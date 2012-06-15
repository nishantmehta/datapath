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

#include "NumaMemoryAllocator.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "StorageUnit.h"
#include "DATE.h"
#include <assert.h>

using namespace std;

#include <iostream>

#define COUNT 10000000

struct Type {
	int a;
	int b;
	int c;
	Type (int aa, int bb, int cc): a(aa), b(bb), c(cc) {}

	bool operator==(const Type& rhs) {
		assert(this != &rhs);

		if (a == rhs.a && b == rhs.b && c == rhs.c)
			return true;

		return false;
	}
};


void simple_sanity_check() {
	// get some data
	StorageUnit temp;
	int *data = (int *) mmap_alloc (COUNT*sizeof(Type));
	Type* p = (Type*)data;
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		*p = t;
		p++;
	}

	// put it into the storage
	MMappedStorage myStore ((void*)data, COUNT*sizeof(Type), 0);

	// put the storage into a column
	Column myCol (myStore);

	// build an iterator for the column
	ColumnIterator <Type> myIter (myCol);

	// and iterate!
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		assert (myIter.GetCurrent () == t);
		myIter.Advance ();
	}
}

void simple_sanity_check_using_insert() {

	// insert the data
	MMappedStorage myStore;
	Column myCol (myStore);
	ColumnIterator <Type> myIter (myCol);
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		myIter.Insert (t);
		myIter.Advance ();
	}
	myIter.Done (myCol);

	//Now check sanity

	// build an iterator for the column
	ColumnIterator <Type> myIter_o (myCol);

	// and iterate!
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		assert (myIter_o.GetCurrent () == t);
		myIter_o.Advance ();
	}
}

void simple_sanity_check_using_insert_1() {

	// insert the data
	MMappedStorage myStore;
	Column myCol (myStore);
	ColumnIterator <int> myIter (myCol);
	for (int i = 0; i < COUNT; i++) {
		//Type t(i, i+1, i+2);
		myIter.Insert (i);
		myIter.Advance ();
		int x = i+1;
		myIter.Insert (x);
		myIter.Advance ();
		x = i+2;
		myIter.Insert (x);
		myIter.Advance ();
	}
	myIter.Done (myCol);

	//Now check sanity

	// build an iterator for the column
	ColumnIterator <int> myIter_o (myCol);

	// and iterate!
	for (int i = 0; i < COUNT; i++) {
		//Type t(i, i+1, i+2);
		assert (myIter_o.GetCurrent () == i);
		myIter_o.Advance ();
		assert (myIter_o.GetCurrent () == i+1);
		myIter_o.Advance ();
		assert (myIter_o.GetCurrent () == i+2);
		myIter_o.Advance ();
	}
}

void simple_sanity_check_using_partinsert() {

	// insert the data
	MMappedStorage myStore;
	Column myCol (myStore);
	ColumnIterator <Type> myIter (myCol);
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		myIter.Insert (t);
		myIter.Advance ();
	}
	myIter.Done (myCol);

	//Now check sanity

	// build an iterator for the column
	ColumnIterator <Type> myIter_o (myCol);

	// and iterate!
	for (int i = 0; i < COUNT/2; i++) {
		Type t(i, i+1, i+2);
		assert (myIter_o.GetCurrent () == t);
		myIter_o.Advance ();
	}

	// Now we want to insert while read above
	ColumnIterator <Type> myIter_p; // write only
	// new create deep copy of the column
	myIter_p.CreateDeepCopy(myIter_o);
	// now add more data to new column using new iterator
	for (int i = COUNT/2; i < COUNT; i++) {
		Type val = myIter_o.GetCurrent ();
		Type t(i, i+1, i+2);
		assert (t == val);
		myIter_p.Insert(val);
		myIter_o.Advance ();
		myIter_p.Advance ();
	}
	myIter_p.Done(myCol);

	// Create empty column
	Column newCol;
	// Swap out internal column in this column
	myIter_o.Done(newCol);

	ColumnIterator <Type> myIter_r (newCol);
	for (int i = 0; i < COUNT; i++) {
		Type t(i, i+1, i+2);
		assert (myIter_r.GetCurrent () == t);
		myIter_r.Advance ();
	}
	myIter_r.Done (newCol);
}

int main () {

	cout << "\nStarting simple sanity check";
	simple_sanity_check();
	cout << "\nFinished simple sanity check";

	cout << "\nStarting simple sanity check using insert";
	simple_sanity_check_using_insert();
	cout << "\nFinished simple sanity check using insert";

	cout << "\nStarting simple sanity check using insert 1";
	simple_sanity_check_using_insert_1();
	cout << "\nFinished simple sanity check using insert 1";

	cout << "\nStarting simple sanity check using part read and then insert";
	simple_sanity_check_using_partinsert();
	cout << "\nFinished simple sanity check using part read and then insert";
	cout << endl;

/*
	// put the column into another iterator
	ColumnIterator <int> myIter2 (myCol, 24);

	// and iterate!
	while (!myIter2.AtUnwrittenByte ()) {
		cout << myIter2.GetCurrent () << "\n";
		myIter2.Advance ();
	}
	cout << "\n\n";
	
	for (int i = 0; i < 8; i++) {
		int foo;
		foo = i + 8;
		myIter2.Insert (foo);;
		myIter2.Advance ();
	}

	// done iterating
	myIter2.Done (myCol);

	// put the column into another iterator
	ColumnIterator <int> myIter3 (myCol, 16);

	while (!myIter3.AtUnwrittenByte ()) {
		cout << myIter3.GetCurrent () << "\n";
		myIter3.Advance ();
	}
	cout << "\n\n";

	// create another store	
	MMappedStorage myStore2;
	
	// put it in a column
	Column myCol2 (myStore2);

	// put the column into an iterator
	ColumnIterator <int> myIter4 (myCol2, 16);

	// and put a bunch of data into it
	for (int i = 0; i < 12; i++) {
		int foo;
		foo = i + 100;
		myIter4.Insert (foo);;
		myIter4.Advance ();
	}

	// done iterating
	myIter4.Done (myCol2);

	// put the column into two iterators
	myCol.copy (myCol2);
	ColumnIterator <int> myIter5 (myCol, 16);
	ColumnIterator <int> myIter6 (myCol2, 16);

	for (int i = 0; i < 12; i++) {
		if (i < 6) {
			cout << myIter5.GetCurrent () << " " <<
			        myIter6.GetCurrent () << "\n";
		} else {
			int foo;
			foo = i + 2000;
			myIter6.Insert (foo);
			foo = i + 1000;
			myIter5.Insert (foo);
		}
		myIter5.Advance ();
		myIter6.Advance ();
	}

	cout << "\n\n";

	myIter5.Done (myCol2);
	myIter6.Done (myCol);
	Column myCol3;
	myCol3.copy (myCol2);
	ColumnIterator <int> myIter7 (myCol, 16);
	ColumnIterator <int> myIter8 (myCol2, 16);
	ColumnIterator <int> myIter9 (myCol3, 16);

	while (!myIter7.AtUnwrittenByte ()) {
		cout << myIter7.GetCurrent () << " " <<
			myIter8.GetCurrent () << " " <<
			myIter9.GetCurrent () << "\n";
		myIter7.Advance ();
		myIter8.Advance ();
		myIter9.Advance ();
	}
*/	
	
	
}
