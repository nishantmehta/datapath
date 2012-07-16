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
COPYRIGHT_NOTICE(2012, Alin Dobra and Christopher Jermaine)

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
  SQL_OPEN_DATABASE( GetMetadataDB() );

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (
            type          TEXT,
            file          TEXT,
            convTo        TEXT,
            noExtract     INTEGER
    );
  "/>);

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (
            func          TEXT,
            type          TEXT,
            args          TEXT,
            ret           TEXT,
            priority      INTEGER,
            assoc         INTEGER,
            pure          INTEGER
    );
  "/>);

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (
            type          TEXT,
            base          TEXT
    );
  "/>);

  // create relation for the function templates if it doesn't exist
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncTempInfo (
            func        TEXT,
            ret         TEXT,
            file        TEXT
    );
  "/>);

  SQL_STATEMENT_TABLE(</"
      SELECT type, file, convTo, noExtract
      FROM DataTypeManager_TypeInfo;
    "/>, </(type, text),(file, text),(convTo, text), (noExtract, int)/>){
            TypeInfo* t = new TypeInfo;
            t->file = file;
            t->noExtract = noExtract;
            vector<string> vec;
            Tokenize(convTo, vec);
            for (int i = 0; i < vec.size(); i++)
                (t->convertibleTo).insert(vec[i]);
            string ts(type);
            mType[ts] = t;
    }SQL_END_STATEMENT_TABLE;

  SQL_STATEMENT_TABLE(</"
      SELECT func, type, args, ret, priority, assoc
      FROM DataTypeManager_FuncInfo;
    "/>, </(func, text),(type, text),(args, text),(ret, text),(priority, double),(assoc, double),(pure, int)/>){
            FuncInfo* f = new FuncInfo;
            f->type = type;
            Tokenize(args, f->args);
            f->returnType = ret;
            f->priority = priority;
            f->assoc = (Associativity)assoc;
            f->pure = (bool) pure;
            string fs(func);
            mFunc[fs].insert(f);
    }SQL_END_STATEMENT_TABLE;

  SQL_STATEMENT_TABLE(</"
      SELECT type, base
      FROM DataTypeManager_SynonymInfo;
    "/>, </(type, text),(base, text)/>){
            string b(base);
            string t(type);
            mSynonymToBase[t] = b;
    }SQL_END_STATEMENT_TABLE;

    SQL_STATEMENT_TABLE(</"
        SELECT func, ret, file
        FROM DataTypeManager_FuncTempInfo;
    "/>, </(func, text), (ret, text), (file, text)/>) {
        FuncTemplateInfo fInfo( ret, file );
        mTempFunc[func] = fInfo;
    }SQL_END_STATEMENT_TABLE;
  // close the database
  SQL_CLOSE_DATABASE;

}

