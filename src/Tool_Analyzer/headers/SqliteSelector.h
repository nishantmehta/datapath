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
#ifndef SQLITESELECTOR_H
#define	SQLITESELECTOR_H

#include "DotFileGenerator.h"

#include <sqlite3.h>

class SqliteSelector {
private:
	sqlite3 *logDB;			//pointer to the database
	sqlite3_stmt *preStmtSEL;	//prepared statement, SELECT syntax
	const char* zSQL_SEL;		//helper for preStmtSEL

	DotFileGenerator dfGen;

public:
	SqliteSelector();
    void DataSelector(string sender, string receiver, string msgtype, double tsf, double tst, double dless, double dmore, int maxduration, int minduration, bool screen, bool all);
};

#endif	/* SQLITESELECTOR_H */

