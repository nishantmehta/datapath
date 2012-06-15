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
#include "Vector.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "StorageUnit.h"
#include<assert.h>

using namespace std;

#define VECSIZE 10

#define COLUMNSIZE 100000

void vector_test() {

	Vector<int, VECSIZE> vec;

	for (int i = 0; i < VECSIZE; i++)
		vec[i] = i;

	MMappedStorage myStore;

	Column myCol (myStore);

	ColumnIterator <Vector<int, VECSIZE> > myIter (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		myIter.Insert(vec);
		myIter.Advance();
	}

	myIter.Done(myCol);

	ColumnIterator <Vector<int, VECSIZE> > myIter2 (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		Vector<int, VECSIZE> v = myIter2.GetCurrent();
		assert (v == vec);
		assert (!(v != vec));
		assert (!(v < vec));
		myIter2.Advance();
	}
}

void sqmatrix_test() {

	SquareMatrix<int, VECSIZE> matrix;

	for (int i = 0; i < VECSIZE; i++)
		for (int j = 0; j < VECSIZE; j++)
			matrix[i][j] = i+j;

	MMappedStorage myStore;

	Column myCol (myStore);

	ColumnIterator <SquareMatrix<int, VECSIZE> > myIter (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		myIter.Insert(matrix);
		myIter.Advance();
	}

	myIter.Done(myCol);

	ColumnIterator <SquareMatrix<int, VECSIZE> > myIter2 (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		SquareMatrix<int, VECSIZE> m = myIter2.GetCurrent();
		assert (m == matrix);
		assert (!(m != matrix));
		assert (!(m < matrix));
		myIter2.Advance();
	}
}


void matrix_test() {

	Vector<int, VECSIZE> vec;

	for (int i = 0; i < VECSIZE; i++)
			vec[i] = i;

	Matrix<Vector<int, VECSIZE>, VECSIZE/2> matrix;

	for (int i = 0; i < VECSIZE/2; i++)
			matrix[i] = vec;

	MMappedStorage myStore;

	Column myCol (myStore);

	ColumnIterator <Matrix<Vector<int, VECSIZE>, VECSIZE/2> > myIter (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		myIter.Insert(matrix);
		myIter.Advance();
	}

	myIter.Done(myCol);

	ColumnIterator <Matrix<Vector<int, VECSIZE>, VECSIZE/2> > myIter2 (myCol);

	for (int i = 0; i < COLUMNSIZE; i++) {
		Matrix<Vector<int, VECSIZE>, VECSIZE/2> m = myIter2.GetCurrent();
		assert (m == matrix);
		assert (!(m != matrix));
		assert (!(m < matrix));
		myIter2.Advance();
	}
}


int main() {

	vector_test();
	cout << "\nVector test passed";
	sqmatrix_test();
	cout << "\nSqMatrix test passed";
	matrix_test();
	cout << "\nMatrix test passed";

	cout << endl;
	return 0;
}
