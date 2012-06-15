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
#ifndef _ID_UNIQUE_H_
#define _ID_UNIQUE_H_

/** This header file defines the IDUnique class and the corresponding Info classes */

#include "IDInterface.h"
#include "Swap.h"
#include "Errors.h"

#include <string.h>
#include <typeinfo>


/** The class IDUnique is the ancestor of all IDs that need to be
		unique throughout the system. The uniqueness will be enforced at
		the level of this class not subsequent classes. This makes things
		much easier when it comes to manipulating the ids.

		All descendants from IDUnique can be place into heterogeneous
		containers. The container IDUniqueContainer is defined in ContainerTypes.h

		All IDUniques are designed to be used in containers and as keys in
		maps. They should work with both the STL containers and maps as
		well as the TwoWayList based list and map.

		We do not want any of the descentant types to behave like int. For
		this reason the hierachy is split into IDInt and IDUnique;

		WARNING: the derived classes should not increase the size of the object.
		Any extra information should be stored into static maps
*/

class IDUnique : public IDInterface {
protected:
	size_t id; // the internal representation of the ID

	static size_t nextID; // the next assignable id

	// create a new id not used before
	// in derived classes, this method should take extra parameters and should be public
	void NewID(void);

	// constructor from int (private to make sure is used only internally)
	IDUnique(size_t _id):id(_id){ }

public:
	// the default copy constructor and operator= should copy the object correctly

	// default constructor, this creates an invalid object and will
	// result in an error if used
	IDUnique(void);

	// return the real type of the object as a string. The string will be the name of the class
	// the implementation uses typeinfo
	// DO NOT REDEFINE IN DERIVED CLASSES
	const char* GetType(void) const;

	// swap implemented here
	void swap(IDUnique& other);

	// copy the content from another object
	void copy(IDUnique& other);

	/** Operator < for containers that need it */
	bool operator<(const IDUnique& other) const;
	bool LessThan(const IDUnique& other) const;

	/** Operator ==  for containers and other uses */
	bool operator==(const IDUnique& other) const;
	bool IsEqual(const IDUnique& other) const;

	/** Is this id valied, i.e. build with a constructor */
	bool IsValid(void);

	// getInfo transformed into virtual method. At this level it does nothing
	// needs to be redefined in derived classes
	virtual void getInfo(IDInfo& where){};

	// print the name, for debugging only
	void Print();
	string GetStr();
	size_t GetID() {return id;}

	// virtual destructor to get the vTable
	virtual ~IDUnique(void){ }
};


////////////////////////
/// INLINE METHODS

inline
bool IDUnique::IsValid(void){
	return id!=ID_UNINITIALIZED && id!=0;
}

inline
void IDUnique::Print(){
	IDInfo info;
	getInfo(info);
	cerr << info.getName();
}

inline
string IDUnique::GetStr(){
	IDInfo info;
	getInfo(info);
	string str;
	str += info.getName();
	return str;
}

inline
IDUnique::IDUnique(){
	// with this id, if the default constructed object is used as an index,
	// a segfault results immediatelly.
	id = 0;
}


/** The implementation of swap uses memcpy insteaad of SWAP to copy
 * the virtual table as well */
inline
void IDUnique::swap(IDUnique& swapMe){
	char *foo = new char[sizeof (IDUnique)];
	memmove (foo, this, sizeof (IDUnique));
	memmove (this, &swapMe, sizeof (IDUnique));
	memmove (&swapMe, foo, sizeof (IDUnique));
	delete [] foo;
}

inline
void IDUnique::copy(IDUnique& swapMe){
	memcpy (this, &swapMe, sizeof (IDUnique));
}


inline
void IDUnique::NewID(void){
	WARNINGIF(id!=0, "id should be 0 when calling NewID().");
	id = nextID;
	nextID++;
}

inline
bool IDUnique::operator<(const IDUnique& other) const{
	return (id<other.id);
}

inline
bool IDUnique::LessThan(const IDUnique& other) const{
	return (id<other.id);
}


inline
bool IDUnique::operator==(const IDUnique& other) const{
	return (id==other.id);
}

inline
bool IDUnique::IsEqual(const IDUnique& other) const{
	return (id==other.id);
}


inline
const char* IDUnique::GetType(void) const {
	return typeid(*this).name();
}

#endif // _ID_UNIQUE_H_
