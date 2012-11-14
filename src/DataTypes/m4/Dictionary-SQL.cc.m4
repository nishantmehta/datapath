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


dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

using namespace std;

void Dictionary::Load(const char* name){
    SQL_OPEN_DATABASE( GetMetadataDB() );

    // create relation that holds the data if it doesn't exist.
    SQL_STATEMENTS_NOREZ(</"
	    CREATE TABLE IF NOT EXISTS Dictionary_%s (
     	    	    id          INTEGER,
		    str           TEXT);
	"/>,</name/>);

    // Clear existing data from maps
    indexMap.clear();
    reverseMap.clear();
    
    SQL_STATEMENT_TABLE(</"
             SELECT id, str FROM Dictionary_%s;
        "/>, </(id, int), (str, text)/>,</name/>) {
      StringType s(str);
      indexMap[id] = str;
      
      if( id != InvalidID )
	reverseMap[str] = id;

      if (nextID<id)
	nextID=id;

    } SQL_END_STATEMENT_TABLE;
    
    SQL_CLOSE_DATABASE;
    
    nextID++;
    modified = false;
}

void Dictionary::Save(const char* name){
  if (!modified)
    return;
  
  SQL_OPEN_DATABASE( GetMetadataDB() );
  
  SQL_STATEMENTS_NOREZ(</"DELETE FROM Dictionary_%s"/>,</name/>);
  
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
     INSERT INTO Dictionary_%s (id, str) VALUES (?1, ?2);"/>, 
				 </int, text/>, </name/>);
    // iterate through the dictionary
    for( IndexMap::const_iterator it = indexMap.begin(); it != indexMap.end(); ++it ) {
      IntType id = it->first;
      StringType s = it->second;
      const char * str = s.c_str();
      SQL_INSTANTIATE_PARAMETERS(id, str);
    } 
  SQL_PARAMETRIC_END;
    
  SQL_CLOSE_DATABASE;
  
  modified = false;
}
