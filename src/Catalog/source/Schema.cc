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

#include "Schema.h"
#include "Errors.h"

#include <string>

using namespace std;


Schema::Schema(AttributeContainer &_atts, string _relName, string _metadataPath,
	long int _numTuples):
	relName(_relName), metadataPath(_metadataPath), numTuples(_numTuples) {
	// suck in all the attributes
	attributes.swap(_atts);
}

bool Schema::FindAttByName(string name) {
	// keep in mind that this method is private

	// first, move to the beginning
	attributes.MoveToStart();

	// loop
	bool found = false;
	while (!found && !attributes.AtEnd()) {
		// check if we found it
		if (attributes.Current().GetName() == name)
			found = true;
		// otherwise, advance
		else
			attributes.Advance();
	}

	// keep the position for someone else to work with it.
	return(found);
}

bool Schema::FindAttByIndex(int idx) {
	// keep in mind that this method is private

	// first, move to the beginning
	attributes.MoveToStart();

	// check the length -- return false if not here
	if (idx >= attributes.Length())
		return(false);

	// loop
	bool found = false;
	while (!found && !attributes.AtEnd()) {
		// check if we found it
		if (attributes.Current().GetIndex() == idx)
			found = true;
		// otherwise, advance
		else
			attributes.Advance();
	}

	return(found);
}

bool Schema::GetAttribute(string relName, Attribute &who) {
	// first, we'll check what kind of information this attribute has
	// for us to search

	bool foundAtt = FindAttByName(relName);

	// now, let's populate the parameter if we found something
	if (foundAtt) {
		attributes.Current().CopyTo(who);
	}

	// and return
	return(foundAtt);
}

bool Schema::GetAttribute(Attribute &who) {

	// first, we'll check what kind of information this attribute has
	// for us to search

	bool foundAtt;

	// do we have an attribute index? if so, search for it
	if (who.haveIndex)
		foundAtt = FindAttByIndex(who.GetIndex());
	// do we have an attribute name? if so, search for it
	else if (who.haveName)
		foundAtt = FindAttByName(who.GetName());
	// otherwise? well, can't find.
	else {
		WARNING("Not enough information to find attribute.");
		return(false);
	}

	// now, let's populate the parameter if we found something
	if (foundAtt) {
		attributes.Current().CopyTo(who);
	}

	// and return
	return(foundAtt);
}

void Schema::AddAttribute(Attribute &who) {
	// first, we'll check what kind of information this attribute has
	// for us to search

	bool foundAtt;

	// do we have an attribute index? if so, search for it
	if (who.haveIndex)
		foundAtt = FindAttByIndex(who.GetIndex());
	// do we have an attribute name? if so, search for it
	else if (who.haveName)
		foundAtt = FindAttByName(who.GetName());
	// otherwise? well, can't find.
	else {
		WARNING("Not enough information to find attribute.");
		return;
	}

	// did we find it? good, swap it in
	if (foundAtt)
		who.swap(attributes.Current());
	else
		// otherwise, insert it
		attributes.Append(who);
}

void Schema::GetPrimaryKey(AttributeContainer &where) {
	// go through the structure, checking attributes and inserting
	for (attributes.MoveToStart(); !attributes.AtEnd(); attributes.Advance()) {
		if (attributes.Current().IsPrimaryKey()) {
			Attribute newGuy;
			attributes.Current().CopyTo(newGuy);
			where.Append(newGuy);
		}
	}
}

void Schema::GetForeignKeys(AttributeContainer &who) {
	// go through the structure, checking attributes and inserting
	for (attributes.MoveToStart(); !attributes.AtEnd(); attributes.Advance()) {
		if (attributes.Current().IsForeignKey()) {
			Attribute newGuy;
			attributes.Current().CopyTo(newGuy);
			who.Append(newGuy);
		}
	}
}

void Schema::Project(AttributeContainer &toKeep) {
	// first, let's do a sanity check and see if all the attributes are
	// present -- this is redundant, but makes sure we don't damage the
	// structure if only certain attributes are projected.

	for (toKeep.MoveToStart();!toKeep.AtEnd();toKeep.Advance()) {

		bool found;
		if (toKeep.Current().haveIndex)
			found = FindAttByIndex(toKeep.Current().GetIndex());
		else if (toKeep.Current().haveName)
			found = FindAttByName(toKeep.Current().GetName());
		else {
			WARNING("Malformed attribute collection.");
			return;
		}

		if (!found) {
			WARNING("An attribute was not found in the schema.");
			return;
		}
	}

	// now that we have checked this, let's go!

	AttributeContainer newAtts;

	for (toKeep.MoveToStart();!toKeep.AtEnd();toKeep.Advance()) {
		if (toKeep.Current().haveIndex)
			FindAttByIndex(toKeep.Current().GetIndex());
		else if (toKeep.Current().haveName)
			FindAttByName(toKeep.Current().GetName());

		Attribute newGuy;
		attributes.Current().CopyTo(newGuy);

		newAtts.Append(newGuy);
	}

	// now, swap the structures and we're done
	newAtts.swap(attributes);
}

void Schema::Remove(AttributeContainer &toRemove) {
	// first, let's do a sanity check and see if all the attributes are
	// present -- this is redundant, but makes sure we don't damage the
	// structure if only certain attributes are projected.

	for (toRemove.MoveToStart();!toRemove.AtEnd();toRemove.Advance()) {
		bool found;
		if (toRemove.Current().haveIndex)
			found = FindAttByIndex(toRemove.Current().GetIndex());
		else if (toRemove.Current().haveName)
			found = FindAttByName(toRemove.Current().GetName());
		else {
			WARNING("Malformed attribute collection.");
			return;
		}

		if (!found) {
			WARNING("An attribute was not found in the schema.");
			return;
		}
	}


	// now that we have checked this, let's go!
	for (toRemove.MoveToStart();!toRemove.AtEnd();toRemove.Advance()) {
		if (toRemove.Current().haveIndex)
			FindAttByIndex(toRemove.Current().GetIndex());
		else if (toRemove.Current().haveName)
			FindAttByName(toRemove.Current().GetName());

		// remove the current attribute
		Attribute dummy;
		attributes.Remove(dummy);
	}
}

void Schema::GetAttributes(AttributeContainer &attsIn) {
	// go through all attributes, copying in.
	for (attributes.MoveToStart();!attributes.AtEnd();attributes.Advance()) {
		Attribute newAtt;
		attributes.Current().CopyTo(newAtt);
		attsIn.Append(newAtt);
	}
}

void Schema::swap(Schema &with) {
	// swap the attributes
	attributes.swap(with.attributes);

	// swap the relation name
	string auxName = relName;
	relName = with.relName;
	with.relName = auxName;

	// swap the metadata path
	string auxPath = metadataPath;
	metadataPath = with.metadataPath;
	with.metadataPath = auxPath;

	// swap the number of tuples
	int auxTuples = numTuples;
	numTuples = with.numTuples;
	with.numTuples = auxTuples;
}

void Schema::CopyTo(Schema &here) {
	// write the attributes.
	AttributeContainer newAtts;
	for (attributes.MoveToStart();!attributes.AtEnd();attributes.Advance()) {
		// make a copy and write it in
		Attribute newAtt;
		attributes.Current().CopyTo(newAtt);

		// insert
		newAtts.Append(newAtt);
	}

	// swap with collection
	newAtts.swap(here.attributes);

	// now copy the rest of the values
	here.relName = relName;
	here.metadataPath = metadataPath;
	here.numTuples = numTuples;
}
