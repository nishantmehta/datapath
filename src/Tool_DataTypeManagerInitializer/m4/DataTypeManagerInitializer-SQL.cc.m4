dnl #
dnl #  Copyright 2012 Christopher Dudley
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
COPYRIGHT_NOTICE(2012, Christopher Dudley)

#include <vector>
#include <string>
#include "MetadataDB.h"
#include "DataTypeManagerInitializer.h"

using namespace std;

void DataTypeManagerInitializer::initRelations()
{
  // Open database
  SQL_OPEN_DATABASE( GetMetadataDB() );

  // Create the table that holds the type information if it does not exist.
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo_Init (
      	    type 		 TEXT,
	    file		 TEXT,
	    convTo	 	 TEXT,
	    noExtract		 INTEGER
	);
  "/>);

  // Create the table that holds the function info if it does not exist.
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo_Init (
			func          TEXT,
			type          TEXT,
			args          TEXT,
			ret           TEXT,
			priority      INTEGER,
			assoc         INTEGER,
			pure		  INTEGER
    );
  "/>);

  // Create the table that holds the synonym info it it does not exist.
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo_Init (
			type          TEXT,
			base          TEXT
    );
  "/>);

  // Ensure that all of the relations are empty.

  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_TypeInfo_Init;
   "/>);

  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_FuncInfo_Init;
  "/>);

  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_SynonymInfo_Init;
  "/>);

  SQL_CLOSE_DATABASE;
}

void DataTypeManagerInitializer::addType( vector<TypeRecord> & types )
{
  // Open database
  SQL_OPEN_DATABASE( GetMetadataDB() );
  
  // Insert records
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_TypeInfo_Init(type, file, convTo, noExtract)
	  VALUES(?1, ?2, ?3, ?4);
  "/>, </text, text, text, int/>);

  	for( vector<TypeRecord>::const_iterator it = types.begin(); it != types.end(); ++it )
	{
		string convStr;
		
    	for( vector<string>::const_iterator iter = it->conversions.begin(); iter != it->conversions.end(); ++iter )
    	{
			convStr += *iter + " ";
    	}

    	SQL_INSTANTIATE_PARAMETERS( it->name.c_str(), it->include.c_str(), convStr.c_str(), it->noExtract );
	}

  SQL_PARAMETRIC_END;

  SQL_CLOSE_DATABASE;
}

void DataTypeManagerInitializer::addFunc( vector<FuncRecord> & funcs )
{
  // Open Database
  SQL_OPEN_DATABASE( GetMetadataDB() );

  // Create parameterized insertion statement
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_FuncInfo_Init( func, type, args, ret, priority,
      assoc, pure) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);
  "/>, </text, text, text, text, int, int, int/>);

  	for( vector<FuncRecord>::const_iterator it = funcs.begin(); it != funcs.end(); ++it )
	{
		// Convert vector of arguments to space delimited string
    	string argsStr;

    	for( vector<string>::const_iterator iter = it->args.begin();
     	     iter != it->args.end(); ++iter )
    	{
			argsStr += *iter + " ";
    	}

    	// Instantiate insertion
    	SQL_INSTANTIATE_PARAMETERS( it->name.c_str(), it->type.c_str(), argsStr.c_str(),
									it->ret.c_str(), it->priority, it->assoc, it->pure );
	}
  // End parameterized insertion statement
  SQL_PARAMETRIC_END;

  // Close database
  SQL_CLOSE_DATABASE;
}

void DataTypeManagerInitializer::addSyn( vector<SynRecord> & syns )
{
  // Open Database
  SQL_OPEN_DATABASE( GetMetadataDB() );

  // Create parameterized insertion statement
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_SynonymInfo_Init( type, base )
      VALUES (?1, ?2);
  "/>, </text, text/>);

  	for( vector<SynRecord>::const_iterator it = syns.begin(); it != syns.end(); ++it )
	{
		// Instantiate insertion
    	SQL_INSTANTIATE_PARAMETERS( it->syn.c_str(), it->base.c_str() );
	}

  // End parameterized insertion statement
  SQL_PARAMETRIC_END;

  // Close database
  SQL_CLOSE_DATABASE;
}
