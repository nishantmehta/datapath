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
#include "DataTypeManager.h"
#include "DataTypeManagerInitializer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "Errors.h"

using namespace std;

inline
DataTypeManagerInitializer::DataTypeManagerInitializer()
{}

inline
DataTypeManagerInitializer::~DataTypeManagerInitializer()
{
  typeData.clear();
  synData.clear();
  funcData.clear();
}

void DataTypeManagerInitializer::Initialize()
{
  DataTypeManagerInitializer * dtmi = new DataTypeManagerInitializer();

  FATALIF( dtmi == NULL,
           "Unable to create data type manager initializer, new returned null pointer." );

  dtmi->initialize();

  delete dtmi;
}

inline
void DataTypeManagerInitializer::addType( string name, string include, vector<string> & conversions, bool noExtract )
{
  typeData.push_back( TypeRecord( name, include, conversions, noExtract ) );

  // Every base type is a synonym for itself.
  synData.push_back( SynRecord( name, name ) );
}

inline
void DataTypeManagerInitializer::addSyn( string base, string syn )
{
  synData.push_back( SynRecord( base, syn ) );
}

inline
void DataTypeManagerInitializer::addFunc( string name, string type, vector<string> & args,
                      string ret, int priority, int assoc, int pure )
{
  funcData.push_back( FuncRecord( name, type, args, ret, priority, assoc, pure ) );
}

// Add a set of functions to the file representing a binary operator with the arguments
// being all combinations of 2 types from the types vector, where the types are given
// in order from smallest to largest, and the rettypes vector contains the return type
// of the operator for each type in the types vector if that type is the largest.
//
// This attempts to follow C++'s guidelines for type promotion in operators as best
// as possible.
//
// Example:
// name = "operator+"
// types = {"INT", "BIGINT", "FLOAT", "DOUBLE"};
// rettypes = {"INT", "BIGINT", "FLOAT", "DOUBLE"};
//
// operator+("INT", "INT") returns "INT", while operator+("INT", "BIGINT") returns "BIGINT"
void DataTypeManagerInitializer::addPromotingOp( string name, vector<string> types, vector<string> rettypes )
{
    for( int first = types.size() - 1; first >= 0; --first )
    {
        // Add function with both types the same
        vector<string> args(2, types[first]);
        addFunc( name, "", args, rettypes[first], -1, NoAssoc, true );

        for( int second = first - 1; second >= 0; --second )
        {
            // Add functions for all second operands of types smaller than first.
            args.clear();
            args.push_back( types[first] );
            args.push_back( types[second] );
            addFunc( name, "", args, rettypes[first], -1, NoAssoc, true );

            args.clear();
            args.push_back( types[second] );
            args.push_back( types[first] );
            addFunc( name, "", args, rettypes[first], -1, NoAssoc, true );
        }
    }
}

