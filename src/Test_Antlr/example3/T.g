grammar T;

options
{
	language = C;
	output = AST;
}

tokens {
	OPERATOR;
}
/** Create a simple calculator*/

/* This is main rule which will be called from main*/
r: (stat1 {/*printf("  \%s  ", $stat1.tree==NULL?"NULL":$stat1.tree.toStringTree();*/})+;

stat1: 
	expr NEWLINE -> expr
	| ID '=' expr NEWLINE -> ^('=' ID expr)
	| NEWLINE  ->
	;

expr:
	(multExpr -> multExpr) ((o='+'|o='-') m=multExpr -> ^(OPERATOR[$o, $o.text->chars] $expr $m) )*;

multExpr:
	(atom -> atom) (o='*' m=atom -> ^(OPERATOR[$o, $o.text->chars] $multExpr $m) )*;

atom:
		// value of an INT is the int computed from the char sequence
	  INT
	| ID
	| '('! expr ')'!
	;

ID: ('a'..'z'|'A'..'Z')+ ;
INT: '0'..'9'+ ;
NEWLINE: '\r'? '\n' ;
WS: (' '|'\n'|'\r')+   {$channel=HIDDEN;} ; // ignore whitespace
//fragment
//OPERATOR : '+'|'-'|'*' ;
