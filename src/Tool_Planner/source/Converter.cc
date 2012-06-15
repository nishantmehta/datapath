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
#include "TopPredicates.h"
#include "Optimizer1.h"

#define CONVERTER_DEBUG true

Converter::Converter(Optimizer *o, __int64_t q) {

  // assign
  opt = o;
  queryID = q;
  inAggregate = false; // by default out of aggregate
}

Converter::~Converter() {

  opt = NULL;
}

void Converter::FillOptimizer(SFWQuery *q) {

  // sets this name, first!
  anonAtts = 1;
  anonAggs = 1;

  // get a reference to the catalog
  Catalog &cat = Catalog::GetCatalog();

  // first, we'll go through the FROM clause, inserting table scans
  for (__int64_t i=0;i<q->tableReferenceList.size();i++) {

    // does this table have an alias? 
    if (q->tableReferenceList[i]->alias.compare(q->tableReferenceList[i]->table) != 0) {

      // if so, put in catalog instance
      cat.AddSchemaAlias(q->tableReferenceList[i]->alias, q->tableReferenceList[i]->table);
    }

    // now, add the passthrough
    passThrough.insert(q->tableReferenceList[i]->alias);

    // and the table scan

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER: Adding tablescan: %s (%s)\n", q->tableReferenceList[i]->alias.c_str(), q->tableReferenceList[i]->table.c_str());
#endif

    opt->InsertPredicate(TableScan, NotApplicable, string(q->tableReferenceList[i]->table), string(q->tableReferenceList[i]->alias), 
			 IGNORE_NAME, IGNORE_NAME, IGNORE_NAME,
			 queryID);

  }

  // now, we will look at the SELECT list and get stuff for the top
  // predicate
  whereStage = false;
  for (__int64_t i=0;i<q->selectList.size();i++) {

    // visit every column
    q->selectList[i]->AcceptVisitor(*this);
  }

  // now, go through the WHERE clause and look for joins and filters.
  whereStage = true;
  if (q->whereClause != NULL) {
    q->whereClause->AcceptVisitor(*this);
  }

  // now, we will create passthrough predicates for all relations that
  // do not have any filters
  for (set<string>::iterator it = passThrough.begin(); it != passThrough.end(); it++) {
    
    // put in

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER: adding passthrough table: %s\n", (*it).c_str());
#endif

    opt->InsertPredicate(Selection, LT, "true", *it, IGNORE_NAME,
    IGNORE_NAME, IGNORE_NAME, queryID);
  }


  // finally, look at the GROUP BY clause and fill the top predicate
  vector<string> groupByAtts;
  vector<string> groupByTables;
			    
  for (__int64_t i=0;i<q->groupByClause.size();i++) {
    
    groupByAtts.push_back(q->groupByClause[i]->table + "." + q->groupByClause[i]->id);
    groupByTables.push_back(q->groupByClause[i]->table);
  }

  // create the group by top predicate
  TopGroupBy tb(groupByTables, groupByAtts);
  if (groupByAtts.size() == 0)
    tb.isPresent = false;

  // transform the output attributes into the deried column maps
  map<string, string> derivedColumns;
  set< pair<string, string> >::iterator it;
  for (it = outputAtts.begin(); it != outputAtts.end(); it++) {
    derivedColumns[(*it).second] = (*it).first;
  }

  // get an output file
  char outf[64];
  sprintf(outf, "output_q%ld.txt", queryID);

  TopOutput tp(derivedColumns, outf);
  if (derivedColumns.size() == 0)
    tp.isPresent = false;

  // finally, insert everything that goes on top
  opt->InsertTopPredicate(aggs, disjunctions, tp, tb, queryID);

#ifdef CONVERTER_DEBUG    
  printf("\n\n");
#endif

}

