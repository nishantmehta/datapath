%{
  #include <string.h>
  #include "../headers/AST.h"
  #include "y.tab.h"

  extern "C" void yyerror(char *s);
  extern "C" int yylex();
  int lineno = 1;
%}

%%

"SELECT"     { return SELECT; }
"FROM"       { return FROM; }
"WHERE"      { return WHERE; }
"DISTINCT"   { return DISTINCT; }
"ALL"        { return ALL; }
","          { return COMMA; }
"."          { return DOT; }
"AND"        { return AND; }
"OR"         { return OR; }
"NOT"        { return NOT; }
"IS"         { return IS; }
"AS"         { return AS; }
"BETWEEN"    { return BETWEEN; }
"LIKE"       { return LIKE; }
"AVG"        { return AVG; }
"COUNT"      { return COUNT; }
"SUM"        { return SUM; }
"MIN"        { return MIN; }
"MAX"        { return MAX; }
"GROUP BY"   { return GROUPBY; }
"CASE"       { return CASE; }
"WHEN"       { return WHEN; }
"THEN"       { return THEN; }
"ELSE"       { return ELSE; }
"END"        { return END; }
"DATE"       { return DATE; }
"("          { return LPAREN; }
")"          { return RPAREN; }
"*"          { return ASTERISK; }
"/"          { return SLASH; }
"+"          { return PLUS; }
"-"          { return MINUS; }
"="          { return EQUALS; }
"<>"         { return NOTEQ; }
"<="         { return LE; }
">="         { return GE; }
"<"          { return LT; }
">"          { return GT; }
";"          { return SEMICOLON; }
"\n"         { lineno++; }
[ \t]        { }

-?[0-9]+("."[0-9]+)?                            { yylval.numValue = atof(yytext); return NUMERIC; }
"\'"[a-zA-Z0-9".""_""#""$""%""*""\-"]*"\'"      { yylval.strValue = strdup(yytext); return STRING; }
[a-zA-Z][a-zA-Z0-9"_"]*                         { yylval.strValue = strdup(yytext); return IDENTIFIER; }


.            { yyerror("Unrecognized character"); }

%%

void yyerror(char *s) {
  printf("%s [%s] [line %i]\n", s, yytext, lineno);
  YY_FLUSH_BUFFER;
  lineno = 1;
}

int yywrap() {
  lineno = 1;
  return 1;
}
