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

#ifndef HI_H
#define HI_H
#include "Column.h"
#include <map>
#include "HString.h"
#include "Constants.h"
#include "Iterator.h"

/**
		This is superfast hash (integer) based string iterator. All relevant functions are inlined in this header file
		This iterator is tightly coupled with HString and details can be found there about the behavior
*/

class HStringIterator {
private:

  // interval value that we can return in GetCurrent()
  HString myValue;

  char guard[1024]; // buffer between myValue and the rest of the data

	// Just pass the type to follow the interface, but we dont necessarily need it
	Iterator it;

	// how many tuples are in
	int totalCNT;

	// map of frequency of hash strings
	std::map<__uint64_t, int> hashFrequencyMap;

	// dictionary we are usign for updates
	HString::Dictionary& updateDictionary;

	int c_totalCNT;
	//std::map<__uint64_t, int> c_hashFrequencyMap;

public:

	// creates a HString iterator for the given column... the requests for data that
	// are sent to the column are of size stepSize.  iterateMe is consumed.
	HStringIterator (Column &iterateMe, HString::Dictionary& _dictionary = HString::globalDictionary,
								 int stepSize = COLUMN_ITERATOR_STEP);

	// This will iterate from [fragmentStart, fragmentEnd]
	HStringIterator (Column &iterateMe, int fragmentStart, int fragmentEnd, HString::Dictionary& _dictionary = HString::globalDictionary,
								 int stepSize = COLUMN_ITERATOR_STEP);

	// This is empty ctor for swapping use
	HStringIterator(HString::Dictionary& _dictionary = HString::globalDictionary, int stepSize = COLUMN_ITERATOR_STEP) : updateDictionary(_dictionary), totalCNT(0) {};

	// destructor... if there is a column left in the HStringIterator, it will be
	// destroyed as well
	~HStringIterator ();

	// tells the iterator that we are done iterating... any new bytes written past
	// the end of the column will be included in the column from now on.  The column
	// is taken out of the iterator and put into iterateMe
	void Done (Column &iterateMe);

	// advance to the next object in the column...
	void Advance ();


	// add a new data object into the column at the current position, overwriting the
	// bytes that are already there.  Note that if the size of addMe differs from the
	// size of the object that is already there, addMe will over-run part of the next
	// object.  Sooooo... overwrite existing objects in the column with care!
	void Insert (HString &addMe);
	void Insert (const HString &addMe);

	// returns true if the object under the cursor has never been written to and so
	// it should not be read (it is undefined what happens if you read it)
	int AtUnwrittenByte ();

	// returns the data object at the current position in the column...
	const HString& GetCurrent ();

	// Is the string frequent
	// The function will also increase the frequency of an item
	bool IsFrequent (__uint64_t hashVal);

/**
  create deep copy from 'fromMe'. Everything is created new down the hierarchy,
  basically all iterator states are replicated, new Column and MMappedStorage is
  created with partial data + states copied. Storage is write only for this iterator.
  The storage (data) is copied only up to the current position of fromMe. This is used
  (for example) in the join, where you find a tuple that matches with more than one tuple
  on the RHS and so now you need to start making multiple copies of values.

  Here is the short usage,
    HStringIterator<Type> myIter;
    // make sure fromMe.Done(col) is NOT called before making deep copy
    // after below line, myIter is ready to add more data using Insert(), since
    // it is pointing at the end of new replicated partial storage.
    myIter.CreateDeepCopy (fromMe); // myIter will contain write only storage
                                    // and ready to add more data using Insert()
    // Now start Inserting into myIter
*/
	void CreateDeepCopy (HStringIterator& fromMe);

	void swap (HStringIterator& swapMe);

	void MarkFragment ();

        void CheckpointSave ();

        void CheckpointRestore ();

	HStringIterator& operator=(const HStringIterator&);
};

////////// INLINE FUNCTIONS //////////////

inline
HStringIterator& HStringIterator::operator=(const HStringIterator& other) {
	// any crap can be here because this is just to support STL
}

inline
const HString& HStringIterator :: GetCurrent () {
  /* we do this here to ensure that we do not use the garbage at the
     end and ask for a lot of crap. */

  int objLen = myValue.GetObjLength();
  it.EnsureSpace ( objLen, objLen );
  
  return myValue;
}

inline
int HStringIterator :: AtUnwrittenByte () {
	//return (objLen == 0);
	return it.AtUnwrittenByte ();
}

inline
bool HStringIterator :: IsFrequent (__uint64_t hashVal) {

  return false; // nothing is frequent anymore

  /* USE THIS STUFF IF YOU WANT TO FORCE THINGS INTO DICTIONARY
  return true; //WARNING. This is here as a temporary hack, remove it before loading any large relation;

  WE NEED A BETTER SOLUTION
  */
    hashVal = MASK_IN_DICT(hashVal);
	totalCNT++;

	std::map<__uint64_t, int>::iterator it = hashFrequencyMap.find(hashVal);
	if (it != hashFrequencyMap.end()) {
		it->second++;

		return (it->second > MIN_FREQUENT) || (it->second > MIN_THRESHOLD*totalCNT);
	}

	hashFrequencyMap[hashVal]=1;

	return false;
}


inline
void HStringIterator :: Insert (const HString &addMe) {
	Insert(const_cast<HString&>(addMe));
}

inline
void HStringIterator :: Insert (HString &addMe) {

	if (it.IsInvalid ())
		return;

  assert (it.IsWriteOnly() == true);
  // addMe already has the bit set if in global dictionary
  // first see if it is in the local dictionary

  if (!addMe.IsInDictionary())
    addMe.LookUpInLocalDictionary(updateDictionary);
  // here IsInDictionary() tells us if addMe is in either the local or
  // global dictionary

  // then we do not track it in frequency map
  if (!addMe.IsInDictionary()){// if not in dictionary
    if (IsFrequent(addMe.GetHashValue())) {
      // if not in dictionary and string is frequent, add in dictionary
      // addMe is modified with one extra bit set indicating in dictionary

      HString::AddEntryInDictionary(addMe, updateDictionary);
    }
  }

  // see how much space we need
  it.SetObjLen (addMe.ComputeObjLength ());

  it.EnsureWriteSpace ();

  char* myData = it.GetData ();
  // and write the object
  *((__uint64_t*) myData) = addMe.GetHashValue(); // this has to be written in all cases

  if (!addMe.IsInDictionary()) { // if not in dictionary
    // infrequent and not in dictionary, just write rest of the details
    // GetStrLength() is valid only when ComputeObjLength is called above
    *((__uint64_t*) myData + 1) = addMe.GetStrLength();
    WARNINGIF(addMe.GetStrLength() > 256, "String is too weird size %lld", (long long) addMe.GetStrLength());

    // GetString() is sure to have some value if string is not in dictionary
    strcpy(myData + 16, addMe.GetString());

  }
  
  myValue.Set(myData);
}

inline
void HStringIterator :: Advance () {

  if (it.IsInvalid ())
    return;
  
  // advance our position
  int oObjLen = myValue.GetObjLength ();
  it.SetObjLen (oObjLen);
  it.AdvanceBy (oObjLen);

  if (!it.IsWriteOnly()){
    it.EnsureHeaderSpace ();
    myValue.Set(it.GetData());
  }
}

inline
void HStringIterator :: MarkFragment () {
	if (it.IsInvalid())
		return;

	it.MarkFragment ();
}

#endif