void Converter::VisitComparisonTest(ComparisonTest *ct) {

  // this is the most important part of the WHERE clause. what we'll
  // do here is identify the type of comparison we have and push it in
  // the stack... essentially, comparisons can have the following
  // forms (all of them commute the operation):
  //
  // <X.column> OP <literal>  : this is a filter
  // <X.column> OP <X.column> : this, also, is a filter
  // <X.column> OP <Y.column> : we have a join!

  // first, we will consider the operator and produce the
  // corresponding string, but carefully -- a boolean NOT might be in
  // place so we might have to invert it.

  string oper;
  Operator nikOp;
  switch(ct->op) {
  case Equals:
    oper = (boolNot ? "!=" : "==");
    nikOp = (boolNot ? NEQ : EQ);
    break;

  case NotEquals:
    oper = (boolNot ? "==" : "!=");
    nikOp = (boolNot ? EQ : NEQ );
    break;

  case LessThan:
    oper = (boolNot ? ">=" : "<");
    nikOp = (boolNot ? GT : LT );
    break;

  case GreaterThan:
    oper = (boolNot ? "<=" : ">");
    nikOp = (boolNot ? LT : GT );
    break;

  case LessEqual:
    oper = (boolNot ? ">" : "<=");
    nikOp = (boolNot ? GEQ : LEQ );
    break;

  case GreaterEqual:
    oper = (boolNot ? "<" : ">=");
    nikOp = (boolNot ? LEQ : GEQ);
    break;

  default:
    break;
  }

  // push the op
  ops.push(nikOp);

  // fine, now we will proceed with the expression on the left side...
  ct->left->AcceptVisitor(*this);

	printf("AFTER LEFT SIDE: %ld %s\n", expTypes.size(), expr.c_str());

  // then we append the operator to the expression...
  expr += " " + oper + " ";

  // and we go to the expression on the right side...
  ct->right->AcceptVisitor(*this);

  // now, what we have to do is determine the kind of comparison we
  // are dealing with, so we pop the two expression types from the
  // stack
	printf("HEY LOOK AT THIS: %ld %s\n", expTypes.size(), expr.c_str());
  ExpType t1 = expTypes.top();
  expTypes.pop();
  
  ExpType t2 = expTypes.top();

  // push t1 again, to conserve
  expTypes.push(t1);

  // first case is the easiest -- column vs. literal
  if ((t1 == EColumn && t2 == ELiteral) || (t1 == ELiteral && t2 == EColumn)) {

    // push the expresion type
    expTypes.push(ELitFilter);
  }

  // now the second case -- column vs. column
  else if (t1 == EColumn && t2 == EColumn) {

    // get the table names of these guys
    pair<string,string> p1 = cols.top();
    cols.pop();

    pair<string,string> p2 = cols.top();

    // push p1 again, to conserve
    cols.push(p1);

    // are they equal?
    if (p1.first.compare(p2.first) == 0) {

      // then, this is also a filter, so push up
      expTypes.push(ELitFilter);
    }
    else {

      // otherwise, we have a join
      expTypes.push(EJoin);
    }
  }

  // whatever else we don't support... yet.
  else {
    printf("Comparison expression too complicated for parser to handle.\n");
  }
}

void Converter::VisitBetweenTest(BetweenTest *bt) {

  // temporarily ignored
}

void Converter::VisitLikeTest(LikeTest *lt)  {
	// We just need to form the construct
	// Like( value, pattern)
	
  Operator nikOp;
	derivedOnlyAtt = false;

	ops.push(LIKE);

	expr += "MATCH(";

	// do the value
	lt->value->AcceptVisitor(*this);
	expr += ",";

	// do the pattern 
	lt->pattern->AcceptVisitor(*this);
	expr += ")";

	expTypes.push(ELitFilter);

}

void Converter::VisitNotPredicate(NotPredicate *np) {

  // set the NOT flag
  boolNot = !boolNot;
  np->pred->AcceptVisitor(*this);
}

