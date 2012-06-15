dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
include(Resources-T.m4)dnl
m4_divert(0)dnl
dnl # This file contains macros to "smoothen up" the injection of SQL code in C/C++
/* This file is generated from the corresponding .cc.m4 file (look in m4/ directory)

   ANY MODIFICATIONS ARE WIPED OUT NEXT TIME maker RUNS OR CHANGES ARE MADE TO .cc.m4 file. 
	 BETTER NOT TO TOUCH -- looking is encouraged, though
*/

#include <sqlite3.h>
#include <assert.h>
#include "Debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
m4_divert(-1)
dnl # macro to deal with multi line strings
m4_define(</M4_MULTI_LINE/>,</dnl
<//>m4_translit(</$@/>,</
/>,)dnl
/>)

dnl # macro to execute one or more statements without a result
dnl # $1=command in "" or a char* with the command
dnl # $2=</val1, .../> optional argument for printf like behavior. Thext must contain %d, %f or %s
m4_define(</SQL_STATEMENTS_NOREZ/>,</dnl
	zSql_INTERNAL = M4_MULTI_LINE($1) ;

<//>m4_case($#,1,<//>,</
    sprintf(buffer_INTERNAL,zSql_INTERNAL, $2);
		zSql_INTERNAL = buffer_INTERNAL;
<//>/>)

  PDEBUG(zSql_INTERNAL);

  // want to get all the statements inside
	while(true){
    stmt_INTERNAL = 0;
		rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

		// check if we get an empty statement (seems to go forever
		if (zSql_INTERNAL == zLeftover_INTERNAL)
		  break;

		if( rc_INTERNAL != SQLITE_OK ){
      fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
			   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
      zSql_INTERNAL = zLeftover_INTERNAL;
 			fprintf(stderr, "Abandonning all other statements in batch");
      break;
    }

    if( !stmt_INTERNAL ){
      /* this happens for a comment or white-space */
      zSql_INTERNAL = zLeftover_INTERNAL;
      continue;
    }

		if (sqlite3_column_count(stmt_INTERNAL) != 0)
		  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			  zSql_INTERNAL);

    // running the statement
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW ){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				
				assert(false);
				
    }

    zSql_INTERNAL = zLeftover_INTERNAL;

		sqlite3_finalize(stmt_INTERNAL);

  }

/>)

dnl # macro to deal with parametric queries that do not return anything
dnl # there is a version of the macro for scalar return values
dnl # $1=SQL statement with ?1, ?2 ... as the plug in parameters
dnl # $2=list of types of parameters. The size of the list has to coincide
dnl #   with the number of parameters
dnl #   the list is of the form </int, double, text/>
m4_define(</SQL_STATEMENT_PARAMETRIC_NOREZ/>,</dnl
<//>m4_pushdef(</SQL_INTERNAL_PARAMETER_LIST/>, </$2/>)
dnl # we deal with the lis when we expand parameters
  zSql_INTERNAL = M4_MULTI_LINE($1) ;

  PDEBUG(zSql_INTERNAL);

	stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
  	   zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  if (sqlite3_column_count(stmt_INTERNAL) != 0)
		 fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
	  	  zSql_INTERNAL);

  // the statement gets run when parameters get instantiated
/>)

dnl # auxiliary macro to help with the next macro
dnl # $@ list of types

