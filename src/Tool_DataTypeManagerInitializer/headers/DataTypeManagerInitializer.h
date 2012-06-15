//
//  Copyright 2012 Christopher Dudley
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
#ifndef _DATA_TYPE_MANAGER_INITIALIZER_H
#define _DATA_TYPE_MANAGER_INITIALIZER_H

#include <vector>
#include "DataTypes.h"
#include "DataTypeManager.h"

/**
 * Class to initialize the metadata database with information about base types
 * and operators.
 *
 * This class is not thread-safe, and thus only one component should use it
 * (likely the start-up script).
 *
 */

class DataTypeManagerInitializer
{
 private:

  // Private data types to allow for easy bulk inserts of data into the database.

  struct TypeRecord
  {
    std::string name;
    std::string include;
    std::vector<std::string> conversions;
    bool noExtract;

    TypeRecord( std::string n, std::string i, std::vector<std::string> & conv, bool _noExtract )
    {
      name = n;
      include = i;
      conversions = conv;
      noExtract = _noExtract;
    }
  };

  struct SynRecord
  {
	std::string base;
	std::string syn;

	SynRecord( std::string b, std::string s )
	{
	  base = b;
	  syn = s;
	}
  };

  struct FuncRecord
  {
	std::string name;
	std::string type;
	std::vector<std::string> args;
	std::string ret;
	int priority;
	int assoc;
	int pure;

	FuncRecord( std::string n, std::string t, std::vector<std::string> & ar,
				std::string r, int pr, int as, int pu )
	{
	  name = n;
	  type = t;
	  args = ar;
	  ret = r;
	  priority = pr;
	  assoc = as;
	  pure = pu;
	}
  };

  DataTypeManagerInitializer();
  ~DataTypeManagerInitializer();

  // These vectors keep track of the records that need to be added to each of the
  // init tables.

  vector<TypeRecord> typeData;
  vector<SynRecord> synData;
  vector<FuncRecord> funcData;

  // SQL functions

  /*
   * Creates the initializatioin tables and ensures that they are empty.
   */
  void initRelations();

  /*
   * Adds a set of types to the type initialization table.
   */
  void addType( std::vector<TypeRecord> & types  );

  /*
   * Adds a set of synonyms to the synonym initialization table.
   */
  void addSyn( std::vector<SynRecord> & syns );

  /*
   * Adds a set of functions to the function initialization table.
   */
  void addFunc( std::vector<FuncRecord> & funcs );

  // Helper functions

  /*
   * Adds a single type to the set of types to be added to the type initialization
   * table.
   */
  void addType( std::string name, std::string include, std::vector<std::string> & conversions, bool noExtract );

  /*
   * Adds a single synonym to the set of synonyms to be added to the synonym
   * initialization table.
   */
  void addSyn( std::string base, std::string syn );
  
  /*
   * Adds a signle function to the set of functions to be added to the function
   * initialization tables.
   */
  void addFunc( std::string name, std::string type, std::vector<std::string> & args, std::string ret,
				int priority, int assoc, int pure );

  /*
   * Adds a set of functions that model how type promotion works with binary
   * operators in C++.
   */
  void addPromotingOp( std::string name, std::vector<std::string> types,
					   std::vector<std::string> rettypes );
  
  // This function generates the base type and function information and loads
  // it into the database, and also tells the DataTypeManager to initialize itself
  // from this data.
  void initialize();

 public:

  // Creates and destroys the initializer object as necessary, and performs the initialization
  // sequence.
  static void Initialize();
};

#endif