void Converter::PopPredicate() {

  // let's decide what to insert!
  ExpType et = expTypes.top();
  expTypes.pop();

// 	// function 
 	if (et == EFunction) {

// 		// get the table and column
     pair<string, string> col = cols.top();
     cols.pop();
    
//     // get the literal
     string lit = lits.top();
     lits.pop();

// #ifdef CONVERTER_DEBUG    
     printf("CONVERTER adding function: %s\n", expr.c_str());
// #endif
     if (!inAggregate)
       opt->InsertPredicate(Selection, FUNCTION, expr, col.first, IGNORE_NAME, col.second, IGNORE_NAME, queryID);

// 		// remove table from passthroughs
     passThrough.erase(col.first);

		
 	} else
  // literal filter
  if (et == ELitFilter) {
    
    // get the table and column
    pair<string, string> col = cols.top();
    cols.pop();
    
    // get the literal
    string lit = lits.top();
    lits.pop();

    // get the operator
    Operator dop = (Operator)ops.top();
    ops.pop();

    // insert

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER adding literal filter: %s\n", expr.c_str());
#endif
    if (!inAggregate)
      opt->InsertPredicate(Selection, dop, expr, col.first, IGNORE_NAME, col.second, IGNORE_NAME, queryID);

    // remove table from passthroughs
    passThrough.erase(col.first);
  }

  // column filter
  else if (et == EColFilter) {

    // get both columns
    pair<string, string> col1 = cols.top();
    cols.pop();

    pair<string, string> col2 = cols.top();
    cols.pop();

    // get the operator
    Operator dop = (Operator)ops.top();
    ops.pop();

    // insert in

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER adding column filter: %s\n", expr.c_str());
#endif

    if (!inAggregate)
      opt->InsertPredicate(Selection, dop, expr, col1.first, col2.first, 
			   col1.second, col2.second, queryID);

    // remove table from passthroughs
    passThrough.erase(col1.first);
  }

  // join
  else if (et == EJoin) {

    // get both columns
    pair<string, string> col1 = cols.top();
    cols.pop();

    pair<string, string> col2 = cols.top();
    cols.pop();

    // get the operator
    Operator dop = (Operator)ops.top();
    ops.pop();

    // insert in

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER adding join: %s (%s vs. %s)\n", expr.c_str(), col1.first.c_str(), col2.first.c_str());
#endif
    if (!inAggregate)
      opt->InsertPredicate(Join, dop, expr, col1.first, col2.first, 
			   col1.second, col2.second, queryID);

    // we don't remove passthroughs for joins, sorry bro.
  }

  // OR'd expression
  else if (et == EOrFilter) {

    // temporary stuff
    set<string> attNames;
    set<string> tableNames;

    // pop top
    ExpType tx = expTypes.top();

    // add stuff until the stack is empty
    while (tx != EMarker) {
      
      // add stuff only if we stumble upon a column
      if (tx == EColumn) {
	
	pair<string, string> p = cols.top();
	cols.pop();
	tableNames.insert(p.first);
	attNames.insert(p.first + "." + p.second);
      }

      tx = expTypes.top();
      expTypes.pop();
    }

#ifdef CONVERTER_DEBUG        
    printf("CONVERTER adding disjunction: %s [%ld] [%ld]", expr.c_str(), (__int64_t)attNames.size(), (__int64_t)tableNames.size());
#endif

    // fill in vectors with sets
    vector<string> atts;
    atts.resize(attNames.size());
    vector<string> tabs;
    tabs.resize(tableNames.size());
    std::copy(attNames.begin(), attNames.end(), atts.begin());
    std::copy(tableNames.begin(), tableNames.end(), tabs.begin());

#ifdef CONVERTER_DEBUG
    printf(" (atts: ");
    for (__int64_t i=0;i<atts.size();i++) {
      printf("%s ", atts[i].c_str());
    }
    printf(") (tables: ");
    for (__int64_t i=0;i<tabs.size();i++) {
      printf("%s ", tabs[i].c_str());
    }
    printf(")\n");
#endif

    // create type
    TopDisjunction dj(expr, atts, tabs);

    // and save it
    disjunctions.push_back(dj);
  }

}

void Converter::VisitAndPredicate(AndPredicate *ap) {

  // we will go on both sides of the tree, restarting our structures
  // at once

  // first, on the left
  boolNot = false;
  expr = "";
  expTypes.push(EMarker);
  ap->left->AcceptVisitor(*this);
  PopPredicate();


  // then, right
  if (ap->right != NULL) {
    boolNot = false;
    expr = "";
    expTypes.push(EMarker);
    ap->right->AcceptVisitor(*this);
    PopPredicate();
  }
}

void Converter::VisitOrPredicate(OrPredicate *oop) {

  // so, we'll visit both sides of the expression and accumulate them
  // using an || operator
    
  // go to the left side
  oop->left->AcceptVisitor(*this);

  // add OR
  expr += " || ";

  // go to the right side
  oop->right->AcceptVisitor(*this);

  // push type
  expTypes.push(EOrFilter);
}


void Converter::VisitColumnExpression(ColumnExpression *ce) {

  // get the expression
  string cexp;
  cexp = ce->table + "." + ce->id;

  // accumulate the expression
  expr += cexp;

  // push the expression type and column
  if (whereStage) {
    expTypes.push(EColumn);
  }
 
  pair<string,string> p;
  p.first = ce->table;
  p.second = ce->id;
  cols.push(p);
}

void Converter::VisitNumericExpression(NumericExpression *ne) {

  // just append the value to the expression
  char no[32];
  sprintf(no, "%f", ne->value);
  expr += no;

  // push the expression type and value
	expTypes.push(ELiteral);
	lits.push(no);
}


void Converter::VisitDateExpression(DateExpression *ne) {

  // just append the value to the expression
  expr += ne->value.c_str();

  // push the expression type and value
  expTypes.push(ELiteral);
  lits.push(ne->value.c_str());
}

void Converter::VisitStringExpression(StringExpression *se) {

  // just append the value of the expression
  expr += se->value;

  // push the expression type
	expTypes.push(ELiteral);
	lits.push(se->value);
}

