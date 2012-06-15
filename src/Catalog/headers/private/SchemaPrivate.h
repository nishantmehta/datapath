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
	// Collection of attributes
	AttributeContainer attributes;

	// Relation name
	string relName;

	// Path to the metadata information
	string metadataPath;

	// Number of tuples
	long long int numTuples;

	// Find an attribute based on its name
	bool FindAttByName(string name);

	// Find an attribute based on its index
	bool FindAttByIndex(int idx);

	// Forbid default constructors and assignment operators
	Schema(const Schema &) { }

	Schema &operator=(const Schema &) { return(*this); }

	// The catalog class is my friend.
	friend class Catalog;
