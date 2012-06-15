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

#include "HStringIterator.h"
#include "HString.h"

using namespace std;
#include <iostream>

// 2nd argument is 8 for inner iterator 'it' because we need 8 bytes header size info to read to know size of next obj
HStringIterator :: HStringIterator (Column &iterateMe, HString::Dictionary& _updateDictionary, int stepSize):
	updateDictionary(_updateDictionary), it (iterateMe, 16, stepSize) {

	if (it.IsInvalid ())
		return;

	// If column is blank and valid
	if (it.GetFirstInvalidByte() == 0)
		return;

	int objLen = ((HString*) it.GetData())->GetObjLength ();
	it.SetObjLen (objLen);
	it.EnsureFirstObjectSpace(BYTE_ALIGN(objLen));

  totalCNT=0;
}

HStringIterator :: HStringIterator (Column &iterateMe, int fragmentStart, int fragmentEnd, HString::Dictionary& _updateDictionary, int stepSize):
	updateDictionary(_updateDictionary), it (iterateMe, fragmentStart, fragmentEnd, 16, stepSize) {

	if (it.IsInvalid ())
		return;

	// If column is blank and valid
	if (it.GetFirstInvalidByte() == 0)
		return;

	int objLen = ((HString*) it.GetData())->GetObjLength ();
	it.SetObjLen (objLen);
	it.EnsureFirstObjectSpace(BYTE_ALIGN(objLen));

  totalCNT=0;
}


HStringIterator :: ~HStringIterator () {
} 

void HStringIterator :: Done (Column &iterateMe) {
	it.Done (iterateMe);
}

void HStringIterator :: CreateDeepCopy (HStringIterator& fromMe) {

	// do shallow copy first
	// memmove (this, &fromMe, sizeof (HStringIterator)); dont work for reference dictionary type

	it.CreateDeepCopy(fromMe.it);

	for (std::map<__uint64_t, int>::iterator it = (fromMe.hashFrequencyMap.begin());
				it != (fromMe.hashFrequencyMap.end()); ++it) {
		hashFrequencyMap[it->first] = it->second;
	}
}

void HStringIterator :: swap (HStringIterator& swapMe) {

	it.swap(swapMe.it);
	hashFrequencyMap.swap(swapMe.hashFrequencyMap); // STL swap
	updateDictionary.swap(swapMe.updateDictionary);
}

void HStringIterator :: CheckpointSave () {
	c_totalCNT = totalCNT;
	//c_hashFrequencyMap = hashFrequencyMap;
        it.CheckpointSave();
}

void HStringIterator :: CheckpointRestore () {
	totalCNT = c_totalCNT;
	//hashFrequencyMap = c_hashFrequencyMap;
        it.CheckpointRestore();
}