void Converter::VisitAggregateExpression(AggregateExpression *ae) {

  if (!whereStage) {

    // set for derived column
    derivedOnlyAtt = false;

    // first, we'll save the expression up to this point because we're
    // going to do a replacement
    string texp = expr;
    expr = "";

    // get the expression for that aggregate -- but before, kill the
    // cols stack
    
    inAggregate = true; // shut up the predicate stuff
    cols = stack<pair<string, string> >();
    if (ae->expr != NULL)
      ae->expr->AcceptVisitor(*this);
    inAggregate = false;

    // save the expression
    string mexp = expr;

    // now, give the aggregate a new name
    char no[32];
    sprintf(no, "agg_q%ld_%ld", queryID, anonAggs++);

    // also put it in the out expression
    texp += no;

    // restore the old expression
    expr = texp;

    // and get the aggregate type
    string atype;
    switch(ae->aggType) {
    case Avg:
      atype = "Average";
      break;

    case Sum:
      atype = "Sum";
      break;

    case Count:
      atype = "Count";
      break;

    case CountAll:
      atype = "Count(*)";
      break;

    case Min:
      atype = "Min";
      break;

    case Max:
      atype = "Max";
      break;

    default:
      break;
    }

    // now obtain all the tables from the columns involved
    set<string> tabs;
    while (!cols.empty()) {
      pair<string, string> p = cols.top();
      cols.pop();
      tabs.insert(p.first);
    }

    // and convert into a vector
    vector<string> baseTables;
    baseTables.resize(tabs.size());
    std::copy(tabs.begin(), tabs.end(), baseTables.begin());

    // create our TopAggregate

#ifdef CONVERTER_DEBUG    
    printf("CONVERTER adding aggregate: %s, %s, %s\n", mexp.c_str(), no, atype.c_str());
#endif

    TopAggregate ta(mexp, no, atype, baseTables);

    // and insert it
    aggs.push_back(ta);

  } else {
    printf("Aggregate expressions not supported in WHERE clause.\n");
  }
}

void Converter::VisitCaseExpression(CaseExpression *ce) {
  if (!whereStage) {


		// We translate the case into the expression: 
    // ( (boolExpr) ? (trueExpr) : (falseExpr) )

    derivedOnlyAtt = false;
		

		// do the boolean expr
		ce->boolExpr->AcceptVisitor(*this);
		
		expr = "CASE("+expr; // this weird thing works around the fact that
											 // boolExpr resets expr
		
		expr+=" , ";
		// do the true
		ce->trueExpr->AcceptVisitor(*this);
		
		expr+=" , ";
		// do the false
		ce->falseExpr->AcceptVisitor(*this);
		
		// end it up
		expr+=")";

  } else {
    printf("CASE expressions not supported in WHERE clause.\n");
  }
}

void Converter::VisitSimpleWhenExpression(SimpleWhenExpression *) {

  // temporarily ignored
}

void Converter::VisitSearchWhenExpression(SearchWhenExpression *) {

  // temporarily ignored
}

void Converter::VisitArithmeticExpression(ArithmeticExpression *ae) {
  if (!whereStage) {

    // set for the derived column
    derivedOnlyAtt = false;

    expr += "(";

    // do the left side
    ae->left->AcceptVisitor(*this);
    
    
    // add the operator
    switch(ae->op) {
    case Plus: 
      expr += " + ";
      break;
      
    case Minus: 
      expr += " - ";
      break;

    case Times: 
      expr += " * ";
      break;

    case Divide: 
      expr += " / ";
      break;

    default:
      break;       
    }

    // do the right side
    ae->right->AcceptVisitor(*this);

    // close the expression
    expr += ")";

  } else {
    printf("Arithmetic expressions not supported in WHERE clause.\n");
  }
}

void Converter::VisitAllFromTable(AllFromTable *t) {

  // this is easy -- put all the attributes from the table into the
  // output set

  // get the catalog, schema and att collection
  Catalog &c = Catalog::GetCatalog();
  Schema sch(t->table);
  c.GetSchema(sch);
  AttributeContainer atts;
  sch.GetAttributes(atts);

  // now, insert each attribute
  for (atts.MoveToStart(); !atts.AtEnd(); atts.Advance()) {
    string n = sch.GetRelationName() + "." + atts.Current().GetName();
    pair<string,string> p;
    p.first = p.second = n;
    outputAtts.insert(p);
    
  }
}

void Converter::VisitDerivedColumn(DerivedColumn *dc) {

  // before going down to the columns, we have to set up some flags in
  // order to determine if they are not just direct attributes but the
  // result of a more complicated expression.
  derivedOnlyAtt = true;

  // go down the expression
  expr = "";
  dc->expr->AcceptVisitor(*this);

  // now insert in the output
  pair<string, string> p;
  p.first = expr;

  // we'll assign an alias if there is none
  if (dc->alias.compare("") == 0 && !derivedOnlyAtt) {
    char no[32];
    sprintf(no, "att_q%ld_%ld", queryID, anonAtts++);
    p.second = no;
  }
  else {
    p.second = dc->alias;
  }

  outputAtts.insert(p);
}
