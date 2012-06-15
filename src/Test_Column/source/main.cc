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

#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "StorageUnit.h"
#include "CHRISINT.h"

using namespace std;

#include <iostream>

void *mmap_alloc (int numBytes) {
	return malloc (numBytes);
}

void mmap_free (void *me) {
	free (me);
}

int main () {

	// get some data
	StorageUnit temp;
	int *data = (int *) mmap_alloc (16);
	for (int i = 0; i < 4; i++) {
		data[i] = i;
	}

	// put it into the storage
	MMappedStorage myStore (data, 16);

	// put the storage into a column
	Column myCol (myStore);

	// build an iterator for the column
	ColumnIterator <INT> myIter (myCol, 16);

	// and iterate!
	for (int i = 0; i < 4; i++) {
		cout << myIter.GetCurrent () << "\n";
		myIter.Advance ();
	}

	// now add some data
	for (int i = 0; i < 4; i++) {
		INT foo;
		foo = i + 4;
		myIter.Insert (foo);;
		myIter.Advance ();
	}

	// done iterating
	myIter.Done (myCol);

	// put the column into another iterator
	ColumnIterator <INT> myIter2 (myCol, 24);

	// and iterate!
	while (!myIter2.AtUnwrittenByte ()) {
		cout << myIter2.GetCurrent () << "\n";
		myIter2.Advance ();
	}
	cout << "\n\n";
	
	for (int i = 0; i < 8; i++) {
		INT foo;
		foo = i + 8;
		myIter2.Insert (foo);;
		myIter2.Advance ();
	}

	// done iterating
	myIter2.Done (myCol);

	// put the column into another iterator
	ColumnIterator <INT> myIter3 (myCol, 16);

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
	ColumnIterator <INT> myIter4 (myCol2, 16);

	// and put a bunch of data into it
	for (int i = 0; i < 12; i++) {
		INT foo;
		foo = i + 100;
		myIter4.Insert (foo);;
		myIter4.Advance ();
	}

	// done iterating
	myIter4.Done (myCol2);

	// put the column into two iterators
	myCol.copy (myCol2);
	ColumnIterator <INT> myIter5 (myCol, 16);
	ColumnIterator <INT> myIter6 (myCol2, 16);

	for (int i = 0; i < 12; i++) {
		if (i < 6) {
			cout << myIter5.GetCurrent () << " " <<
			        myIter6.GetCurrent () << "\n";
		} else {
			INT foo;
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
	ColumnIterator <INT> myIter7 (myCol, 16);
	ColumnIterator <INT> myIter8 (myCol2, 16);
	ColumnIterator <INT> myIter9 (myCol3, 16);

	while (!myIter7.AtUnwrittenByte ()) {
		cout << myIter7.GetCurrent () << " " <<
			myIter8.GetCurrent () << " " <<
			myIter9.GetCurrent () << "\n";
		myIter7.Advance ();
		myIter8.Advance ();
		myIter9.Advance ();
	}
	
	
	
}