void DataTypeManagerInitializer::initialize()
{
  const string OP_PREFIX = "";
  const string BOOL_TYPE = "bool";

  this->initRelations();

  vector<string> nonNumericTypes;
  vector<string> numericTypes;

  map<AttributeTypeEnum, string> baseTypeMap;
  map<AttributeTypeEnum, string>::iterator bIter;

  // Make sure our data vectors are clear before we begin.
  typeData.clear();
  synData.clear();
  funcData.clear();

  // Base types
  nonNumericTypes.push_back("DATE");
  nonNumericTypes.push_back("IPV4ADDR");
  nonNumericTypes.push_back("MACADDR");
  nonNumericTypes.push_back("HString");

  numericTypes.push_back("INT");
  numericTypes.push_back("BIGINT");
  numericTypes.push_back("FLOAT");
  numericTypes.push_back("DOUBLE");

  // Synonyms
  addSyn( "INT", "INTEGER" );
  addSyn( "INT", "SMALLINT" );
  addSyn( "BIGINT", "IDENTIFIER" );
  addSyn( "HString", "VARCHAR" );
  addSyn( "HString", "STRING" );

  // Header files
  map<string, string> typeToFile;
  typeToFile["DATE"] = string("DATE.h");
  typeToFile["IPV4ADDR"] = string("IPV4ADDR.h");
  typeToFile["MACADDR"] = string("MACADDR.h");
  typeToFile["HString"] = string("HString.h");

  typeToFile["INT"] = string("INT.h");
  typeToFile["BIGINT"] = string("BIGINT.h");
  typeToFile["FLOAT"] = string("FLOAT.h");
  typeToFile["DOUBLE"] = string("DOUBLE.h");

  // Add types
  vector<string> conversions;

  // Add string literal type
  addType( "STRING_LITERAL", "STRING_LITERAL.h", conversions, false );

  // Add non-numeric base types
  for( int i = 0; i < nonNumericTypes.size(); ++i )
    {
      addType( nonNumericTypes[i], typeToFile[nonNumericTypes[i]], conversions, false );
    }

  // Add numeric base types, taking into account conversions
  for( int i = 0; i < numericTypes.size(); ++i )
    {
      conversions.clear();

      for( int j = 0; j < numericTypes.size(); ++j )
    {
      if( j != i )
        {
          conversions.push_back( numericTypes[j] );
        }
    }

      addType( numericTypes[i], typeToFile[numericTypes[i]], conversions, true );
    }

  // Add bool type
  conversions.clear();
  addType( BOOL_TYPE, "", conversions, false );
  nonNumericTypes.push_back( BOOL_TYPE );

  // Add assignment operator for all types
  // For non-numeric types, assignment simply takes the same type on both
  // sides and returns that type as well
  for( int i = 0; i < nonNumericTypes.size(); ++i )
    {
      vector<string> args(2, nonNumericTypes[i]);
      addFunc( OP_PREFIX + "=", "", args, nonNumericTypes[i], -1, NoAssoc, true );
    }

  // For numeric types, the right side can be any other numeric type and it
  // returns the numeric type on the left.
  for( int i = 0; i < numericTypes.size(); ++i )
    {
      for( int j = 0; j < numericTypes.size(); ++j )
    {
      vector<string> args;
      args.push_back(numericTypes[i]);
      args.push_back(numericTypes[j]);

      addFunc( OP_PREFIX + "=", "", args, nonNumericTypes[i], -1, NoAssoc, true );
    }
    }

  // Add equality and inequality operators for all types.
  vector<string> ops;
  ops.push_back(OP_PREFIX + "==");
  ops.push_back(OP_PREFIX + "!=");
  for( int i = 0; i < nonNumericTypes.size(); ++i )
    {
      vector<string> args(2, nonNumericTypes[i]);
      for( int j = 0; j < 2; ++j )
    {
      addFunc( ops[j], "", args, BOOL_TYPE, -1, NoAssoc, true );
    }
    }

  for( int j = 0; j < 2; ++j )
    {
      vector<string> retBool(numericTypes.size(), BOOL_TYPE);

      addPromotingOp( ops[j], numericTypes, retBool );
    }

  // Add comparsion operators for numeric types
  ops.clear();
  ops.push_back(OP_PREFIX + ">");
  ops.push_back(OP_PREFIX + "<");
  ops.push_back(OP_PREFIX + ">=");
  ops.push_back(OP_PREFIX + "<=");
  for( int j = 0; j < 4; ++j )
    {
      vector<string> retBool(numericTypes.size(), BOOL_TYPE);

      addPromotingOp( ops[j], numericTypes, retBool );
    }

  // Add comparison operators for date
  for( int j = 0; j < 4; ++j ) {
    vector<string> args(2, "DATE");
    addFunc( ops[j], "", args, BOOL_TYPE, -1, NoAssoc, true );
  }

  // Add arithmetic operators for numeric types
  ops.clear();
  ops.push_back(OP_PREFIX + "+");
  ops.push_back(OP_PREFIX + "-");
  ops.push_back(OP_PREFIX + "/");
  ops.push_back(OP_PREFIX + "*");
  for( int j = 0; j < 4; ++j )
    {
      addPromotingOp( ops[j], numericTypes, numericTypes );
    }

  // Add and, or, and not operators for bool
  vector<string> args(2, BOOL_TYPE);
  addFunc( OP_PREFIX + "&&", "", args, BOOL_TYPE, -1, NoAssoc, true );
  addFunc( OP_PREFIX + "||", "", args, BOOL_TYPE, -1, NoAssoc, true );
  args.clear();
  args.push_back(BOOL_TYPE);
  addFunc(OP_PREFIX + "!", "", args, BOOL_TYPE, -1, NoAssoc, true );

  // Add unary plus and minus for numeric types
  for( int i = 0; i < numericTypes.size(); ++i )
    {
      args.clear();
      args.push_back( numericTypes[i]);

      addFunc( OP_PREFIX + "+", "", args, numericTypes[i], -1, NoAssoc, true );
      addFunc( OP_PREFIX + "-", "", args, numericTypes[i], -1, NoAssoc, true );
    }

  // Add conversion functions from string literal to HString and Date
  args.clear();
  args.push_back("STRING_LITERAL");
  addFunc( "HString", "", args, "HString", -1, NoAssoc, true );
  addFunc( "DATE", "", args, "DATE", -1, NoAssoc, true );

  // Add Date's + and - operators for ints.
  args.clear();
  args.push_back("DATE");
  args.push_back("INT");
  addFunc( OP_PREFIX + "-", "", args, "DATE", -1, NoAssoc, true );
  addFunc( OP_PREFIX + "+", "", args, "DATE", -1, NoAssoc, true );

  // Now that we've collated all of the data to put into the database, let's
  // run the SQL that inserts the data.
  addType( typeData );
  addSyn( synData );
  addFunc( funcData );

  // Tell the DataTypeManager to initialize its internal data structures
  // and the tables it uses.

  DataTypeManager & dtm = DataTypeManager::GetDataTypeManager();
  dtm.Initialize();

  // We're done with the initialiation data, so clear out the vectors holding
  // the data.
  typeData.clear();
  synData.clear();
  funcData.clear();
}

int main(void) {
  DataTypeManagerInitializer::Initialize();
}
