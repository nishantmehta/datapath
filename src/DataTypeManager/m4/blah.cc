

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


#include<iostream>
#include<vector>
#include<string>
#include<set>
#include<assert.h>
#include "DataTypeManager.h"
#include "MetadataDB.h"

using namespace std;

DataTypeManager* DataTypeManager::instance = NULL;

static void Tokenize (string input, vector<string>& vec) {

        const char* p = input.c_str();
        vec.clear();

        // Skip initial spaces if any
        while (*p != '\0') {
        if (*p == ' ') p++;
            else if (*p == '\0') {
                cout << "\nWrong input format";
                exit(1);
            }
            else break;
        }

        while (*p != '\0') {
            string str;
            while (*p != '\0') {
                if (*p != ' ') {str += *p; p++;}
                else break;
            }
            vec.push_back(str);

            // skip spaces
            while (*p != '\0') {
                if (*p == ' ') p++;
                else break;
            }
        }
}

void DataTypeManager::Load(void){

  // open database
  
  sqlite3 *db_INTERNAL; // database
  int rc_INTERNAL = SQLITE_OK; // error codes
	sqlite3_stmt* stmt_INTERNAL; //statement
	char* pzTail_INTERNAL; // tail pointer
	int nCol_INTERNAL; // number of columns
	const char *zLeftover_INTERNAL; // leftover for processing multiple statements
	const char *zSql_INTERNAL;
  char buffer_INTERNAL[10000];

  rc_INTERNAL = sqlite3_open(GetMetadataDB() , &db_INTERNAL);
  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db_INTERNAL));
    sqlite3_close(db_INTERNAL);
    exit(1);
  }

 // we start a transaction to make sure the code runs faster
 	zSql_INTERNAL = "BEGIN TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
 printf("\nBEGIN TRANSACTION;");
#endif
;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (            type          TEXT,            file          TEXT,            convTo        TEXT,            noExtract     INTEGER    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (            func          TEXT,            type          TEXT,            args          TEXT,            ret           TEXT,            priority      INTEGER,            assoc         INTEGER,            pure          INTEGER    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (            type          TEXT,            base          TEXT    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  
  zSql_INTERNAL = "      SELECT type,file,convTo,noExtract      FROM DataTypeManager_TypeInfo;    ";



  PDEBUG(zSql_INTERNAL);

  stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

	if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
		  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  while(true){
	  // read a row
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
 		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				break;
    }

		if (rc_INTERNAL == SQLITE_DONE) // nothing more to do
		  break;

    // read the columns
    char* type = strdup((char*)sqlite3_column_text(stmt_INTERNAL,0));
    char* file = strdup((char*)sqlite3_column_text(stmt_INTERNAL,1));
    char* convTo = strdup((char*)sqlite3_column_text(stmt_INTERNAL,2));
    long int noExtract = sqlite3_column_int64(stmt_INTERNAL,3);

    // user defined code
{
            TypeInfo* t = new TypeInfo;
            t->file = file;
            t->noExtract = noExtract;
            vector<string> vec;
            Tokenize(convTo, vec);
            for (int i = 0; i < vec.size(); i++)
                (t->convertibleTo).insert(vec[i]);
            string ts(type);
            mType[ts] = t;
    }
  }

	sqlite3_finalize(stmt_INTERNAL);
;

  
  zSql_INTERNAL = "      SELECT func,type,args,ret,priority,assoc      FROM DataTypeManager_FuncInfo;    ";



  PDEBUG(zSql_INTERNAL);

  stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

	if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
		  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  while(true){
	  // read a row
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
 		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				break;
    }

		if (rc_INTERNAL == SQLITE_DONE) // nothing more to do
		  break;

    // read the columns
    char* func = strdup((char*)sqlite3_column_text(stmt_INTERNAL,0));
    char* type = strdup((char*)sqlite3_column_text(stmt_INTERNAL,1));
    char* args = strdup((char*)sqlite3_column_text(stmt_INTERNAL,2));
    char* ret = strdup((char*)sqlite3_column_text(stmt_INTERNAL,3));
    double priority = sqlite3_column_double(stmt_INTERNAL,4);
    double assoc = sqlite3_column_double(stmt_INTERNAL,5);
    long int pure = sqlite3_column_int64(stmt_INTERNAL,6);

    // user defined code
{
            FuncInfo* f = new FuncInfo;
            f->type = type;
            Tokenize(args, f->args);
            f->returnType = ret;
            f->priority = priority;
            f->assoc = (Associativity)assoc;
            f->pure = (bool) pure;
            string fs(func);
            mFunc[fs].insert(f);
    }
  }

	sqlite3_finalize(stmt_INTERNAL);
;

  
  zSql_INTERNAL = "      SELECT type,base      FROM DataTypeManager_SynonymInfo;    ";



  PDEBUG(zSql_INTERNAL);

  stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

	if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
		  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  while(true){
	  // read a row
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
 		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				break;
    }

		if (rc_INTERNAL == SQLITE_DONE) // nothing more to do
		  break;

    // read the columns
    char* type = strdup((char*)sqlite3_column_text(stmt_INTERNAL,0));
    char* base = strdup((char*)sqlite3_column_text(stmt_INTERNAL,1));

    // user defined code
{
            string b(base);
            string t(type);
            mSynonymToBase[t] = b;
    }
  }

	sqlite3_finalize(stmt_INTERNAL);
