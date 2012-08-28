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
#include "AttributeManager.h"
#include "AttributeInfo.h"
#include "QueryManager.h"
#include "Catalog.h"
#include "Errors.h"
#include "DataTypeManager.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/** Todo: fix the deletion to erase info from reverseMap */

// instance default value
AttributeManager* AttributeManager::instance = NULL;


AttributeInfoInternal :: AttributeInfoInternal(string _relationName, string _name,
	string _type, int _column, int _slot){
	name = REL_ATT_KEY(_relationName, _name);
	type=_type;

	relationName = _relationName;

	SlotID slotToAdd(_slot);
	slot= slotToAdd;

	SlotID columnToAdd(_column);
	column = columnToAdd;
}


AttributeInfoInternal::AttributeInfoInternal(QueryID& id, string _name,
	string _type, int _slot){
	QueryManager& qm=QueryManager::GetQueryManager();
	string qName;
	if (!qm.GetQueryName(id,qName)){
		FATAL("Query is not registered!");
	}

	// get attribute full name by adding query id to it
	name = SYN_ATT_KEY(qName,_name);
	type=_type;

	relationName = "";
	SlotID slotToAdd(_slot);
	slot = slotToAdd;
}


AttributeManager::AttributeManager() {
	//initialize the mutex
	pthread_mutex_init (&attributeManagerMutex, NULL);

	firstSynthSlot = FIRST_NONRESERVED_SLOT;

	//fill the manager with attributes from catalog
	FillInAttributesFromCatalog();
}

AttributeManager::~AttributeManager() {
	//destroy the mutex
	pthread_mutex_destroy (&attributeManagerMutex);

	//free all the used memory
	myAttributes.clear();
	reverseMap.clear();

	relationToSlots.clear();

	queryIdToAttributes.clear();
}

AttributeManager& AttributeManager::GetAttributeManager() {
	//is instance already created?
	if(instance == NULL)
	{
		//creating it once
		instance = new AttributeManager();
	}
	return *instance;
}

int AttributeManager::NextEmptySlot(int from) {
	// stupid linear scan to find the next empty slot
	// we do not do this too often so this should be fine
	int slot;

	int slotsNum = slotUsage.size();
	for (slot=from; slot<slotsNum; slot++) {
		if (slotUsage[slot] == false)
			break;
	}

	// if slot ends up with value size() than we insert a new element
	// this results in having infinite slots.. Need to change logic, if fixed number
	// of slots are needed
	if(slot == slotsNum) {
		slotUsage.push_back(false);
	}

	// need to compensate for permanently used slots
	return slot+firstSynthSlot;
}

void AttributeManager::FillInAttributesFromCatalog() {
	// the catalog should be initialized by now
	// get access to the catalog
	Catalog& catalog=Catalog::GetCatalog();

	// var to keep track of the next slot to use
	int slot=FIRST_NONRESERVED_SLOT;

	// get the relation names
	StringContainer relNames = catalog.GetRelationNames();

	// iterate over relations
	for (unsigned int i=0; i<relNames.size(); i++) {
		string thisRel = relNames[i];

		// ask for the schema of this relation
		Schema schema;
		catalog.GetSchema(thisRel,schema);

		//creating map for holding attribute's column and slot mapping
		SlotToSlotMap* slotToSlotMap = new SlotToSlotMap();

		//container for holding attributes from schema
		AttributeContainer attributes;
		schema.GetAttributes(attributes);

		attributes.MoveToStart();

		int columnNo = 0;
		// iterate over the attributes from schema.
		// here its assumed that attributes are coming in right order
		// as defined in schema
		while(!attributes.AtEnd()) {
			//get each attribute
			Attribute& attribute = attributes.Current();
			attributes.Advance();

			//retrieve information of each attribute of schema
			string attName = attribute.GetName();
			string attributeType = attribute.GetType();

			//now create AttributeInfoInternal from Attribute's information
			AttributeInfoInternal* att = new AttributeInfoInternal(thisRel, attName,
				attributeType, columnNo++, slot);
			slot++;

			SlotID slotToAdd(att->Slot());
			SlotID slotCopy = slotToAdd;
			SlotID columnToAdd(att->Column());

			// add into the map key as attribute's column and value as its slot
			slotToSlotMap->Insert(columnToAdd, slotToAdd);

			// create a key of form: relationname_attname
			string attKey = att->Name();

			// add attribute information to the container
			myAttributes[attKey] = att;
			reverseMap[slotCopy] = attKey;
		}

		//insert mapping between the relation name and ColumnToSlot mapping
		relationToSlots.insert(pair<string,SlotToSlotMap*>(thisRel,slotToSlotMap));
	}

	//set the first slot available for synthesized attributes
	firstSynthSlot = slot;
}

