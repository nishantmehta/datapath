dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #

#include "HString.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Constants.h"
#include "MetadataDB.h"


dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

/** The information in the dictionary is stored in SQLite in the relation:
		
		HDictionary ( hash:INTEGER, val:TEXT);
	  //Use GetMetadataDB() in open database later

*/

tr1::unordered_map<__uint64_t, string> HString::globalDictionary;
pthread_mutex_t HString::mutex = PTHREAD_MUTEX_INITIALIZER;
vector<HString::Dictionary*> HString::localDictionaries;

void HString::MergeAllLocalIntoGlobalDictionary() {

	for (int i = 0; i < localDictionaries.size(); i++) {

		for (Dictionary::const_iterator it = localDictionaries[i]->begin(); it != localDictionaries[i]->end(); ++it) {

			__uint64_t h = it->first;
			assert(IN_DICT(h)); // bit should be set
			Dictionary::const_iterator it_global = globalDictionary.find(h);
			if (it_global == globalDictionary.end()) { // if not found in global dictionary, add it
			 	char* myCopy = strdup((it->second).c_str());
				globalDictionary[h] = myCopy;
			} else {
				// If hash is found in global dictionary, see if strings are same. If not, just assert for now.
				// Different strings indicates we need to change our hash computing function, or we can also choose to
				// ignore the collision by making one of the frequent strings infrequent
			  WARNINGIF((it_global->second) != (it->second), "When merging local into global dictionary global entry --%s-- has the same hash %ld as new entry --%s--", (it_global->second).c_str(), h,(it->second).c_str());
			}
		}
	}
}

void HString::InitializeDictionary(void){
				
	// open database
	SQL_OPEN_DATABASE( GetMetadataDB() );

	// create relation that has the data if not existing
	SQL_STATEMENTS_NOREZ(</"
    /* HString */
    CREATE TABLE IF NOT EXISTS HDictionary (
      hash          INTEGER,
      val           TEXT
    );
	"/>);

	SQL_STATEMENT_TABLE(</"
    SELECT hash, val
    FROM HDictionary;
  "/>, </(hash, int),(val, text)/>){
		AddEntryInDictionary(hash, val);
	}SQL_END_STATEMENT_TABLE;

	// Add empty string to dictionary
        const char * tmp = "";
        AddEntryInDictionary(0, tmp);

	// close the database
	SQL_CLOSE_DATABASE;

}

void HString::SaveDictionary(void){
	if (localDictionaries.size()==0)
		return; // no change

	SQL_OPEN_DATABASE( GetMetadataDB() );

	// create relation that has the data if not existing
	SQL_STATEMENTS_NOREZ(</"
    /* HString */
    CREATE TABLE IF NOT EXISTS HDictionary (
      hash          INTEGER,
      val           TEXT
    );
	"/>);

	// delete all content
	SQL_STATEMENTS_NOREZ(</"
	  DELETE FROM HDictionary
	"/>);

	// Merge all local into global dictionary
	MergeAllLocalIntoGlobalDictionary();

	// get all the entries in the dictionary and put them in
	SQL_STATEMENT_PARAMETRIC_NOREZ(</"
		INSERT INTO HDictionary(hash, val) VALUES (?1, ?2);
	"/>, </int, text/>);
		// iterate through dictionary
		for (Dictionary::const_iterator it = globalDictionary.begin(); it != globalDictionary.end(); ++it) {
			__uint64_t hash = it->first;
			const char* val = (it->second).c_str();
			SQL_INSTANTIATE_PARAMETERS(hash, val);
		}
	SQL_PARAMETRIC_END;

	// and that is about all
	SQL_CLOSE_DATABASE;
}