dnl # macro to instantiate parameters of previous parametric statement
dnl # it is WRONG to call this outside PARAMETRIC:END-PARAMETRIC construct
dnl # $1=value or expression of first parameter (must have type compatible to declared type)
dnl # $2= second parameter, etc
m4_define(</SQL_INSTANTIATE_PARAMETERS/>,</dnl
  sqlite3_reset(stmt_INTERNAL);
  sqlite3_clear_bindings(stmt_INTERNAL);

<//>dnl # we go through the list of parameters and generate code to extract them
<//>dnl # when done we run the statement
<//>m4_pushdef(</SQL_INTERNAL_PARAM_COUNT/>,1)dnl
<//>m4_pushdef(</SQL_INTERNAL_LIST_VALS/>,</$@/>)dnl
<//>m4_foreach(</_P_/>,m4_quote(SQL_INTERNAL_PARAMETER_LIST),</dnl
<//><//>dnl # below the brackets are doubled since LIST is a list
<//><//>m4_case(_P_,int,</dnl definition for int
  sqlite3_bind_int64(stmt_INTERNAL, SQL_INTERNAL_PARAM_COUNT, m4_first((SQL_INTERNAL_LIST_VALS)));
/>,double,</dnl definition for double
  sqlite3_bind_double(stmt_INTERNAL, SQL_INTERNAL_PARAM_COUNT, m4_first((SQL_INTERNAL_LIST_VALS)));
/>,text,</dnl definition for text
  sqlite3_bind_text(stmt_INTERNAL, SQL_INTERNAL_PARAM_COUNT, m4_first((SQL_INTERNAL_LIST_VALS)), -1, SQLITE_TRANSIENT);
/>,</dnl default: unknown type
</#/>error The type "_P_" is unsupported. Please use int, double or text
<//><//>/>)dnl
<//><//>dnl # increment parameter counter
<//><//>m4_define(</SQL_INTERNAL_PARAM_COUNT/>, m4_incr(SQL_INTERNAL_PARAM_COUNT))dnl
<//><//>dnl # move to next parameter
<//><//>m4_define(</SQL_INTERNAL_LIST_VALS/>,m4_quote(m4_shift(SQL_INTERNAL_LIST_VALS)))dnl
<//>/>)
<//>m4_popdef(</SQL_INTERNAL_PARAM_COUNT/>)dnl
<//>m4_popdef(</SQL_INTERNAL_LIST_VALS/>)dnl
  // running the query
	rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
  if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
    fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }


/>)

dnl # macro to end a parametric definition
m4_define(</SQL_PARAMETRIC_END/>,</dnl
  sqlite3_finalize(stmt_INTERNAL);
<//>m4_popdef(</SQL_INTERNAL_PARAMETER_LIST/>);
/>)


dnl # macro to run a statement that returns a scalar and store the value into
dnl # $1 = the SQL statement
dnl # $2 = the variable to be defined
dnl # $3 = the type (compatible with the value returned)
dnl # $4 = optinal printf parameters
dnl # the only supported values for type are int, double, text (equivqlent to const char*)
m4_define(</SQL_STATEMENT_SCALAR/>,</
  zSql_INTERNAL = M4_MULTI_LINE($1);

<//>m4_case($#,3,<//>,</
    sprintf(buffer_INTERNAL,zSql_INTERNAL, $4);
		zSql_INTERNAL = buffer_INTERNAL;
<//>/>)

  PDEBUG(zSql_INTERNAL);

  stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

	if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
		  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

	if (sqlite3_column_count(stmt_INTERNAL) != 1)
	  fprintf(stderr, "Statement %s \n returned a result and was not supposed to.\n",
			 zSql_INTERNAL);

	rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
  if( rc_INTERNAL != SQLITE_ROW ){
    fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

	// define the variable with the value
<//>m4_case($3,int,</dnl definition for int
  long int $2 = sqlite3_column_int(stmt_INTERNAL,0);
/>,double,</dnl definition for double
  double $2 = sqlite3_column_double(stmt_INTERNAL,0);
/>,text,</dnl definition for text
	// we copy the string so it is not overwriten
  char* $2 = strdup((char*)sqlite3_column_text(stmt_INTERNAL,0));
/>,</dnl default: unknown type
</#/>error The type "$3" is unsupported. Please use int, double or text
/>)

	sqlite3_finalize(stmt_INTERNAL);

/>)

