%{
  // cool includes here

  #include <string>
  #include <vector>
  #include "../headers/AST.h"

  using namespace std;

  extern "C" {
    int yylex(void);
    int yyparse(void);
    int yywrap();
    void yyerror(char *s);

  }

  vector<SFWQuery *> queries;

  vector<ColumnExpression *> gbVector;
  vector<TableReference *> tableVector;
  vector<Expression *> whenVector;
  vector<VColumn *> colVector;
%}


// this big union of types is used by the parser
%union {
  char *strValue;
  double numValue;
  int num;
  SetQuantifier sq;
  SFWQuery *sfw;
  ColumnExpression *cole;
  TableReference *tr;
  Expression *exp;
  BooleanPredicate *pred;
  VColumn *col;
  CompOp op;
}


// token definitions
%token SELECT FROM WHERE
%token DISTINCT ALL
%token COMMA DOT
%token AND OR NOT IS AS
%token LPAREN RPAREN
%token ASTERISK SLASH
%token PLUS MINUS
%token EQUALS NOTEQ LT GT LE GE
%token BETWEEN LIKE
%token AVG COUNT SUM MIN MAX
%token DATE
%token GROUPBY
%token CASE WHEN END THEN ELSE
%token SEMICOLON

// typed terminal tokens go here
%token <strValue> IDENTIFIER
%token <numValue> NUMERIC
%token <strValue> STRING

// start rule
%start querySequence

// types for grammar rules
%type <num> querySequence
%type <sfw> sfwQuery
%type <sq> setQuantifier
%type <cole> columnExpression
%type <tr> tableReference
%type <exp> valueExpression
%type <exp> numericTerm
%type <exp> valueExpressionPrimary
%type <exp> caseExpression
%type <exp> dateExpression
%type <exp> setFunction
%type <pred> searchCondition
%type <pred> whereClause
%type <pred> booleanTerm
%type <pred> booleanFactor
%type <pred> booleanTest
%type <op> comparisonOperator
%type <col> selectSubList
%type <col> derivedColumn
%%

querySequence: sfwQuery SEMICOLON querySequence { 
                                                  queries.insert(queries.begin(), $1); 
						  $$ = 1 + $3;
                                                }

             | sfwQuery SEMICOLON SEMICOLON     { 
                                                  queries.insert(queries.begin(), $1); 
						  $$ = 1;
                                                }
             ;

sfwQuery: SELECT setQuantifier selectList FROM tableReferenceList whereClause groupByClause { 
                                                                                               $$ = new SFWQuery($2, colVector, tableVector, $6, gbVector);  
											       gbVector.clear();
											       tableVector.clear();
											       colVector.clear();
                                                                                            } 
        ;

setQuantifier:          { $$ = All; }
             | ALL      { $$ = All; }
             | DISTINCT { $$ = Distinct; }
             ;

selectList: ASTERISK                          { /* do nothing */ }
          | selectVarious                     { /* nothing here,
						   either */ }
	  ;

selectVarious: selectSubList                     { colVector.push_back($1); }
             | selectSubList COMMA selectVarious { colVector.push_back($1); }
             ;

selectSubList: derivedColumn            { $$ = $1; }
             | IDENTIFIER DOT ASTERISK  { $$ = new AllFromTable($1); }
             ;

derivedColumn: valueExpression                { $$ = new DerivedColumn($1); }
             | valueExpression AS IDENTIFIER  { $$ = new DerivedColumn($1, $3); }
             ;

valueExpression: numericTerm                              { $$ = $1; }
               | numericTerm PLUS numericTerm             { $$ = new ArithmeticExpression($1, $3, Plus); }
               | numericTerm MINUS numericTerm            { $$ = new ArithmeticExpression($1, $3, Minus); }
               ;

numericTerm: valueExpressionPrimary                      { $$ = $1; }
           | numericTerm ASTERISK valueExpressionPrimary { $$ = new ArithmeticExpression($1, $3, Times); }
           | numericTerm SLASH valueExpressionPrimary    { $$ = new ArithmeticExpression($1, $3, Divide); }	
           ;

valueExpressionPrimary: NUMERIC                                                 { $$ = new NumericExpression($1); }
                      | STRING                                                  { $$ = new StringExpression($1); }
                      | dateExpression                                          { $$ = $1; }
                      | setFunction                                             { $$ = $1; }
                      | LPAREN valueExpression RPAREN                           { $$ = $2; }
                      | columnExpression                                        { $$ = $1; }
                      | caseExpression                                          { $$ = $1; }
                      ;

