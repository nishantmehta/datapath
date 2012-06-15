tree grammar Eval;

options
{
	language = C;
	tokenVocab=T;
	ASTLabelType	= pANTLR3_BASE_TREE;
}

@header{
#ifdef __cplusplus
#include <string>
#include <map>
#endif
}

@members
{
	std::map <std::string, int> hMap;
}

expr returns [int value]:
	^(OPERATOR a=expr b=expr) {
			if (!strcmp((char*)$OPERATOR.text->chars, "+"))
			 $value = a+b;
			else if (!strcmp((char*)$OPERATOR.text->chars, "-"))
			 $value = a-b;
			else if (!strcmp((char*)$OPERATOR.text->chars, "*"))
			 $value = a*b;
			else {
				printf("\nERROR: Not supported operator");
			}
	}
| ID
	{
    // lookup value of variable
    int v = hMap[std::string((char*)$ID.text)];
    // if found, set the value else error
    if (v != -1) $value = v;
    else
    printf("\nError : undefined variable \%s", $ID.text->chars);
  }
| INT {$value = atoi((const char*)$INT.text->chars);}
;

r: stat1+ ;

stat1: 
expr {printf("\n  \%d \n", $expr.value);}
| ^('=' ID expr) {hMap[std::string((char*)$ID.text)] =  $expr.value;}
;
