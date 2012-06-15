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
#ifndef _AST_H
#define _AST_H

#include <string>
#include <vector>

#include "ASTVisitor.h"
#include<stdio.h>
#include<assert.h>

using namespace std;


/**
 ** This file defines a bunch of classes. Basically, every rule
 ** of the abstract query syntax tree has its corresponding class
 ** here. The idea is to implement a visitor pattern on top of them,
 ** i.e. a special abstract class that has a bunch of methods to
 ** easily traverse the tree. 
 **
 ** Look at ASTVisitor.h for more info on the visitor pattern. Please
 ** note that, when adding new class types here, those changes have to
 ** be reflected in the ASTVisitor class, too. I try to enforce them
 ** by having every class implement the AcceptVisitor() method, but
 ** one can never know.
 **/

// A table reference -- stores a table reference/alias
class TableReference {
public:
  string table;
  string alias;

  TableReference(string t, string a): table(t), alias(a) { }
  TableReference(string t): table(t), alias(t) { }
};

// ABSTRACT CLASS -- expression in the SELECT class
class Expression {
public:
  virtual void AcceptVisitor(ASTVisitor &) = 0;
};

// ABSTRACT CLASS -- predicate in the WHERE class
class BooleanPredicate {
public:
  virtual void AcceptVisitor(ASTVisitor &) = 0;
};

// ABSTRACT CLASS -- column definition in the SELECT class
class VColumn {
public:
  virtual void AcceptVisitor(ASTVisitor &) = 0;
};


// -----------------------------------------------
// -------------- EXPRESSION TYPES  --------------
class ColumnExpression : public Expression {
public:
  string table;
  string id;

  ColumnExpression(string tab, string i) : 
    table(tab), id(i) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitColumnExpression(this);
  }
};

class NumericExpression : public Expression {
public:
  double value;

  NumericExpression(double vl): value(vl) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitNumericExpression(this);
  }

};

class DateExpression : public Expression {
public:
  string value;

  void FromYMD(int _yyyy, int _mm, int _dd){
    char buf[128];
    sprintf(buf, "DATE(%d, %d, %d)", _yyyy, _mm, _dd);
    value = buf;
  }

  DateExpression(const char* _date){ 
    // Idealy, this would convert to DATE(yyyy,mm,dd)
    _date++;
    int yy = (_date[0]-'0')*1000+(_date[1]-'0')*100+(_date[2]-'0')*10+(_date[3]-'0');
    int mm = (_date[5]-'0')*10+(_date[6]-'0');
    int dd = (_date[8]-'0')*10+(_date[9]-'0');
    
    FromYMD(yy, mm, dd);
  }


  DateExpression(double _yyyy, double _mm, double _dd){
    FromYMD(_yyyy, _mm, _dd);
  }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitDateExpression(this);
  }

};

class StringExpression : public Expression {
public:
  string value;

  StringExpression(string vl): value(vl) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitStringExpression(this);
  }

};

class AggregateExpression : public Expression {
public:
  AggType aggType;
  SetQuantifier setQuantifier;
  Expression *expr;

  AggregateExpression(AggType agg, SetQuantifier sq, Expression *ex) :
    aggType(agg), setQuantifier(sq), expr(ex) { }

  // use this one for COUNT(*)
  AggregateExpression(AggType agg) :
    aggType(agg), setQuantifier(All), expr(NULL) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitAggregateExpression(this);
  }

};

// Alin replaced this by the simpler version

/* class CaseExpression : public Expression { */
/* public: */
/*   Expression *valExpr; */
/*   vector<Expression *> whenExpr; */
/*   Expression *elseExpr; */

/*   CaseExpression(Expression *ve, vector<Expression *> when, Expression *els) : */
/*     valExpr(ve), whenExpr(when), elseExpr(els) { } */

/*   CaseExpression(Expression *ve, vector<Expression *> when) : */
/*     valExpr(ve), whenExpr(when), elseExpr(NULL) { } */

/*   void AcceptVisitor(ASTVisitor &v) { */
/*     v.VisitCaseExpression(this); */
/*   } */

/* }; */


