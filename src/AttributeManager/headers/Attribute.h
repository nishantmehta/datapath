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
// for Emacs -*- c++ -*-
#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include "Errors.h"
#include "Debug.h"
#include "TwoWayList.h"
#include "TwoWayList.cc"

#include <string>

using namespace std;

/** This class defines what an attribute is in the catalog. It contains
	* the following information:
	*
	*  - Attribute name.
	*  - Attribute type.
	*  - Attribute index.
	*  - Primary key information.
	*  - Foreign key information.
	*
	* Keep in mind that an instance of this class can be incomplete. For
	* example, it can contain the attribute name, but not the type nor
	* the index. Or, it can contain just the index. This is necessary in
	* order to allow for flexibility with operations like schema
	* projections and unions, so that those methods just receive a
	* collection of Attribute types and not things like integers
	* (indices) or strings (att. names).
	*
	* Take a look at the description of the "state" variables, as they
	* define how "complete" an instance of this class is.
**/
class Attribute {

#include "AttributePrivate.h"

public:
	// Empty constructor -- for collections
	Attribute();

	// Constructor with just a name
	Attribute(string _name);

	// Constructor with just an index
	Attribute(int _index);

	// Constructor with name, type, index and number of uniques
	Attribute(string _name, string _type, int _index, long int _uniques);

	// destructor
	virtual ~Attribute() {}

	// Get/set for attribute name
	string GetName();
	void SetName(string name);

	// Get/set for attribute type
	string GetType();
	void SetType(string  &in);

	// Get/set for attribute index
	int GetIndex();
	void SetIndex(int idx);

	// Get/set for the number of unique values for this attribute
	long int GetUniques();
	void SetUniques(long int idx);

	// Get/set for primary key
	bool IsPrimaryKey();
	void SetPrimaryKey(bool val);

	// Get/sets for foreign key info
	bool IsForeignKey();
	string GetForeignRelation();
	string GetForeignAttribute();
	void SetForeignKey(string relation, string attribute);

	// Copies the contents of this instance.
	void CopyTo(Attribute &out);

	// swaps the contents of *this with the parameter instance.
	void swap(Attribute &with);

	// Prints attribute information -- for debugging
	void Print();
};

// list of attributes to talk to the Catalog
typedef TwoWayList<Attribute> AttributeContainer;

#endif // _ATTRIBUTE_H