dateExpression : DATE LPAREN STRING RPAREN   { $$ = new DateExpression($3); }
| DATE LPAREN NUMERIC COMMA NUMERIC COMMA NUMERIC RPAREN  { $$ = new DateExpression($3, $5, $7); }
;

columnExpression: IDENTIFIER DOT IDENTIFIER { $$ = new ColumnExpression($1, $3); }
                | IDENTIFIER                { $$ = new ColumnExpression("", $1); }
                ;

caseExpression: CASE WHEN searchCondition THEN valueExpression ELSE valueExpression END 
{
	$$ = new CaseExpression($3, $5, $7);
}

/* caseExpression: CASE valueExpression whenSimple END                      {  */
/*                                                                             $$ = new CaseExpression($2, whenVector);  */
/* 									    whenVector.clear(); */
/*                                                                          } */

/*               | CASE valueExpression whenSimple ELSE valueExpression END {  */
/*                                                                             $$ = new CaseExpression($2, whenVector, $5);  */
/* 									    whenVector.clear(); */
/*                                                                          } */

/*               | CASE whenSimple END                                      {  */
/*                                                                             $$ = new CaseExpression("NOTHING", whenVector);  */
/* 									    whenVector.clear(); */
/*                                                                          } */
/*               ; */

whenSimple: WHEN valueExpression THEN valueExpression whenSimple { whenVector.push_back(new SimpleWhenExpression($2, $4)); }
          | WHEN searchCondition THEN valueExpression whenSimple { whenVector.push_back(new SearchWhenExpression($2, $4)); }
          |                                                      { }
          ;



setFunction: AVG LPAREN setQuantifier valueExpression RPAREN         { $$ = new AggregateExpression(Avg, $3, $4); }
           | SUM LPAREN setQuantifier valueExpression RPAREN         { $$ = new AggregateExpression(Sum, $3, $4); }
           | COUNT LPAREN setQuantifier valueExpression RPAREN       { $$ = new AggregateExpression(Count, $3, $4); }
           | COUNT LPAREN ASTERISK RPAREN                            { $$ = new AggregateExpression(Count); }
           | MIN LPAREN setQuantifier valueExpression RPAREN         { $$ = new AggregateExpression(Min, $3, $4); }
           | MAX LPAREN setQuantifier valueExpression RPAREN         { $$ = new AggregateExpression(Max, $3, $4); }
           ;

tableReferenceList: tableReference                          { tableVector.push_back($1); }
                  | tableReference COMMA tableReferenceList { tableVector.push_back($1); }
                  ;

tableReference: IDENTIFIER               { $$ = new TableReference($1); }
              | IDENTIFIER AS IDENTIFIER { $$ = new TableReference($1, $3); }
              ;

whereClause:                       { $$ = new ComparisonTest(new NumericExpression(1), new NumericExpression(1), Equals); }
           | WHERE searchCondition { $$ = $2; }
           ;

searchCondition: booleanTerm                     { $$ = new AndPredicate($1, NULL); }
               | booleanTerm AND searchCondition { $$ = new AndPredicate($1, $3); }
               ;

booleanTerm: booleanFactor                 { $$ = $1; }
           | booleanTerm OR booleanFactor  { $$ = new OrPredicate($1, $3); }
           ;

booleanFactor: booleanTest     { $$ = $1; }
             | NOT booleanTest { $$ = new NotPredicate($2); }
             ;

booleanTest: valueExpression comparisonOperator valueExpression              { $$ = new ComparisonTest($1, $3, $2); }
           | valueExpression BETWEEN valueExpression AND valueExpression     { $$ = new BetweenTest($1, $3, $5); }
           | valueExpression NOT BETWEEN valueExpression AND valueExpression { $$ = new NotPredicate(new BetweenTest($1, $4, $6)); }
           | valueExpression LIKE valueExpression                            { $$ = new LikeTest($1, $3); }
           | valueExpression NOT LIKE valueExpression                        { $$ = new NotPredicate(new LikeTest($1, $4)); }
           ;

comparisonOperator: EQUALS { $$ = Equals; }
	          | NOTEQ  { $$ = NotEquals; }
	          | LE     { $$ = LessThan; }
		  | GE     { $$ = GreaterThan; }
		  | LT     { $$ = LessEqual; }
		  | GT     { $$ = GreaterEqual; }
                  ;

groupByClause:                         { /* do nothing */ }
             | GROUPBY groupingColumns { /* same here, look down vvvv */ }
             ;

groupingColumns: columnExpression                       { gbVector.push_back($1); }
               | columnExpression COMMA groupingColumns { gbVector.push_back($1); }
               ;

%%

