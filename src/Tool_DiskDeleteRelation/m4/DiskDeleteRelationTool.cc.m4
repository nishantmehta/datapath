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
dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE


#include<iostream>
#include<string.h>
#include<set>

using namespace std;

void printError() {
    cout << "\nError : Wrong arguments, only below commands are correct";
    cout << "\n<executable-name> -rel <int ID>";
    cout << "\n<executable-name> -list";
}

void DeleteRelation(int relID) {
	// Open the database;
	SQL_OPEN_DATABASE( "disktest.db" );

  // delete the relation
  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DiskAllocatorState
    WHERE relID=%d;
  "/>,</relID/>);

	// Close the database;
	SQL_CLOSE_DATABASE;
}

void ListRelation() {
	// Open the database;
	SQL_OPEN_DATABASE( "disktest.db" );

	// Get the relations from database
	set<int> rel;
  SQL_STATEMENT_TABLE(</"
      SELECT relID
      FROM DiskAllocatorState;
    "/>, </(relID, int)/>){
			rel.insert(relID);
    }SQL_END_STATEMENT_TABLE;

	// Print the relations
	cout << "\nRelations :";
	for (set<int>::iterator it = rel.begin(); it != rel.end(); ++it) {
		cout << " " << (*it);
	}
	cout << endl;

	// Close the database;
	SQL_CLOSE_DATABASE;
}

int main(int argc, char** argv) {

	if (argc == 2) {
		if (strcmp(argv[1], "-list") == 0) {
			ListRelation();
			return 0;
		} else {
			printError();
			return 1;
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "-rel") == 0) {
			int relID = atoi(argv[2]);
			DeleteRelation(relID);
			return 0;
		} else {
			printError();
			return 1;
		}
	} else {
			printError();
			return 1;
	}
}
