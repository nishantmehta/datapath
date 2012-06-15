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
#ifndef _ATTRIBUTEMANAGER_H_
#define _ATTRIBUTEMANAGER_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <sstream>
#include <algorithm>

#include "ID.h"
#include "Errors.h"
#include "AttributeInfoInternal.h"
#include "ContainerTypes.h"
#include "AttributeInfo.h"


// How may slots are reserved (indicates first available one)
// The researved slots are used by the Chunks for special purposes
// This number has to be in sync with the macros in Chunk.h
#define FIRST_NONRESERVED_SLOT 2

using namespace std;


// forward definitions
class AttributeInfo;


/**
	* This class maintains the information about attributes present in the base
	* relations as well sythesized attributes. It maintains information such as
	* relation to which attribute belong, its slots and type.
**/

class AttributeManager {

#include "AttributeManagerPrivate.h"

public:
	// Destructor
	virtual ~AttributeManager(void);

	static AttributeManager& GetAttributeManager();

	/////////////////////////
	// METHODS TO ADD ATTRIBUTES

	// allocate slot for synthesized attribute
	// return the slot allocated
	SlotID AddSynthesizedAttribute(QueryID id, string attName, string attType);

	// allocate slots for synthesized attributes
	// void AddSynthesizedAttributes(QueryID& id, StringContainer& atts);

	//////////////////////////
	// METHODS TO GET INFORMATION ABOUT INDIVIDUAL ATTRIBUTES

	// retrieves the slot for given attribute
	SlotID GetAttributeSlot(string attLongName);
	SlotID GetAttributeSlot(string tableName, string attributeName);

	// return empty string if not found (do not fail)
	string GetAttributeType(string attLongName);
	string GetAttributeType(string tableName, string attributeName);

	// retrieves the slot for all attributes
	void GetAttributesSlots(string tableName, SlotContainer& where);

	// retrieves the slot for given sythesized attribute
	SlotID GetAttributeSlot(QueryID id, string attributeName);

	// retrieves the column slot for given attribute
	SlotID GetAttributeColumn(string tableName, string attributeName);
	SlotID GetAttributeColumn(string attLongName);

	// retrieve the name of an attribute by SlotID
	string GetAttributeName(SlotID slot);

	/////////////////////////
	// METHODS TO GET INFORMATION ABOUT GROUPS OF ATTRIBUTES

	// extract information about the attributes in the system
	void GetInfoOnAllAttributes(AttributeInfoContainer& where);

	// returns column to slot mapping for given relation
	// the result is placed in where
	void GetColumnToSlotMapping(string tableName, SlotToSlotMap& where);

	// deletes synthesized attributes present in system for given query id
	void DeleteAttributes(QueryID queryid);

	// generates m4 file having attribute information
	void GenerateM4Files(string fullFileName);

	friend class AttributeInfo;
};

#endif
