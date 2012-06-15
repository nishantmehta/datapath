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

#ifndef CI_CC
#define CI_CC

#include "ColumnIterator.h"
using namespace std;
#include <iostream>
#include <assert.h>

#define _dp_max(x,y) ((x)>(y)? (x) : (y))

// 2nd argument is zero for innder iterator 'it', because we assume this iterator is for fixed size types
template <class DataType> 
ColumnIterator <DataType> :: ColumnIterator (Column &iterateMe, int stepSize) : it (iterateMe, 0, _dp_max(stepSize, sizeof(DataType))) {

	// return if invalid column, because otherwise we set objLen and that is used in Advance unnecessarily to advance
	if (it.IsInvalid ())
		return;

	// Set the object size
	it.SetObjLen (sizeof(DataType));
}

// 2nd argument is zero for innder iterator 'it', because we assume this iterator is for fixed size types
template <class DataType> 
ColumnIterator <DataType> :: ColumnIterator (Column &iterateMe, int fragmentStart, int fragmentEnd, int stepSize) :
														 it (iterateMe, fragmentStart, fragmentEnd, 0, _dp_max(stepSize, sizeof(DataType))) {

	// return if invalid column, because otherwise we set objLen and that is used in Advance unnecessarily to advance
	if (it.IsInvalid ())
		return;

	// Set the object size
	it.SetObjLen (sizeof(DataType));
}

template <class DataType> 
ColumnIterator <DataType> :: ColumnIterator () : it () {
	// This is iterator with no column, hence invalid state set
}

template <class DataType>
ColumnIterator <DataType> :: ~ColumnIterator () {
} 

template <class DataType>
void ColumnIterator <DataType> :: Done (Column &iterateMe) {
	it.Done(iterateMe);
}

template <class DataType>
void ColumnIterator <DataType> :: CreateDeepCopy (ColumnIterator<DataType>& fromMe) {
	it.CreateDeepCopy(fromMe.it);
}

template <class DataType>
void ColumnIterator <DataType> :: swap (ColumnIterator& swapMe) {
	it.swap (swapMe.it);
}

template <class DataType>
void ColumnIterator <DataType> :: CreateShallowCopy (ColumnIterator& copyMe) {
	it.CreateShallowCopy (copyMe.it);
}

template <class DataType>
void ColumnIterator <DataType> :: CheckpointSave () {
	it.CheckpointSave ();
}

template <class DataType>
void ColumnIterator <DataType> :: CheckpointRestore () {
	it.CheckpointRestore ();
}

#endif
