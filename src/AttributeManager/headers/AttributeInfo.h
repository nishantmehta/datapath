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
#ifndef _ATTRIBUTE_INFO_H
#define _ATTRIBUTE_INFO_H

#include "AttributeType.h"
#include "AttributeInfoInternal.h"
#include "TwoWayList.h"
#include "TwoWayList.cc"

#include <string>

using namespace std;

/** Encapsulator for the information on the attributes */
class AttributeInfo {
private:
	string name;

	// relation to which attribute belongs
	string relationName;

	// type of the attribute
	string type;

	bool isSynthesized;

public:
	AttributeInfo(){ }
	AttributeInfo(AttributeInfoInternal& ainfo);
	virtual ~AttributeInfo() {}

	string& GetName(){ return name;}

	// this returns the name of the relation if base attribute or the
	// name of the query if syhthesized
	string& GetRelation(){ return relationName; }

	bool IsSynthesized(){ return isSynthesized;}
	string GetType(){ return type; }

	void swap(AttributeInfo& other);
};

// NOTE: The method swap and the constructor is implemented in AttributeManager.cc since
// it is not possible to have a pogram that does not use both. This
// way we avoid having yet another .cc file


// container to keep info about attributes in the system
typedef TwoWayList<AttributeInfo> AttributeInfoContainer;

#endif // _ATTRIBUTE_INFO_H