class CaseExpression : public Expression {
public:
  BooleanPredicate *boolExpr;
  Expression * trueExpr;
  Expression *falseExpr;
	
	CaseExpression(BooleanPredicate *_boolExpr, Expression* _trueExpr, Expression *_falseExpr):
		boolExpr(_boolExpr), trueExpr(_trueExpr), falseExpr(_falseExpr){ }
		
  void AcceptVisitor(ASTVisitor &v) {
    v.VisitCaseExpression(this);
  }

};


class SimpleWhenExpression : public Expression {
public:
  Expression *whenExpr;
  Expression *thenExpr;
  
  SimpleWhenExpression(Expression *when, Expression *then) : 
    whenExpr(when), thenExpr(then) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitSimpleWhenExpression(this);
  }
};

class SearchWhenExpression : public Expression {
public:
  BooleanPredicate *whenBool;
  Expression *thenExpr;

  SearchWhenExpression(BooleanPredicate *when, Expression *then) :
    whenBool(when), thenExpr(then) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitSearchWhenExpression(this);
  }
};

class ArithmeticExpression : public Expression {
public:
  Expression *left;
  Expression *right;
  AritOp op;

  ArithmeticExpression(Expression *l, Expression *r, AritOp o) :
    left(l), right(r), op(o) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitArithmeticExpression(this);
  }

};


// ----------------------------------------------
// -------------- PREDICATE TYPES  --------------

class ComparisonTest : public BooleanPredicate {
public:
  Expression *left;
  Expression *right;
  CompOp op;

  ComparisonTest(Expression *l, Expression *r, CompOp o) :
    left(l), right(r), op(o) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitComparisonTest(this);
  }

};

class BetweenTest : public BooleanPredicate {
public:
  Expression *expr;
  Expression *low;
  Expression *up;

  BetweenTest(Expression *ex, Expression *l, Expression *u) :
    expr(ex), low(l), up(u) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitBetweenTest(this);
  }


};

class LikeTest : public BooleanPredicate {
public:
  Expression *value;
  Expression *pattern;

  LikeTest(Expression *v, Expression *p) :
	value(v), pattern(p){}

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitLikeTest(this);
  }

};

class NotPredicate : public BooleanPredicate {
public:
  BooleanPredicate *pred;

  NotPredicate(BooleanPredicate *p) :
    pred(p) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitNotPredicate(this);
  }

};

class AndPredicate : public BooleanPredicate {
public:
  BooleanPredicate *left;
  BooleanPredicate *right;

  AndPredicate(BooleanPredicate *l, BooleanPredicate *r) :
    left(l), right(r) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitAndPredicate(this);
  }

};

class OrPredicate : public BooleanPredicate {
public:
  BooleanPredicate *left;
  BooleanPredicate *right;

  OrPredicate(BooleanPredicate *l, BooleanPredicate *r) :
    left(l), right(r) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitOrPredicate(this);
  }

};

// -------------------------------------------
// -------------- COLUMN TYPES  --------------
class AllFromTable : public VColumn {
public:
  string table;

  AllFromTable(string t) : table(t) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitAllFromTable(this);
  }

};

class DerivedColumn : public VColumn {
public:
  Expression *expr;
  string alias;

  DerivedColumn(Expression *e): expr(e), alias("") { }
  DerivedColumn(Expression *e, string al) : expr(e), alias(al) { }

  void AcceptVisitor(ASTVisitor &v) {
    v.VisitDerivedColumn(this);
  }

};


// This is the *main* query class -- contains everything up there.
// Note that the parser produces a vector of these, not only one.
class SFWQuery {
public:
  SetQuantifier setQuantifier;
  vector<VColumn *> selectList;
  vector<TableReference *> tableReferenceList;
  BooleanPredicate *whereClause;
  vector<ColumnExpression *> groupByClause;

  SFWQuery(SetQuantifier sq, vector<VColumn *> sl, vector<TableReference *> trl, BooleanPredicate *whr, vector<ColumnExpression *> gbc) :
    setQuantifier(sq), selectList(sl), tableReferenceList(trl), whereClause(whr), groupByClause(gbc) { }
  
};

#endif // _AST_H
