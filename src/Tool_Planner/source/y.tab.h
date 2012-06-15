/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SELECT = 258,
     FROM = 259,
     WHERE = 260,
     DISTINCT = 261,
     ALL = 262,
     COMMA = 263,
     DOT = 264,
     AND = 265,
     OR = 266,
     NOT = 267,
     IS = 268,
     AS = 269,
     LPAREN = 270,
     RPAREN = 271,
     ASTERISK = 272,
     SLASH = 273,
     PLUS = 274,
     MINUS = 275,
     EQUALS = 276,
     NOTEQ = 277,
     LT = 278,
     GT = 279,
     LE = 280,
     GE = 281,
     BETWEEN = 282,
     LIKE = 283,
     AVG = 284,
     COUNT = 285,
     SUM = 286,
     MIN = 287,
     MAX = 288,
     DATE = 289,
     GROUPBY = 290,
     CASE = 291,
     WHEN = 292,
     END = 293,
     THEN = 294,
     ELSE = 295,
     SEMICOLON = 296,
     IDENTIFIER = 297,
     NUMERIC = 298,
     STRING = 299
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 28 "Tool_Planner/source/Parser.y"

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



/* Line 2068 of yacc.c  */
#line 110 "Tool_Planner/source/y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


