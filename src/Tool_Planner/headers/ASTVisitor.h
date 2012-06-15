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
#ifndef _ASTVISITOR_H
#define _ASTVISITOR_H

#include <map>
#include <set>
#include <string>
#include <stack>
#include <vector>

using namespace std;

// Enumerations -- used to compose more complex types
enum SetQuantifier {All, Distinct};
enum AggType {Avg, Sum, Count, CountAll, Min, Max};
enum AritOp {Plus, Minus, Times, Divide};
enum CompOp {Equals, NotEquals, LessThan, GreaterThan, LessEqual, GreaterEqual, Like, NotLike};

// class pre-definitions
class ColumnExpression;
class NumericExpression;
class DateExpression;
class StringExpression;
class AggregateExpression;
class CaseExpression;
class SimpleWhenExpression;
class SearchWhenExpression;
class ArithmeticExpression;
class ComparisonTest;
class BetweenTest;
class LikeTest;
class NotPredicate;
class AndPredicate;
class OrPredicate;
class AllFromTable;
class DerivedColumn;
class SFWQuery;
class Optimizer;
class TopDisjunction;
class TopAggregate;

// Implements a visitor pattern -- note that each of the classes
// mentioned have an AcceptVisitor() method, which is responsible for
// doing all the necessary casts internally and going to the precise
// method.
class ASTVisitor {
public:

  // visiting expressions
  virtual void VisitColumnExpression(ColumnExpression *) = 0;
  virtual void VisitNumericExpression(NumericExpression *) = 0;
  virtual void VisitDateExpression(DateExpression *) = 0;
  virtual void VisitStringExpression(StringExpression *) = 0;
  virtual void VisitAggregateExpression(AggregateExpression *) = 0;
  virtual void VisitCaseExpression(CaseExpression *) = 0;
  virtual void VisitSimpleWhenExpression(SimpleWhenExpression *) = 0;
  virtual void VisitSearchWhenExpression(SearchWhenExpression *) = 0;
  virtual void VisitArithmeticExpression(ArithmeticExpression *) = 0;

  // visiting predicates
  virtual void VisitComparisonTest(ComparisonTest *) = 0;
  virtual void VisitBetweenTest(BetweenTest *) = 0;
  virtual void VisitLikeTest(LikeTest *) = 0;
  virtual void VisitNotPredicate(NotPredicate *) = 0;
  virtual void VisitAndPredicate(AndPredicate *) = 0;
  virtual void VisitOrPredicate(OrPredicate *) = 0;

  // visiting columns
  virtual void VisitAllFromTable(AllFromTable *) = 0;
  virtual void VisitDerivedColumn(DerivedColumn *) = 0;
};


// the type checker is a visitor, i.e. it implements all the stuff
// defined above
class TypeChecker : public ASTVisitor {
private:

  // aliased tables go here
  map<string, string> tables;

  // aliased derived columns go here
  set<string> cols;

  // status
  bool ok;

  // stack/temps s of types
  enum TType {Numeric, String, Date};
  stack<TType> typeStack;
  TType tempType;
	// function to go from string type to TType
	TType TypeToClass(string type);

  // returns true if types are compatible
  bool AreCompatible(TType t1, TType t2);

public:
  TypeChecker();
  ~TypeChecker();

  // this is the type checker's entrypoint
  bool CheckTypes(SFWQuery *query);

  // visiting expressions
  void VisitColumnExpression(ColumnExpression *);
  void VisitNumericExpression(NumericExpression *);
  void VisitDateExpression(DateExpression *);
  void VisitStringExpression(StringExpression *);
  void VisitAggregateExpression(AggregateExpression *);
  void VisitCaseExpression(CaseExpression *);
  void VisitSimpleWhenExpression(SimpleWhenExpression *);
  void VisitSearchWhenExpression(SearchWhenExpression *);
  void VisitArithmeticExpression(ArithmeticExpression *);

  // visiting predicates
  void VisitComparisonTest(ComparisonTest *);
  void VisitBetweenTest(BetweenTest *);
  void VisitLikeTest(LikeTest *);
  void VisitNotPredicate(NotPredicate *);
  void VisitAndPredicate(AndPredicate *);
  void VisitOrPredicate(OrPredicate *);

