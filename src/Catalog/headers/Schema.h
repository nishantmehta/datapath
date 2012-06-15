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

#ifndef _SCHEMA_H
#define _SCHEMA_H

#include "Attribute.h"
#include "Debug.h"
#include "Errors.h"
#include "TwoWayList.h"
#include "TwoWayList.cc"

#include <string>

using namespace std;

/** This class stores schema infromation about a particular relation.
	* Essentially, the attributes that compose it (a collection of Attribute
	* objects), the name of the relation, the path to the metadata file
	* and the number of tuples in the relation.
	*
	* Additionally, functionality for projection and schema union is provided.
**/
class Schema {

#include "SchemaPrivate.h"

public:

	// Empty constructor -- for collections
	Schema() { }

	// Constructor with name only -- for Catalog search
	Schema(string _relName):
		relName(_relName) { }

	// destructor
	virtual ~Schema() { }

	// Complete constructor based on attributes, name, metadata path and
	// number of tuples -- the attribute container is left unusable.
	Schema(AttributeContainer &atts, string relName, string metadataPath,
		long int numTuples);

	// Get/Set for the relation name
	string GetRelationName();
	void SetRelationName(string relName);

	// Get/Set for the metadata path
	string GetMetadataPath();
	void SetMetadataPath(string path);

	// Get/Set for the number of tuples
	int GetNumTuples();
	void SetNumTuples(long long int tuples);

	// Returns a *copy* of the entire collection of attributes
	void GetAttributes(AttributeContainer &attsIn);

	// Returns information for a given attribute
	bool GetAttribute(string attName, Attribute &who);
	bool GetAttribute(Attribute &who);

	// Sets information for an existing attribute, if existing --
	// otherwise, replace it
	void AddAttribute(Attribute &who);

	// Returns a collection of attributes that define the primary key
	void GetPrimaryKey(AttributeContainer &who);

	// Returns a collection of attributes that are part of a foreign key
	void GetForeignKeys(AttributeContainer &who);

	// Projects a set of attributes from this schema
	void Project(AttributeContainer &toKeep);

	// Removes a set of attributes from this schema
	void Remove(AttributeContainer &toRemove);

	// Swap two schemas
	void swap(Schema &with);

	// Copy contents in another schema
	void CopyTo(Schema &here);
};

// container for Schemas
typedef TwoWayList<Schema> SchemaContainer;


// Inlined methods

inline string Schema::GetRelationName() {
	return(relName);
}

inline void Schema::SetRelationName(string _relName) {
	relName = _relName;
}

inline string Schema::GetMetadataPath() {
	return(metadataPath);
}

inline void Schema::SetMetadataPath(string _metadataPath) {
	metadataPath = _metadataPath;
}

inline int Schema::GetNumTuples() {
	return(numTuples);
}

inline void Schema::SetNumTuples(long long int _numTuples) {
	numTuples = _numTuples;
}

#endif // _SCHEMA_H