;
  // close the database
  
  	zSql_INTERNAL = "END TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
  printf("\nEND TRANSACTION;");
#endif 
  sqlite3_close(db_INTERNAL);
;

}

void DataTypeManager::Save(void){
  //if (!modified)
  //  return;

  cout << "Saving the type information " << endl;

  
  sqlite3 *db_INTERNAL; // database
  int rc_INTERNAL = SQLITE_OK; // error codes
	sqlite3_stmt* stmt_INTERNAL; //statement
	char* pzTail_INTERNAL; // tail pointer
	int nCol_INTERNAL; // number of columns
	const char *zLeftover_INTERNAL; // leftover for processing multiple statements
	const char *zSql_INTERNAL;
  char buffer_INTERNAL[10000];

  rc_INTERNAL = sqlite3_open(GetMetadataDB() , &db_INTERNAL);
  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db_INTERNAL));
    sqlite3_close(db_INTERNAL);
    exit(1);
  }

 // we start a transaction to make sure the code runs faster
 	zSql_INTERNAL = "BEGIN TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
 printf("\nBEGIN TRANSACTION;");
#endif
;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (            type          TEXT,            file          TEXT,            convTo        TEXT,            noExtract     INTEGER    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // delete all content
  	zSql_INTERNAL = "    DELETE FROM DataTypeManager_TypeInfo  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // get all the entries and put them in
  
  zSql_INTERNAL = "    INSERT INTO DataTypeManager_TypeInfo(type, file, convTo, noExtract) VALUES (?1, ?2, ?3, ?4);  " ;

  PDEBUG(zSql_INTERNAL);

	stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
  	   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  if (sqlite3_column_count(stmt_INTERNAL) != 0)
		 fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
	  	  zSql_INTERNAL);

  // the statement gets run when parameters get instantiated
;
    // iterate through mType
      for (TypeToInfoMap::const_iterator it = mType.begin(); it != mType.end(); ++it) {
      string t = it->first;
            string f = (it->second)->file;
            // iterate and store in single space separated string
            string conv;
            for (set<string>::const_iterator iter = ((it->second)->convertibleTo).begin();
                                                                                iter != ((it->second)->convertibleTo).end(); ++iter) {
                conv += (*iter);
                conv += " ";
            }
        sqlite3_reset(stmt_INTERNAL);
  sqlite3_clear_bindings(stmt_INTERNAL);

  sqlite3_bind_text(stmt_INTERNAL, 1, t.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 2, f.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 3, conv.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int64(stmt_INTERNAL, 4, (it->second)->noExtract);

  // running the query
	rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
  if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
    fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }


;
    }
    sqlite3_finalize(stmt_INTERNAL);
;
;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (            func          TEXT,            type          TEXT,            args          TEXT,            ret           TEXT,            priority      INTEGER,            assoc         INTEGER,            pure          INTEGER    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // delete all content
  	zSql_INTERNAL = "    DELETE FROM DataTypeManager_FuncInfo  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // get all the entries and put them in
  
  zSql_INTERNAL = "    INSERT INTO DataTypeManager_FuncInfo(func, type, args, ret, priority, assoc, pure) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);  " ;

  PDEBUG(zSql_INTERNAL);

	stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
  	   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  if (sqlite3_column_count(stmt_INTERNAL) != 0)
		 fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
	  	  zSql_INTERNAL);

  // the statement gets run when parameters get instantiated