dnl # macro to implement sql statements that return a table
dnl # code to process rows in the table can be passed
dnl # $1=SQL statement (only one allowed)
dnl # $2=variables and  types to be defined for each result attribute
dnl # $3=optional printf arguments
dnl # e.g. </(a,int), (b,double), (c,text)/> specifies that variable a will
dnl #   the value of the first column as an int, ..
dnl # all the code between this statement and SQL_END_STATEMENT_TABLE is assumed to be normal code
m4_define(</SQL_STATEMENT_TABLE/>,</
  zSql_INTERNAL = M4_MULTI_LINE($1);

<//>m4_case($#,2,<//>,</
    sprintf(buffer_INTERNAL,zSql_INTERNAL, $3);
		zSql_INTERNAL = buffer_INTERNAL;
<//>/>)

  PDEBUG(zSql_INTERNAL);

  stmt_INTERNAL = 0;
	rc_INTERNAL = sqlite3_prepare_v2(db_INTERNAL, zSql_INTERNAL, -1, &stmt_INTERNAL, &zLeftover_INTERNAL);

	if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot compile statement %s\n The error is %s\n",
		  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
  }

  while(true){
	  // read a row
		rc_INTERNAL = sqlite3_step(stmt_INTERNAL);
 		if( rc_INTERNAL != SQLITE_DONE && rc_INTERNAL != SQLITE_ROW){
      fprintf(stderr, "Cannot run statement %s\n The error is %s\n",
			  zSql_INTERNAL, sqlite3_errmsg(db_INTERNAL));
				break;
    }

		if (rc_INTERNAL == SQLITE_DONE) // nothing more to do
		  break;

    // read the columns
<//>m4_pushdef(</CNT/>,0)dnl
<//>m4_foreach(</_V_/>,</$2/>, </dnl
<//><//>m4_case(m4_second(_V_),int,</dnl int def
    long int m4_first(_V_) = sqlite3_column_int64(stmt_INTERNAL,CNT);
/>, double, </dnl double case
    double m4_first(_V_) = sqlite3_column_double(stmt_INTERNAL,CNT);
/>, text, </dnl text branch
    char* m4_first(_V_) = strdup((char*)sqlite3_column_text(stmt_INTERNAL,CNT));
/>,</dnl default: unknown type
</#/>error The type "$3" is unsupported. Please use int, double or text
/>)dnl
<//><//>m4_define(</CNT/>, m4_incr(CNT))dnl
/>)dnl
<//>m4_popdef(</CNT/>)dnl

    // user defined code
<//>dnl
/>)

dnl # end macro for previous macro
m4_define(</SQL_END_STATEMENT_TABLE/>,</
  }

	sqlite3_finalize(stmt_INTERNAL);
/>)

dnl # macro to connect to a new database
dnl # $1=file containing database
m4_define(</SQL_OPEN_DATABASE/>,</
  sqlite3 *db_INTERNAL; // database
  int rc_INTERNAL = SQLITE_OK; // error codes
	sqlite3_stmt* stmt_INTERNAL; //statement
	char* pzTail_INTERNAL; // tail pointer
	int nCol_INTERNAL; // number of columns
	const char *zLeftover_INTERNAL; // leftover for processing multiple statements
	const char *zSql_INTERNAL;
  char buffer_INTERNAL[10000];

  rc_INTERNAL = sqlite3_open($1, &db_INTERNAL);
  if( rc_INTERNAL != SQLITE_OK ){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db_INTERNAL));
    sqlite3_close(db_INTERNAL);
    exit(1);
  }

 // we start a transaction to make sure the code runs faster
 SQL_STATEMENTS_NOREZ("BEGIN TRANSACTION;")
#ifdef DEBUG
 printf("\nBEGIN TRANSACTION;");
#endif
/>)

dnl # macro to latch on an existing database
dnl # this macro is useful in situations in which an independent snipet of SQL code needs to be
dnl # written but run in the same transaction as another snippet
dnl # $1=sqlite3*
m4_define(</SQL_EXISTING_DATABASE/>,</
  sqlite3 *db_INTERNAL = $1; // database
  int rc_INTERNAL = SQLITE_OK; // error codes
	sqlite3_stmt* stmt_INTERNAL; //statement
	char* pzTail_INTERNAL; // tail pointer
	int nCol_INTERNAL; // number of columns
	const char *zLeftover_INTERNAL; // leftover for processing multiple statements
	const char *zSql_INTERNAL;
  char buffer_INTERNAL[10000];
/>)


dnl # macro to get access to teh database object so that it can be passed to another code snippet
m4_define(</SQL_DATABASE_OBJECT/>,</db_INTERNAL/>)

dnl # macro to close the database
m4_define(</SQL_CLOSE_DATABASE/>,</
  SQL_STATEMENTS_NOREZ("END TRANSACTION;")
#ifdef DEBUG
  printf("\nEND TRANSACTION;");
#endif 
  sqlite3_close(db_INTERNAL);
/>)

m4_divert(0)dnl
