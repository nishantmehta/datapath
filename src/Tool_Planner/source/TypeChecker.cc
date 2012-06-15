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
#include "ASTVisitor.h"
#include "AST.h"
#include "Catalog.h"
#include "Errors.h"

TypeChecker::TypeChecker() {
  ok = true;
}

TypeChecker::~TypeChecker() {
}

TypeChecker::TType TypeChecker::TypeToClass(string type){
	if (type == "INT" || type == "IDENTIFIER" ||
			type == "DOUBLE" || type == "FLOAT" || type=="BIGINT")
	  return Numeric;

	if (type == "VARCHAR" || type == "STRING") 
	  return String;

	if (type == "DATE")
	  return Date;
    
	FATAL("We do not know how to deal with type %s", type.c_str());
}

bool TypeChecker::CheckTypes(SFWQuery *q) {

  // get a reference to the catalog
  Catalog &cat = Catalog::GetCatalog();

  // go through the FROM clause first
  for (int i=0;i<q->tableReferenceList.size();i++) {
    
    // check table existence
    Schema sch(q->tableReferenceList[i]->table);
    if (!cat.GetSchema(sch)) {
      printf("Relation [%s] not found in catalog!\n", q->tableReferenceList[i]->table.c_str());
      return(false);
    }

    // check if it's already aliased
    map<string, string>::iterator it = tables.find(q->tableReferenceList[i]->alias);
    if (it != tables.end()) {
      printf("Relation [%s] referred at least twice in the FROM clause. This is ambiguous\n", q->tableReferenceList[i]->alias.c_str());
      return(false);
    }
    
    // add it
    tables[q->tableReferenceList[i]->alias] = q->tableReferenceList[i]->table;    
  }

  // now, go through the GROUP BY clause
  for (int i=0;i<q->groupByClause.size();i++) {

    // check each of them separately
    q->groupByClause[i]->AcceptVisitor(*this);

    // did we fail?, then let's quit
    if (!ok) {
      return(false);
    }
  }

  // then, go through the SELECT list
  for (int i=0;i<q->selectList.size();i++) {

    // check each of them separately
    q->selectList[i]->AcceptVisitor(*this);

    // did we fail? then let's quit
    if (!ok) {
      return(false);
    }
  }

  // finally, check the WHERE predicates
  if (q->whereClause != NULL) {
    q->whereClause->AcceptVisitor(*this);
  }

  return(ok);
}


bool TypeChecker::AreCompatible(TType t1, TType t2) {

  bool correct = ((t1 == Numeric && t2 == Numeric) || 
		  (t1 == Date && t2 == Date) ||
		  (t1 == String && t2 == String) ||
		  (t1 == Numeric && t2 == Date) ||
		  (t1 == Date && t2 == Numeric) // ||
		  //		  (t1 == String && t2 == Date) ||
		  //(t1 == Date && t2 == String)
		  );

  return(correct);
}

void TypeChecker::VisitColumnExpression(ColumnExpression *ce) {

  // get catalog instance
  Catalog &cat = Catalog::GetCatalog();

  // do we have a table ID? if not, we'll have to look for one that
  // matches it correctly
  if (ce->table.compare("") == 0) {
		
    // let's try to find it in the catalog, through our aliased tables
    bool foundIt = false;
    map<string, string>::iterator it;
    for (it = tables.begin(); it != tables.end() && !foundIt; it++) {
			
      // get the schema
      Schema sch(it->second);
      cat.GetSchema(sch);   
			
      // find the attribute
      Attribute att(ce->id);
      foundIt = sch.GetAttribute(att);
			
      // also annotate the tree and push type into the stack.
      if (foundIt) {
	ce->table = it->first;
	
	// get the type
	typeStack.push( TypeToClass(att.GetType()) );
      }
    }
			
    // didn't we find it? throw error
    if (!foundIt) {
      printf("Attribute [%s] not found in the catalog\n", ce->id.c_str());
      ok = false;
      return;
    }
  }
  // otherwise, just find it directly
  else {

    // get the schema
    Schema sch(tables[ce->table]);
    cat.GetSchema(sch);

    // find the attribute
    Attribute att(ce->id);

    // didn't we find it? throw error    
    if (!sch.GetAttribute(att)) {
      printf("Attribute [%s.%s] not found in the catalog.\n", ce->table.c_str(), ce->id.c_str());
      ok = false;
      return;
    }

		// get the type
		typeStack.push( TypeToClass(att.GetType()) );
				
  }
}

void TypeChecker::VisitNumericExpression(NumericExpression *) {

  // do nothing, just stack up the type
  typeStack.push(Numeric);
}

void TypeChecker::VisitDateExpression(DateExpression *) {

  // do nothing, just stack up the type
  typeStack.push(Date);
}


void TypeChecker::VisitStringExpression(StringExpression *) {

  // do nothing, just stack up the type
  typeStack.push(String);
}

void TypeChecker::VisitAggregateExpression(AggregateExpression *ae) {

  // just check the subexpression
  if (ae->expr != NULL)
    ae->expr->AcceptVisitor(*this);
}