;
    // iterate through mFunc
      for (FuncToInfoMap::const_iterator it = mFunc.begin(); it != mFunc.end(); ++it) {
      string f = it->first;
      for( set<FuncInfo*>::const_iterator iter = it->second.begin(); iter != it->second.end(); ++iter)
      {
            string t = (*iter)->type;
            // iterate and store in single space separated string
            string arg;
            for (int i = 0; i < ((*iter)->args).size(); i++) {
                arg += ((*iter)->args)[i];
                arg += " ";
            }
            string r = (*iter)->returnType;
            int p = (*iter)->priority;
            int a = (*iter)->assoc;
            int purity = (*iter)->pure;

              sqlite3_reset(stmt_INTERNAL);
  sqlite3_clear_bindings(stmt_INTERNAL);

  sqlite3_bind_text(stmt_INTERNAL, 1, f.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 2, t.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 3, arg.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 4, r.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt_INTERNAL, 5, p);
  sqlite3_bind_double(stmt_INTERNAL, 6, a);
  sqlite3_bind_int64(stmt_INTERNAL, 7, purity);

  // running the query
	rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
  if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
    fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }


;
        }
    }
    sqlite3_finalize(stmt_INTERNAL);
;
;

  // create relation that has the data if not existing
  	zSql_INTERNAL = "    CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (            type          TEXT,            base          TEXT,    );  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // delete all content
  	zSql_INTERNAL = "    DELETE FROM DataTypeManager_SynonymInfo  " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

  // get all the entries and put them in
  
  zSql_INTERNAL = "    INSERT INTO DataTypeManager_SynonymInfo(type, base) VALUES (?1, ?2);  " ;

  PDEBUG(zSql_INTERNAL);

	stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
  	   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  if (sqlite3_column_count(stmt_INTERNAL) != 0)
		 fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
	  	  zSql_INTERNAL);

  // the statement gets run when parameters get instantiated
;
            for (map<string, string>::const_iterator it = mSynonymToBase.begin(); it != mSynonymToBase.end(); ++it) {
                string t = it->first;
                string b = it->second;
        sqlite3_reset(stmt_INTERNAL);
  sqlite3_clear_bindings(stmt_INTERNAL);

  sqlite3_bind_text(stmt_INTERNAL, 1, t.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt_INTERNAL, 2, b.c_str(), -1, SQLITE_TRANSIENT);

  // running the query
	rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
  if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
    fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }


;
        }
    sqlite3_finalize(stmt_INTERNAL);
;
;

  // and that is about all
  
  	zSql_INTERNAL = "END TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
  printf("\nEND TRANSACTION;");
#endif 
  sqlite3_close(db_INTERNAL);
;
}

void DataTypeManager::Initialize() {

     
  sqlite3 *db_INTERNAL; // database
  int rc_INTERNAL = SQLITE_OK; // error codes
	sqlite3_stmt* stmt_INTERNAL; //statement
	char* pzTail_INTERNAL; // tail pointer
	int nCol_INTERNAL; // number of columns
	const char *zLeftover_INTERNAL; // leftover for processing multiple statements
	const char *zSql_INTERNAL;
  char buffer_INTERNAL[10000];

  rc_INTERNAL = sqlite3_open(GetMetadataDB() , &db_INTERNAL);
  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db_INTERNAL));
    sqlite3_close(db_INTERNAL);
    exit(1);
  }

 // we start a transaction to make sure the code runs faster
 	zSql_INTERNAL = "BEGIN TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
 printf("\nBEGIN TRANSACTION;");
#endif
;

     // Create tables if non-existant.
     	zSql_INTERNAL = "        CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (            type          TEXT,            file          TEXT,            convTo        TEXT,            noExtract     INTEGER        );       " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

      	zSql_INTERNAL = "        CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (            func          TEXT,            type          TEXT,            args          TEXT,            ret           TEXT,            priority      INTEGER,            assoc         INTEGER,            pure          INTEGER        );      " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

    	zSql_INTERNAL = "        CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (            type          TEXT,            base          TEXT,        );      " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

    // Delete any data that may be in the current tables
     	zSql_INTERNAL = "        DELETE FROM DataTypeManager_TypeInfo;     " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

     	zSql_INTERNAL = "        DELETE FROM DataTypeManager_FuncInfo;     " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

     	zSql_INTERNAL = "        DELETE FROM DataTypeManager_SynonymInfo;     " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

     // Transfer data from init tables to main tables

     	zSql_INTERNAL = "        INSERT INTO DataTypeManager_TypeInfo               SELECT * FROM DataTypeManager_TypeInfo_Init;    " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

    	zSql_INTERNAL = "        INSERT INTO DataTypeManager_FuncInfo               SELECT * FROM DataTypeManager_FuncInfo_Init;    " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

    	zSql_INTERNAL = "        INSERT INTO DataTypeManager_SynonymInfo               SELECT * FROM DataTypeManager_SynonymInfo_Init;    " ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

;

    
  	zSql_INTERNAL = "END TRANSACTION;" ;



  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }


#ifdef DEBUG
  printf("\nEND TRANSACTION;");
#endif 
  sqlite3_close(db_INTERNAL);
;

     // Load data into data structures
     this->Load();
}
