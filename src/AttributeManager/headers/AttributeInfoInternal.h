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
#ifndef _ATTRIBUTE_INFO_INTERNAL_
#define _ATTRIBUTE_INFO_INTERNAL_

#include <string>

#include "AttributeType.h"
#include "ID.h"

#define REL_ATT_KEY(relationName, attName) relationName + "_" + attName
#define SYN_ATT_KEY(qName, attName) qName + "_" + attName

using namespace std;


/** This class holds details of the attribute including its name, and slot information.
	* This class is used privately by AttributeManager.
**/
class AttributeInfoInternal {
private:
	// attribute name
	string name;

    // base attribute name without table or query prefix
    string shortName;

	// relation to which attribute belongs
	string relationName;

	// type of the attribute
	string type;

	// attribute slot
	SlotID slot;

	// attribute column
	SlotID column;

public:
	// constructor for attributes from base relations
	AttributeInfoInternal(string _relationName, string _name, string _type,
		int _column, int _slot = -1);

	// constructor for synthesized attributes
	AttributeInfoInternal(QueryID& id, string _name, string _type, int _slot = -1);

	// destructor
	virtual ~AttributeInfoInternal() {}

	// setter method of slot
	void SetSlot(SlotID _slot){ slot=_slot; }

	// setter method for column
	void SetColumn(SlotID _column){ column = _column; }

	// access methods
	string ShortName(void) { return shortName; }
	string Name(void){ return name;}
	string Type(void){ return type;}

	SlotID Slot(void){ return slot; }

	SlotID Column(void){ return column; }
};

#endif // _ATTRIBUTE_INFO_INTERNAL_
