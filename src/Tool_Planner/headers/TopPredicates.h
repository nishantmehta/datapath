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
#ifndef _TOPPREDICATES_H
#define _TOPPREDICATES_H

#include <string>
#include <vector>

using namespace std;

/** These classes encapsulate basic operator information for the Top
 ** predicate. Observe that most of them contain an expression string,
 ** recreated from the parse tree -- this is necessary for the M4 part
 ** to correctly construct the code, yet it is supposed to be
 ** completely ignored by the code generator.
 **
 ** For simplicity, class attributes are left public.
 **/

// This is the aggregate -- note that this does not denote an output
// attribute, so if an aggregate does not have a TopOutput counterpart
// reflecting the expression using the attName, it will simply be
// projected out.
class TopAggregate {
public:

  // c-like expression string
  string expression;

  // output attribute name
  string attName;

  // aggregate type
  string attType;

  // list of base tables
  vector<string> baseTables;

  // basic constructor
  TopAggregate(string e, string an, string at, vector<string> bt): 
    expression(e), attName(an), attType(at), baseTables(bt) { }
};

// This is the output point. Note that derived columns and their
// respective expressions are kept here. Also, things like aggregates
// and the like have to be reflected in attNames
class TopOutput {
public:

  bool isPresent;

  // set of expressions that define the derived columns -- this is a
  // map between the attribute name and the expression that defines
  // such a column.
  map<string, string> derivedColumns;
  
  // output file name
  string outputFile;

  // basic constructor
  TopOutput(map<string, string> dc, string of):
    derivedColumns(dc), outputFile(of) { isPresent = true; }

  TopOutput() { isPresent = false; }
};

// This represents a disjunction. Since the current optimizer is purely
// based on simple table stats, it is unable to compute efficient
// statistics for disjunctions and more complicated predicates. This
// forces us to move them around as part of the Top predicate, so that
// they are not ignored by the optimizer but not treated the same way
// as other equality predicates
class TopDisjunction {
public:
  
  // the c-like expression for this boolean predicate
  string expression;

  // the set of attributes that participate in the expression
  vector<string> attNames;

  // the set of base tables that participate in the expression
  vector<string> baseTables;

  TopDisjunction(string e, vector<string> an, vector<string> bt):
    expression(e), attNames(an), baseTables(bt) { }
};

// This is basically experimental. I need to create this structures
// and pass them around so that I can wash my hands if someone accuses
// me of ignoring parts of the SQL. I don't.
class TopGroupBy {
public:

  bool isPresent;
  
  // the attributes that participate in the group by predicate
  vector<string> attNames;

  // the set of base tables that participate in the expression
  vector<string> baseTables;

  // basic constructor
  TopGroupBy(vector<string> bt, vector<string> an):
    baseTables(bt), attNames(an) { isPresent = true; }

  TopGroupBy() {isPresent = false; }

};

#endif // _TOPPREDICATES_H
