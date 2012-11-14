/** Implementation of the FactorsManager methods. Need access to the
dictionary to do this. */

#include "Dictionary.h"
#include "DictionaryManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Constants.h"
#include "MetadataDB.h"

#include <sstream>
#include <iostream>


/* This file is generated from the corresponding .cc.m4 file (look in m4/ directory)

   ANY MODIFICATIONS ARE WIPED OUT NEXT TIME maker RUNS OR CHANGES ARE MADE TO .cc.m4 file. 
	 BETTER NOT TO TOUCH -- looking is encouraged, though
*/

#include <sqlite3.h>
#include <assert.h>
#include "Debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

void Dictionary::Load(const char* name){
    
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

    // create relation that holds the data if it doesn't exist.
    	zSql_INTERNAL = "	    CREATE TABLE IF NOT EXISTS Dictionary_%s (     	    	    id          INTEGER,		    str           TEXT);	" ;


    sprintf(buffer_INTERNAL,zSql_INTERNAL, name);
		zSql_INTERNAL = buffer_INTERNAL;


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

    // Clear existing data from maps
    indexMap.clear();
    reverseMap.clear();
    
    
  zSql_INTERNAL = "             SELECT id,str FROM Dictionary_%s;        ";


    sprintf(buffer_INTERNAL,zSql_INTERNAL, name);
		zSql_INTERNAL = buffer_INTERNAL;


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
    long int id = sqlite3_column_int64(stmt_INTERNAL,0);
    char* str = strdup((char*)sqlite3_column_text(stmt_INTERNAL,1));

    // user defined code
 {
      StringType s(str);
      indexMap[id] = str;
      
      if( id != InvalidID )
	reverseMap[str] = id;

      if (nextID<id)
	nextID=id;

    } 
  }

	sqlite3_finalize(stmt_INTERNAL);
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
    
    nextID++;
    modified = false;
}

void Dictionary::Save(const char* name){
  if (!modified)
    return;
  
  
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
  
  	zSql_INTERNAL = "DELETE FROM Dictionary_%s" ;


    sprintf(buffer_INTERNAL,zSql_INTERNAL, name);
		zSql_INTERNAL = buffer_INTERNAL;


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
  
  
  zSql_INTERNAL = "     INSERT INTO Dictionary_%s (id, str) VALUES (?1, ?2);" ;


    sprintf(buffer_INTERNAL,zSql_INTERNAL, name);
		zSql_INTERNAL = buffer_INTERNAL;



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
    // iterate through the dictionary
    for( IndexMap::const_iterator it = indexMap.begin(); it != indexMap.end(); ++it ) {
      IntType id = it->first;
      StringType s = it->second;
      const char * str = s.c_str();
        sqlite3_reset(stmt_INTERNAL);
  sqlite3_clear_bindings(stmt_INTERNAL);

  sqlite3_bind_int64(stmt_INTERNAL, 1, id);
  sqlite3_bind_text(stmt_INTERNAL, 2, str, -1, SQLITE_TRANSIENT);

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
  
  modified = false;
}