SlotID AttributeManager::AddSynthesizedAttribute(QueryID id, string attName,
	string attType) {

	SlotID rez; // put the slotID of the attribute added here

	// get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	//create new attribute for the new synthesized attribute
	AttributeInfoInternal* att = new AttributeInfoInternal(id, attName, attType);

	// get key similar to synthesized attribute name
	string attKey = att->Name();

	// flag for checking if attribute is added to the system
	bool isAttributeAdded = false;

	// find if earlier attributes of same query id were added
	QueryIDToAttributesMap::iterator qtoaMapItr;
	qtoaMapItr = queryIdToAttributes.find(id);

	// if yes, then add this new attribute to that list
	if(qtoaMapItr != queryIdToAttributes.end())
	{
		StringContainer& attributes = (*qtoaMapItr).second;

		// check if attribute with same name is already present for the query id
		if(find(attributes.begin(), attributes.end(), attKey) == attributes.end())
		{
			attributes.push_back(attKey);
			isAttributeAdded = true;
		}
		else
		{
			WARNING("Attribute with same name already present for the query id.");
		}
	}
	else
	{
		// if attributes for this queryid are added for first time,
		// then create a new list of attributes, add this attribute
		//StringContainer* attributes = new StringContainer();
		StringContainer attributes;
		attributes.push_back(attKey);
		// add query id and list as a pair in the map
		QueryID* keyID = new QueryID();
		keyID->copy(id);

		queryIdToAttributes.insert(pair<QueryID,StringContainer>(*keyID,attributes) );
		isAttributeAdded = true;
	}

	// now if attribute is added, then add it to map myAttributes,
	// which maintains all attributes in the system
	if(isAttributeAdded)
	{
		// get next empty slot
		int nextSlot=NextEmptySlot(0);
		SlotID slotToAdd(nextSlot);
		SlotID slotCopy = slotToAdd;
		rez=slotToAdd;

		att->SetSlot(slotToAdd);

		// add attribute information to the container
		myAttributes[attKey] = att;
		reverseMap[slotCopy] = attKey;

		slotUsage.at(nextSlot-firstSynthSlot) = true;
	}

	//return the mutex
	pthread_mutex_unlock (&attributeManagerMutex);

	return rez;
}


SlotID AttributeManager::GetAttributeSlot(string attributeLongName){
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);
	// find the attribute in the map
	AttributeNameInfoMap::iterator attItr = myAttributes.find(attributeLongName);

	// if attribute is found
	if(attItr != myAttributes.end())
	{
		// get attribute info
		AttributeInfoInternal* att = (*attItr).second;

		// create a new SlotID instance with slot of the attribute
		SlotID slotID = att->Slot();

		//return the mutex
		pthread_mutex_unlock(&attributeManagerMutex);

		return slotID;
	}
	else
	{
		// attribute is not found, this may be due to wrong relation or attribute name
		FATAL("No attribute found with name %s!", attributeLongName.c_str())
	}
}

string AttributeManager::GetAttributeName(SlotID slot){
	string rez; // the result
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	// find the attribute in the map
	AttributeSlotToNameMap::iterator attItr = reverseMap.find(slot);

	// if attribute is found
	if(attItr != reverseMap.end())
	{
		// get attribute info
		rez = (*attItr).second;

		//return the mutex
		pthread_mutex_unlock(&attributeManagerMutex);

		return rez;
	}
	else
	{
		// attribute is not found, this may be due to wrong relation or attribute name
		FATAL("No attribute found with slot %i!", (int)slot)
	}
}


SlotID AttributeManager::GetAttributeSlot(string tableName, string attributeName)
{
	// create the key for base relation attribute
	string attLongName = REL_ATT_KEY(tableName, attributeName);

	return GetAttributeSlot(attLongName);
}

void AttributeManager::GetAttributesSlots(string tableName, SlotContainer& where){
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	RelationToSlotsMap::iterator mapItr;

	//find ColumnToSlot mapping for given relation
	mapItr = relationToSlots.find(tableName);

	//if mapping is found
	if(mapItr != relationToSlots.end())
	{
		//copy the mapping information
		SlotToSlotMap& slotMap = *((*mapItr).second);
		slotMap.MoveToStart();
		while (!slotMap.AtEnd()){
			SlotID slot = slotMap.CurrentData();
			where.Append(slot);
			slotMap.Advance();
		}

	}
	else
	{
		FATAL("No map found for relation %s!",tableName.c_str());
	}

	// done with critical region
	pthread_mutex_unlock(&attributeManagerMutex);
}


string AttributeManager::GetAttributeType(string longName){
	string rez; /* initially empty */

	pthread_mutex_lock (&attributeManagerMutex);

	// find the attribute in the map
	AttributeNameInfoMap::iterator attItr = myAttributes.find(longName);

	// if attribute is found
	if(attItr != myAttributes.end())
	{
		// get attribute info
		AttributeInfoInternal* att = (*attItr).second;

		// create a new SlotID instance with slot of the attribute
		rez=att->Type();

	}

	//return the mutex
	pthread_mutex_unlock(&attributeManagerMutex);

	return rez;

}


