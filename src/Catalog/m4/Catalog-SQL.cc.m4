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
/** This is the SQL portion of the catalog code */

dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

#include <list>

#include "Errors.h"
#include "MetadataDB.h"
#include "Catalog.h"
#include <sstream>

/**
	 The information in the catalog is stored into the following relations:

	 CatalogRelations:
	    relation:TEXT,
			relID: INTEGER PRIMARY KEY,
			size: INTEGER

	 CatalogAttributes:
	    relID: int,
			attID: INT PRIMARY KEY, corresponds to colNo
			attribute:TEXT,
			type:TEXT,
			uniqueVals:INTEGER

	 CatalogConstants:
	    name: TEXT,
			value: TEXT
			type: TEXT  can only be: string, integer, double

 */

Catalog::Catalog(){

  // first, let us initialize all the mutexes
  pthread_mutex_init(&constMutex, NULL);
  pthread_mutex_init(&diskMutex, NULL);
  pthread_mutex_init(&schemaMutex, NULL);

	
  SQL_OPEN_DATABASE( GetMetadataDB() );

	/** create the relations if necessary */

	SQL_STATEMENTS_NOREZ(</"
	CREATE TABLE IF NOT EXISTS CatalogRelations (
		/* relation is the relation name (same as relName in table Relations)*/
		/* relID is an id for the catalog only (not related with table Relations) */
		/* size contains statistics of the relation */
		relation				TEXT,
		relID					INTEGER				PRIMARY KEY,
		size					INTEGER
	);

	CREATE TABLE IF NOT EXISTS CatalogAttributes (
		/* physical layout column */
		/* type is the system type of the attribute */
		colNo					INTEGER,
		relID					INTEGER,
		type					TEXT,
		attribute				TEXT,
		uniqueVals				INTEGER
	);

	CREATE TABLE IF NOT EXISTS CatalogConstants (
		name					TEXT,
		value					TEXT,
		type					TEXT
	);
"/>);

  /** Read the constants in the system and insert them into the internal representation */

  SQL_STATEMENT_TABLE(</"
    SELECT name, value, type
    FROM CatalogConstants;
  "/>, </(name,text), (value,text), (type,text)/>){
		
    // is it an integer?
    if (strcasecmp((char *)type, "integer") == 0) {
      int val = atoi(value);
      string valn = string(name);
			
      // add it to the map
      intConstants[valn] = val;
    }
		
    // is it a double?
    else if (strcasecmp((char *)type, "double") == 0) {
      double val = atof(value);
      string valn = string(name);
			
      // add it to the map
      doubleConstants[valn] = val;
    }
		
    // is it a string?
    else if (strcasecmp((char *)type, "string") == 0) {
      string val = string(value);
      string valn = string(name);
			
      // add it to the map
      stringConstants[valn] = val;
    }
		
    // undefined
    else {
      FATAL("unknown data type %s", type);
    }
     
  }SQL_END_STATEMENT_TABLE;


  /** Read the schema information. Relations first then the attributes */
  // first we make a list of relation IDs (quirky interface to the
  // internal datastructures of the catalog, it wants all the info on
  // a relation in a single step)

  list<int> relIds;
  SQL_STATEMENT_TABLE(</"
    SELECT relID
    FROM CatalogRelations;
  "/>, </(relID,int)/>){
    relIds.push_back(relID);
  }SQL_END_STATEMENT_TABLE;

  // now go relation by relation
  for (list<int>::iterator it = relIds.begin(); it!=relIds.end(); it++){
    string relName;
    long int numTuples;

    SQL_STATEMENT_TABLE(</"
    SELECT relation, size 
    FROM CatalogRelations
    WHERE relID=%d;
    "/>, </(name,text), (_numTuples,int)/>, </ (*it) />){
      relName = name;
      numTuples = _numTuples;
    }SQL_END_STATEMENT_TABLE;

    // get the attribute info for this relation

    AttributeContainer curAtts;
    int attIndex=0;		

    SQL_STATEMENT_TABLE(</"
      SELECT attribute, uniqueVals, type
      FROM CatalogAttributes
      WHERE relID=%d
			ORDER BY colNo;
    "/>, </(_attName,text), (attUniques, int), (typec, text)/>, 
			</ (*it) />){
			
      string attName = string(_attName);
      string attType = typec;	  
			
      Attribute att(attName, attType, attIndex++, attUniques);
      curAtts.Append(att);

    }SQL_END_STATEMENT_TABLE;

    // build the schema and add it to the catalog
    string nothing;
    Schema newSch(curAtts, relName, nothing, numTuples);
    schemas.Insert(newSch);

  }
	SQL_CLOSE_DATABASE;
}

void Catalog::SaveCatalog(){
  cout << "Saving the catalog " << endl;

	SQL_OPEN_DATABASE( GetMetadataDB() );
	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM CatalogConstants;
		"/>);

	SQL_STATEMENT_PARAMETRIC_NOREZ(</"
		INSERT INTO CatalogConstants(name, value, type) VALUES (?1, ?2, ?3);
		"/>, </text, text, text/>);
		for (map<string, int>::iterator it = intConstants.begin(); it != intConstants.end(); it++) {
			stringstream ss;
			ss << it->second;
			SQL_INSTANTIATE_PARAMETERS((it->first).c_str(), (ss.str()).c_str(), "integer");
		}
		for (map<string, double>::iterator it = doubleConstants.begin(); it != doubleConstants.end(); it++) {
			stringstream ss;
			ss << it->second;
			SQL_INSTANTIATE_PARAMETERS((it->first).c_str(), (ss.str()).c_str(), "double");
		}
		for (map<string, string>::iterator it = stringConstants.begin(); it != stringConstants.end(); it++) {
			SQL_INSTANTIATE_PARAMETERS((it->first).c_str(), (it->second).c_str(), "string");
		}
		SQL_PARAMETRIC_END;

	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM CatalogRelations ;
		"/>);

	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM CatalogAttributes;
		"/>);

	schemas.MoveToStart();
	int _relID = 0;
	while (schemas.RightLength()) {
		Schema& sch = schemas.Current();
		string relName = sch.GetRelationName();
		int numTup = sch.GetNumTuples();
		AttributeContainer attCont;
		sch.GetAttributes(attCont);

		SQL_STATEMENT_PARAMETRIC_NOREZ(</"
			INSERT INTO CatalogRelations(relation, relID, size) VALUES (?1, ?2, ?3);
			"/>, </text, int, int/>);
			_relID++;
			SQL_INSTANTIATE_PARAMETERS((relName).c_str(), _relID, numTup);
		SQL_PARAMETRIC_END;

                SQL_STATEMENT_PARAMETRIC_NOREZ(</"
                        INSERT INTO CatalogAttributes(colNo, relID, type, attribute, uniqueVals) VALUES (?1, ?2, ?3, ?4, ?5);
                        "/>, </int, int, text, text, int/>);
			attCont.MoveToStart();
			while (attCont.RightLength()) {
				Attribute& att = attCont.Current();
				int _id = att.GetIndex();
				long int _uniq = att.GetUniques();
				string typ = att.GetType();
				string na = att.GetName();
                        	SQL_INSTANTIATE_PARAMETERS(_id, _relID, (typ).c_str(), (na).c_str(), _uniq);
				attCont.Advance();									
			}
                SQL_PARAMETRIC_END;

    schemas.Advance();
	}
	SQL_CLOSE_DATABASE;

  cout << "Catalog saved " << endl;

}
