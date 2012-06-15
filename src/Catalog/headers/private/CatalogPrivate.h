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
	// The constructor -- hidden here!
	Catalog(string xmlFile);

	// Map of table aliases
	map<string, string> tableAliases;

	// Map of integer constants
	map<string, int> intConstants;

	// Map of double constants
	map<string, double> doubleConstants;

	// Map of string constants
	map<string, string> stringConstants;

	// Mutex for constant information
	pthread_mutex_t constMutex;

	// Disk paths
	StringContainer diskPaths;

	// Mutex for disk paths
	pthread_mutex_t diskMutex;

	// Collection of schemas
	SchemaContainer schemas;

	// Mutex for schema information
	pthread_mutex_t schemaMutex;

	// the one instance
	static Catalog *instance;

	// This is a private method to find a schema by its name
	bool FindSchemaByName(string name);

	// Forbid copy constructors and assignment operators
	Catalog(const Catalog &){};
	Catalog &operator=(const Catalog &) { return(*this); }

	// detault constructor is private so only the singleton mechanism works
	Catalog();