void DataTypeManager::Save(void){
  //if (!modified)
  //  return;

  cout << "Saving the type information " << endl;

  SQL_OPEN_DATABASE( GetMetadataDB() );

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (
            type          TEXT,
            file          TEXT,
            convTo        TEXT,
            noExtract     INTEGER
    );
  "/>);

  // delete all content
  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_TypeInfo
  "/>);

  // get all the entries and put them in
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_TypeInfo(type, file, convTo, noExtract) VALUES (?1, ?2, ?3, ?4);
  "/>, </text, text, text, int/>);
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
      SQL_INSTANTIATE_PARAMETERS(t.c_str(), f.c_str(), conv.c_str(), (it->second)->noExtract);
    }
  SQL_PARAMETRIC_END;

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (
            func          TEXT,
            type          TEXT,
            args          TEXT,
            ret           TEXT,
            priority      INTEGER,
            assoc         INTEGER,
            pure          INTEGER
    );
  "/>);

  // delete all content
  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_FuncInfo
  "/>);

  // get all the entries and put them in
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_FuncInfo(func, type, args, ret, priority, assoc, pure) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);
  "/>, </text, text, text, text, double, double, int/>);
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

            SQL_INSTANTIATE_PARAMETERS(f.c_str(), t.c_str(), arg.c_str(), r.c_str(), p, a, purity);
        }
    }
  SQL_PARAMETRIC_END;

  // create relation that has the data if not existing
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (
            type          TEXT,
            base          TEXT,
    );
  "/>);

  // delete all content
  SQL_STATEMENTS_NOREZ(</"
    DELETE FROM DataTypeManager_SynonymInfo
  "/>);

  // get all the entries and put them in
  SQL_STATEMENT_PARAMETRIC_NOREZ(</"
    INSERT INTO DataTypeManager_SynonymInfo(type, base) VALUES (?1, ?2);
  "/>, </text, text/>);
            for (map<string, string>::const_iterator it = mSynonymToBase.begin(); it != mSynonymToBase.end(); ++it) {
                string t = it->first;
                string b = it->second;
      SQL_INSTANTIATE_PARAMETERS(t.c_str(), b.c_str());
        }
  SQL_PARAMETRIC_END;

  // create relation for the function templates if it doesn't exist
  SQL_STATEMENTS_NOREZ(</"
    CREATE TABLE IF NOT EXISTS DataTypeManager_FuncTempInfo (
            func        TEXT,
            ret         TEXT,
            file        TEXT
    );
  "/>);

    SQL_STATEMENT_PARAMETRIC_NOREZ(</"
        INSERT INTO DataTypeManager_FuncTempInfo(func, ret, file) VALUES (?1, ?2, ?3);
    "/>, </text, text, text/>);
        for(FuncTempToInfoMap::iterator it = mTempFunc.begin(); it != mTempFunc.end(); ++it ) {
            const char * func = it->first.c_str();
            const char * ret = it->second.retType.c_str();
            const char * file = it->second.file.c_str();

            SQL_INSTANTIATE_PARAMETERS(func, ret, file);
        }

    SQL_PARAMETRIC_END;

  // and that is about all
  SQL_CLOSE_DATABASE;
}

void DataTypeManager::Initialize() {

     SQL_OPEN_DATABASE( GetMetadataDB() );

     // Create tables if non-existant.
     SQL_STATEMENTS_NOREZ(</"
        CREATE TABLE IF NOT EXISTS DataTypeManager_TypeInfo (
            type          TEXT,
            file          TEXT,
            convTo        TEXT,
            noExtract     INTEGER
        );
       "/>);

      SQL_STATEMENTS_NOREZ(</"
        CREATE TABLE IF NOT EXISTS DataTypeManager_FuncInfo (
            func          TEXT,
            type          TEXT,
            args          TEXT,
            ret           TEXT,
            priority      INTEGER,
            assoc         INTEGER,
            pure          INTEGER
        );
      "/>);

    SQL_STATEMENTS_NOREZ(</"
        CREATE TABLE IF NOT EXISTS DataTypeManager_SynonymInfo (
            type          TEXT,
            base          TEXT,
        );
      "/>);

    // Delete any data that may be in the current tables
     SQL_STATEMENTS_NOREZ(</"
        DELETE FROM DataTypeManager_TypeInfo;
     "/>);

     SQL_STATEMENTS_NOREZ(</"
        DELETE FROM DataTypeManager_FuncInfo;
     "/>);

     SQL_STATEMENTS_NOREZ(</"
        DELETE FROM DataTypeManager_SynonymInfo;
     "/>);

     // Transfer data from init tables to main tables

     SQL_STATEMENTS_NOREZ(</"
        INSERT INTO DataTypeManager_TypeInfo
               SELECT * FROM DataTypeManager_TypeInfo_Init;
    "/>);

    SQL_STATEMENTS_NOREZ(</"
        INSERT INTO DataTypeManager_FuncInfo
               SELECT * FROM DataTypeManager_FuncInfo_Init;
    "/>);

    SQL_STATEMENTS_NOREZ(</"
        INSERT INTO DataTypeManager_SynonymInfo
               SELECT * FROM DataTypeManager_SynonymInfo_Init;
    "/>);

    SQL_CLOSE_DATABASE;

     // Load data into data structures
     this->Load();
}