  // visiting columns
  void VisitAllFromTable(AllFromTable *);
  void VisitDerivedColumn(DerivedColumn *);

};


// the converter is also a visitor. it is the coolest visitor.
class Converter : ASTVisitor {
private:
  bool inAggregate;

  // pointer to the optimizer
  Optimizer *opt;

  // query ID #
  __int64_t queryID;

  // stage -- are we on the SELECT or WHERE part?
  bool whereStage;
  
  // true if only a derived attribute (no complex expression)
  bool derivedOnlyAtt;

  // number for anonymous attributes
  __int64_t anonAtts;

  // number of aggregate attributes
  __int64_t anonAggs;

  // this accumulates an expression
  string expr;

  // set of tables for which to add a passthrough predicate
  set<string> passThrough;

  // set of output attributes
  set< pair<string, string> > outputAtts;

  // this signals if the expression should be NOTed
  bool boolNot;

  // this keeps track of the latest expression types
	// Alin: I introduced the type EFunction to allow Like(..)
  enum ExpType {ELitFilter, EColFilter, EJoin, EOrFilter, ELiteral, EColumn, EMarker, EFunction};
  stack<ExpType> expTypes;

  // this keeps track of the latest literals used
  stack<string> lits;

  // this keeps track of the latest columns referenced to
  stack<pair<string, string> > cols;

  // this keeps track of the latest comparison operators used
  stack<__int64_t> ops;

  // pops predicates and inserts them into the optimizer -- important!
  void PopPredicate();

  // collection of disjunctions (OR'ed expressions)
  vector<TopDisjunction> disjunctions;

  // collection of aggregates
  vector<TopAggregate> aggs;

public:

  Converter(Optimizer *o, __int64_t q);
  ~Converter();

  // the converter's entrypoints
  void FillOptimizer(SFWQuery *q);

  // visiting expressions
  void VisitColumnExpression(ColumnExpression *);
  void VisitNumericExpression(NumericExpression *);
  void VisitDateExpression(DateExpression *);
  void VisitStringExpression(StringExpression *);
  void VisitAggregateExpression(AggregateExpression *);
  void VisitCaseExpression(CaseExpression *);
  void VisitSimpleWhenExpression(SimpleWhenExpression *);
  void VisitSearchWhenExpression(SearchWhenExpression *);
  void VisitArithmeticExpression(ArithmeticExpression *);

  // visiting predicates
  void VisitComparisonTest(ComparisonTest *);
  void VisitBetweenTest(BetweenTest *);
  void VisitLikeTest(LikeTest *);
  void VisitNotPredicate(NotPredicate *);
  void VisitAndPredicate(AndPredicate *);
  void VisitOrPredicate(OrPredicate *);

  // visiting columns
  void VisitAllFromTable(AllFromTable *);
  void VisitDerivedColumn(DerivedColumn *);
};

// this is a class I used to debug the compiler -- it prints the parse
// tree in an orderly fashion
class TreePrinter : ASTVisitor {
private:
  // number of tabs to print
  __int64_t level;

  // print a bunch of tabs
  void PrintTabs();
public:

  TreePrinter();

  // entrypoint
  void PrintQuery(SFWQuery *);

  // visiting expressions
  void VisitColumnExpression(ColumnExpression *);
  void VisitNumericExpression(NumericExpression *);
  void VisitDateExpression(DateExpression *);
  void VisitStringExpression(StringExpression *);
  void VisitAggregateExpression(AggregateExpression *);
  void VisitCaseExpression(CaseExpression *);
  void VisitSimpleWhenExpression(SimpleWhenExpression *);
  void VisitSearchWhenExpression(SearchWhenExpression *);
  void VisitArithmeticExpression(ArithmeticExpression *);

  // visiting predicates
  void VisitComparisonTest(ComparisonTest *);
  void VisitBetweenTest(BetweenTest *);
  void VisitLikeTest(LikeTest *);
  void VisitNotPredicate(NotPredicate *);
  void VisitAndPredicate(AndPredicate *);
  void VisitOrPredicate(OrPredicate *);

  // visiting columns
  void VisitAllFromTable(AllFromTable *);
  void VisitDerivedColumn(DerivedColumn *);
};
#endif // _ASTVISITOR_H
