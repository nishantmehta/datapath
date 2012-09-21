grammar Desc;
options {
    language = C;
    output = AST;
}

@header {
#include <vector>
#include <string>
#include "DataTypeManager.h"

#ifndef TXT
#define TXT(x) ((const char*)(x->getText(x))->chars)
#endif
#ifndef TXTN
#define TXTN(x) ((NormalizeQuotes((const char*)(x->getText(x))->chars)).c_str())
#endif
#ifndef TXTS
#define TXTS(x) ((StripQuotes((const char*)(x->getText(x))->chars)).c_str())
#endif
#ifndef STR
#define STR(X) ( string(TXT(X)) )
#endif
#ifndef STRN
#define STRN(X) ( string(TXTN(X)) )
#endif
#ifndef STRS
#define STRS(X) ( string(TXTS(X)) )
#endif
}

@members {
    static DataTypeManager & dTM = DataTypeManager::GetDataTypeManager();


    string StripQuotes(string str);
    string NormalizeQuotes(string str);
}

/* Keywords */
FUNCTION : 'function' | 'Function' | 'FUNCTION';
OPKEYWORD : 'operator' | 'Operator' | 'OPERATOR';
DATATYPE : 'datatype' | 'Datatype' | 'DATATYPE';
SYNONYM : 'synonym' | 'Synonym' | 'SYNONYM';
TEMPLATE : 'template' | 'Template' | 'TEMPLATE';
GLA : 'gla' | 'Gla' | 'GLA';
GF  : 'gf' | 'Gf' | 'GF';

DEFINE : 'define' | 'Define' | 'DEFINE';
FROM : 'from' | 'From' | 'FROM';
AS : 'as' | 'As' | 'AS';
REQUIRES : 'requires' | 'Requires' | 'REQUIRES';

/* Base stuff */

INT :   '0'..'9'+ 'L'?
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;

ID  :   ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

STRING
    :  '\'' ( ESC_SEQ | ~('\''|'\\') )* '\''
    |  '"' ( ESC_SEQ | ~('\\'|'"') )* '"'
    ;

SEMICOLON : ';' ;
COMMA : ',' ;
COLON : ':' ;
LPAREN : '(' ;
RPAREN : ')' ;
ARROW : '->';

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    |   UNICODE_ESC
    |   OCTAL_ESC
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;

// Actual parser stuff

parse
    : statements
    ;

statements
    : (statement SEMICOLON!)+
    ;

statement
    : DEFINE defineStatement -> defineStatement
    ;

defineStatement
    : DATATYPE n=ID FROM f=STRING
    {
        dTM.AddBaseType(STR($n), STRS($f));
    }
    | SYNONYM syn=ID FROM base=ID
    {
        dTM.AddSynonymType(STR($syn), STR($base));
    }
    | FUNCTION n=ID LPAREN params=typeList RPAREN ARROW ret=ID FROM f=STRING
    {
        dTM.AddFunctions(STR($n), $params.vect, STR($ret), STRS($f), true );
    }
    | OPKEYWORD op=STRING LPAREN params=typeList RPAREN ARROW ret=ID FROM f=STRING
    {
        dTM.AddFunctions(STRS($op), $params.vect, STR($ret), STRS($f), true );
    }
    | GLA n=ID LPAREN params=typeList RPAREN ARROW LPAREN retList=typeList RPAREN FROM f=STRING
    {
        dTM.AddGLA( STR($n), $params.vect, $retList.vect, STRS($f) );
    }
    | GF n=ID LPAREN params=typeList RPAREN ARROW LPAREN retList=typeList RPAREN FROM f=STRING
    {
        dTM.AddGF( STR($n), $params.vect, $retList.vect, STRS($f) );
    }
    | n=ID AS ty=ID
    {
        dTM.AddSynonymType(STR($syn), STR($ty));
    }
  ;

typeList returns [vector<string> vect]
    : a=ID {$vect.push_back(STR($a));} (COMMA b=ID {$vect.push_back(STR($b));})*
    ;
