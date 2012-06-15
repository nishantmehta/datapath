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

private:
	// Attribute name.
	string name;

	// Attribute type.
	string type;

	// Attribute index inside the schema
	int index;

	// Number of unique values for this attribute
	long int uniques;

	// If so, then which relation and attributes?
	string foreignRel;
	string foreignAtt;

	// *THIS IS IMPORTANT* -- these variable store essential state
	// information about the current instance. Like having a name, type,
	// index and so on.
	bool haveName;
	bool haveType;
	bool haveIndex;
	bool haveUniques;

	// Is this attribute part of the primary key?
	bool isPrimaryKey;

	// Is this attribute part of a foreign key?
	bool isForeignKey;

	// Forbid default constructors and assignment operations
	Attribute(const Attribute &) { }

	Attribute &operator=(const Attribute &) { return(*this); }

	// The schema class is my friend.
	friend class Schema;
