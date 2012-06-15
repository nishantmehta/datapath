grammar T;

options
{
	language = C;
}

@header
{
	#include<assert.h>
	#include <map>
	#include <string>
}

@members
{
	using namespace std;
	map <string, int> hMap;
}

/** Create a simple calculator*/

/* This is main rule which will be called from main*/
r: stat1+ ;

stat1: 
	// evaluate expr and emit result
	// $expr.value is the return attribute 'value' from expr call
	expr NEWLINE {printf("\n  \%d \n", $expr.value);}
		// match assignment and stored value
	| ID '=' expr NEWLINE {hMap[string((char*)$ID.text->chars)] = $expr.value;}
	| NEWLINE // do nothing
	;

/* return value of multExpr or, if '+'|'-' present, return multiplication of results 
	 from both multExpr references*/
expr returns [int value]
	: e=multExpr {$value = $e.value;} 
		( '+' e=multExpr {$value += $e.value;} | '-' e=multExpr {$value -= $e.value;} )* ;

/* return the value of an atom or, if '*' present, return multiplication of results 
	from both atom references. $value is the return value of this method, $e.value is
	the return value of the rule labeled with e*/
multExpr returns [int value]
	: e=atom {$value = $e.value;} ('*' e=atom {$value *= $e.value;})* 
	;

atom returns [int value]: 
		// value of an INT is the int computed from the char sequence
	  INT {$value = atoi((const char*)$INT.text->chars);}
	| ID { // variable reference
					// lookup value of variable
					int v = hMap[string((char*)$ID.text->chars)];
					// if found, set the value else error
					if (v != 0) $value = v;
					else
						printf("\nError : undefined variable \%s", $ID.text->chars);
			}
	| '(' expr ')' {$value = $expr.value;}
	;

ID: ('a'..'z'|'A'..'Z')+ ;
INT: '0'..'9'+ ;
NEWLINE: '\r'? '\n' ;
WS: (' '|'\n'|'\r')+   {$channel=HIDDEN;} ; // ignore whitespace