void TypeChecker::VisitCaseExpression(CaseExpression *ce) {

	// check the condition
  ce->boolExpr->AcceptVisitor(*this);
	TType boolType = typeStack.top();
	
	// check the true part
	ce->trueExpr->AcceptVisitor(*this);
	TType trueType = typeStack.top();

	// check the false part
 	ce->falseExpr->AcceptVisitor(*this);
	TType falseType = typeStack.top();	

	if (trueType != falseType){
    printf("Types in CASE/WHEN expression are not consistent.\n");
    ok = false;
    return;
  }

  // push the good type
	typeStack.push(trueType);
}

void TypeChecker::VisitSimpleWhenExpression(SimpleWhenExpression *se) {

  // check the WHEN side of the expression
  se->whenExpr->AcceptVisitor(*this);

  // pop up the type and check it
  TType mt = typeStack.top();
  typeStack.pop();

  if (mt != tempType) {
    printf("Types in CASE/WHEN expression are not consistent.\n");
    ok = false;
    return;
  }

  // now check the THEN side and stack the type
  se->thenExpr->AcceptVisitor(*this);  
}

void TypeChecker::VisitSearchWhenExpression(SearchWhenExpression *se) {

  // check the WHEN predicate
  se->whenBool->AcceptVisitor(*this);

  // check the THEN side and stack the type
  se->thenExpr->AcceptVisitor(*this);
}

void TypeChecker::VisitArithmeticExpression(ArithmeticExpression *ae) {

  // check the lhs
  ae->left->AcceptVisitor(*this);
  TType t1 = typeStack.top();

  // check the rhs
  ae->right->AcceptVisitor(*this);
  TType t2 = typeStack.top();

  if (AreCompatible(t1,t2)) {
    typeStack.push(t1);
  }
  else {
    printf("Incompatible types in arithmetic expression.\n");
    ok = false;
  }
}

void TypeChecker::VisitComparisonTest(ComparisonTest *ct) {

  // check the right side, pop type
  ct->left->AcceptVisitor(*this);
  TType t1 = typeStack.top();

  // check the left side, pop type
  ct->right->AcceptVisitor(*this);
  TType t2 = typeStack.top();

  if (AreCompatible(t1,t2)) {
    typeStack.push(t1);
  }
  else {
    printf("Incompatible types in comparison expression.\n");
    ok = false;
  }
}

void TypeChecker::VisitBetweenTest(BetweenTest *bt) {

  // check the expression, pop type
  bt->expr->AcceptVisitor(*this);
  TType t1 = typeStack.top();

  // now, check the lower bound, and pop type
  bt->low->AcceptVisitor(*this);
  TType t2 = typeStack.top();

  // then, check the upper bound, and pop type
  bt->up->AcceptVisitor(*this);
  TType t3 = typeStack.top();

  // check type comparisons
  if ((t1 == Numeric && t2 == Numeric && t3 == Numeric) ||
      (t1 == Date && t2 == String && t3 == String) ||
      (t1 == String && t2 == String && t3 == String)) {
    typeStack.push(Numeric);
  } else {
    printf("Incompatible types in BETWEEN expression.\n");
    ok = false;
  }    

}

void TypeChecker::VisitLikeTest(LikeTest *lt) {

  // check the right side, pop type
  lt->value->AcceptVisitor(*this);
  TType t1 = typeStack.top();

  // check the left side, pop type
  lt->pattern->AcceptVisitor(*this);
  TType t2 = typeStack.top();

  if (AreCompatible(t1,t2)) {
    typeStack.push(t1);
  }
  else {
    printf("Incompatible types in LIKE expression.\n");
    ok = false;
  }
}

void TypeChecker::VisitNotPredicate(NotPredicate *np) {

  // just check the kid
  np->pred->AcceptVisitor(*this);
}

void TypeChecker::VisitAndPredicate(AndPredicate *ap) {

  // just check the kids
  ap->left->AcceptVisitor(*this);

  if (ap->right != NULL) 
    ap->right->AcceptVisitor(*this);
}

void TypeChecker::VisitOrPredicate(OrPredicate *op) {

  // just check the kids
  op->left->AcceptVisitor(*this);
  op->right->AcceptVisitor(*this);
}


void TypeChecker::VisitAllFromTable(AllFromTable *aft) {

  // just check if this table is referenced in the FROM clause
  map<string, string>::iterator it = tables.find(aft->table);

  // not found? throw error
  if (it == tables.end()) {
    printf("Relation [%s] not found in FROM clause.\n", aft->table.c_str());
    ok = false;
  }
}

void TypeChecker::VisitDerivedColumn(DerivedColumn *dc) {

  // first, check if the alias is not used before
  if (dc->alias.compare("") != 0) {
    set<string>::iterator it = cols.find(dc->alias);

    // already there? throw error
    if (it != cols.end()) {
      printf("Derived column [%s] is aliased at least twice in SELECT clause.\n", dc->alias.c_str());
      ok = false;
      return;
    }

    // put in
    cols.insert(dc->alias);
  }

  // now, check the expression that defines this column
  dc->expr->AcceptVisitor(*this);
}

