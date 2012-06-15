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
#include "AST.h"
#include "ASTVisitor.h"

#include <stdio.h>


TreePrinter::TreePrinter() {
}

void TreePrinter::PrintTabs() {
  for (int i=0;i<level;i++)
    printf("  ");
}

void TreePrinter::PrintQuery(SFWQuery *q) {

  // first, print the SELECT clause
  printf("::: SELECT CLAUSE\n");
  level = 1;

  for (int i=0;i<q->selectList.size();i++) {
    q->selectList[i]->AcceptVisitor(*this);
  }

  // now, print the FROM clause
  printf("\n::: FROM CLAUSE\n");
  level = 1;

  for (int i=0;i<q->tableReferenceList.size();i++) {

    // print each reference
    PrintTabs();
    printf("+ Table: %s / aliased as: %s\n", q->tableReferenceList[i]->table.c_str(), q->tableReferenceList[i]->alias.c_str());
  }

  // now, print the WHERE clause
  printf("\n::: WHERE CLAUSE\n");
  level = 1;
  q->whereClause->AcceptVisitor(*this);

  // finally, print the GroupBy clause
  printf("\n::: GROUP BY CLAUSE\n");
  level = 1;
  for (int i=0;i<q->groupByClause.size();i++) {

    // print each column
    q->groupByClause[i]->AcceptVisitor(*this);
  }
}

void TreePrinter::VisitColumnExpression(ColumnExpression *ce) {
  PrintTabs();
  printf("+ Table: %s / attribute: %s\n", ce->table.c_str(), ce->id.c_str());
}

void TreePrinter::VisitNumericExpression(NumericExpression *ne) {
  PrintTabs();
  printf("+ Numeric literal: %f\n", ne->value);
}


void TreePrinter::VisitDateExpression(DateExpression *ne) {
  PrintTabs();
  printf("+ Date literal: %s\n", ne->value.c_str());
}


void TreePrinter::VisitStringExpression(StringExpression *se) {
  PrintTabs();
  printf("+ String literal: %s\n", se->value.c_str());
}


void TreePrinter::VisitAggregateExpression(AggregateExpression *ae) {
  PrintTabs();

  printf("+ Aggregate expression: ");
  switch(ae->aggType) {
  case Avg:
    printf("Avg\n");
    break;
  case Sum:
    printf("Sum\n");
    break;
  case Count:
    printf("Count\n");
    break;
  case CountAll:
    printf("Count(*)\n");
    break;
  case Min:
    printf("Min\n");
    break;
  case Max:
    printf("Max\n");
    break;
  default:
    break;
  }

  // go to the kid
  if (ae->expr != NULL) {
    level++;
    ae->expr->AcceptVisitor(*this);
    level--;
  }
}


void TreePrinter::VisitCaseExpression(CaseExpression *ce) {
  PrintTabs();
  printf("+ Case expression\n");

  level++;
  ce->boolExpr->AcceptVisitor(*this);
  level--;

  level++;
  PrintTabs();
  printf("when: \n");
  ce->trueExpr->AcceptVisitor(*this);
  level--;

	printf("else: \n");
	ce->falseExpr->AcceptVisitor(*this);
	level--;
}


void TreePrinter::VisitSimpleWhenExpression(SimpleWhenExpression *swe) {
  PrintTabs();
  printf("+ Simple when expression\n");

  level++;
  PrintTabs();
  printf("WHEN: ");
  swe->whenExpr->AcceptVisitor(*this);
  level--;

  level++;
  PrintTabs();
  printf("THEN: ");
  swe->thenExpr->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitSearchWhenExpression(SearchWhenExpression *swe) {
  PrintTabs();
  printf("+ Search when expression\n");

  level++;
  PrintTabs();
  printf("WHEN: ");
  swe->whenBool->AcceptVisitor(*this);
  level--;

  level++;
  PrintTabs();
  printf("THEN: ");
  swe->thenExpr->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitArithmeticExpression(ArithmeticExpression *ae) {
  PrintTabs();
  printf("+ Arithmetic expression\n");

  level++;
  ae->left->AcceptVisitor(*this);
  level--;

  level++;
  ae->right->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitComparisonTest(ComparisonTest *ce) {

  PrintTabs();
  printf("+ Comparison test\n");

  level++;
  ce->left->AcceptVisitor(*this);
  level--;

  level++;
  ce->right->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitBetweenTest(BetweenTest *bt) {
  PrintTabs();
  printf("+ Between test\n");

  level++;
  bt->expr->AcceptVisitor(*this);
  level--;

  level++;
  bt->low->AcceptVisitor(*this);
  level--;

  level++;
  bt->up->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitLikeTest(LikeTest *ce) {
  PrintTabs();
  printf("+ Like test\n");

  level++;
  ce->value->AcceptVisitor(*this);
  level--;

  level++;
  ce->pattern->AcceptVisitor(*this);
  level--;

}


void TreePrinter::VisitNotPredicate(NotPredicate *np) {
  PrintTabs();
  printf("+ NOT predicate\n");

  level++;
  np->pred->AcceptVisitor(*this);
  level--;
}


void TreePrinter::VisitAndPredicate(AndPredicate *np) {
  PrintTabs();
  printf("+ AND predicate\n");

  level++;
  np->left->AcceptVisitor(*this);
  level--;

  if (np->right != NULL) {
    level++;
    np->right->AcceptVisitor(*this);
    level--;
  }
}


void TreePrinter::VisitOrPredicate(OrPredicate *np) {
  PrintTabs();
  printf("+ OR predicate\n");

  level++;
  np->left->AcceptVisitor(*this);
  level--;

  level++;
  np->right->AcceptVisitor(*this);
  level--;
}

void TreePrinter::VisitAllFromTable(AllFromTable *a) {
  PrintTabs();
  printf("+ ALL from table: %s\n", a->table.c_str());
}


void TreePrinter::VisitDerivedColumn(DerivedColumn *dc) {
  PrintTabs();
  printf("+ Derived column (alias: %s)\n", dc->alias.c_str());

  level++;
  dc->expr->AcceptVisitor(*this);
  level--;

}