SlotID AttributeManager::GetAttributeSlot(QueryID id, string attributeName)
{

	QueryManager& qm=QueryManager::GetQueryManager();
	string qName;
	if (!qm.GetQueryName(id,qName)){
		FATAL("Query is not registered!");
	}

	// create the key for synthesized attribute
	string attLongName = SYN_ATT_KEY(qName, attributeName);

	return GetAttributeSlot(attLongName);
}


SlotID AttributeManager::GetAttributeColumn(string tableName, string attributeName){
	string attLongName=REL_ATT_KEY(tableName, attributeName);
	return GetAttributeColumn(attLongName);
}

SlotID AttributeManager::GetAttributeColumn(string attLongName){
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	// find the attribute in the map
	AttributeNameInfoMap::iterator attItr = myAttributes.find(attLongName);

	// if attribute is found
	if(attItr != myAttributes.end())
	{
		// get attribute info
		AttributeInfoInternal* att = (*attItr).second;

		// create a new SlotID instance with column of the attribute
		SlotID slotID = att->Column();

		//return the mutex
		pthread_mutex_unlock(&attributeManagerMutex);

		return slotID;
	}
	else
	{
		FATAL("No attribute found with name %s!", attLongName.c_str());
	}
}


void AttributeManager::GetColumnToSlotMapping(string tableName, SlotToSlotMap& where) {

	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	RelationToSlotsMap::iterator mapItr;

	//find ColumnToSlot mapping for given relation
	mapItr = relationToSlots.find(tableName);

	//if mapping is found
	if(mapItr != relationToSlots.end())
	{
		//copy the mapping information
		where.copy(*((*mapItr).second));
	}
	else
	{
		FATAL("No map found for relation %s!",tableName.c_str());
	}

	//return the mutex
	pthread_mutex_unlock(&attributeManagerMutex);
}

void AttributeManager::DeleteAttributes(QueryID queryid) {
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	// find the query id in the map
	QueryIDToAttributesMap::iterator qtoaMapItr;
	qtoaMapItr = queryIdToAttributes.find(queryid);

	// if present, then remove all the attributes of that query from
	// queryIdToAttributes map and also from myAttributes
	if(qtoaMapItr != queryIdToAttributes.end())
	{
		StringContainer& attributes = (*qtoaMapItr).second;
		StringContainer::iterator attribItr;

		// remove all the synthesized attributes of this queryid from myAttributes
		for(attribItr = attributes.begin(); attribItr != attributes.end(); attribItr++)
		{
			string attribName = (*attribItr);
			// get the attribute info
			AttributeInfoInternal* attributeInfo = (*(myAttributes.find(attribName))).second;
			SlotID attribSlot = attributeInfo->Slot();

			// free the slot of deleted attribute
			slotUsage.at(attribSlot -firstSynthSlot) = false;

			// now delete the attribute from global attribute container
			myAttributes.erase(attribName);
		}

		// free memory occupied by query's attribute container
		attributes.clear();
		// delete attributes;
		//attributes = NULL;

		// now remove entry for the query id
		queryIdToAttributes.erase(queryid);
	}
	else
	{
		WARNING("No Attributes present for given query id %d!", queryid.GetInt());
	}

	//return the mutex
	pthread_mutex_unlock(&attributeManagerMutex);
}

void AttributeManager::GenerateM4Files(string fullFileName) {
	//get the mutex
	pthread_mutex_lock (&attributeManagerMutex);

	fstream attributesM4;
	attributesM4.open(fullFileName.c_str(), fstream::out | fstream::trunc);

	string m4_define = "m4_define";

    DataTypeManager& dTM = DataTypeManager::GetDataTypeManager();

	//first write information of base relations' attributes

	//write attribute type information
	for (AttributeNameInfoMap::iterator it=myAttributes.begin();
		it!=myAttributes.end(); it++){
		AttributeInfoInternal* attributeInfo = (*it).second;
		string attType = (*attributeInfo).Type();
        // Make sure it's the base type!
        // Don't really care if it is actually defined as a type right now
        // as we might not be using the attributes that have not had their type
        // defined. If we do try to do that, the parser will catch it.
        dTM.IsType(attType);

		attributesM4 << m4_define << "(" <<  "ATT_TYPE_" << (*attributeInfo).Name()
			<< "," << attType  << ")" << endl;
	}

	//write attribute slot information
	for (AttributeNameInfoMap::iterator it=myAttributes.begin();
		it!=myAttributes.end(); it++){

		AttributeInfoInternal* attributeInfo = (*it).second;
		SlotID slotId = (*attributeInfo).Slot();

		SlotInfo slotInfo(1);
		slotId.getInfo(slotInfo);
		string id = slotInfo.getIDAsString();

		attributesM4 << m4_define << "(" <<  "ATT_SLOT_" << (*attributeInfo).Name()
			<< "," << id  << ")" << endl;
	}

	attributesM4.close();

	//return the mutex
	pthread_mutex_unlock(&attributeManagerMutex);
}
