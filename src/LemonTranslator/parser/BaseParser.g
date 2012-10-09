parser grammar BaseParser;

/* Do not set options here */

tokens {
  NEWSTATEMENT;
  OLDSTATEMENT;
  ATTS;
  ATT;
  FUNCTION; /* function definition */
  OPDEF; /* operator definition */
  OPERATOR;
  UOPERATOR; /* unary opearator */
  DELWAYPOINT;
  DELQUERY;
  CRDATATYPE;
  CRSYNONYM;
  FCT;
  TPATT;
  ATTFROM;
  ATTWT; /* attribute with types */
  ATTSWT; /* list of attributes with types */
  RUN__;
  QUERRY__;
  WAYPOINT__;
  SELECT__;
  TERMCONN;
  SCANNER__;
  WRITER__;
  LIST;
  CRRELATION;
  FLUSHTOKEN;
  QUITTOKEN;
  FILE__;
  TEXTLOADER__;
  ATTC;
  JOIN_IN; /* type of join */
  JOIN_NOTIN;
  SYNTHESIZE__;
  FUNCTEMPLATE;
  TYPE_;
  CR_TMPL_FUNC;
  STATE__;
  STATE_LIST;

  // Generalized Linear Aggregates
  GLA__;
  GLA_DEF;
  GLATEMPLATE;
  CRGLA;
  CR_TMPL_GLA;
  TYPEDEF_GLA;

  // Generalized Transforms
  GT__;
  GT_DEF;
  GTTEMPLATE;
  CRGT;
  CR_TMPL_GT;
  TYPEDEF_GT;

  // Generalized Filters
  GF__;
  GF_DEF;
  GFTEMPLATE;
  CRGF;
  CR_TMPL_GF;
  TYPEDEF_GF;

  // GISTs
  GIST__;
  GIST_WP;
  GIST_DEF;
  GISTTEMPLATE;
  CRGIST;
  CR_TMPL_GIST;
  TYPEDEF_GIST;
}


/*
  Including C++ headers in the "C" version of Antlr is tricky due to
  the extern "C" statements that get injected in the .h files. To
  avoid compilation problems, you have to make sure that any program
  that uses DataPath.h includes it last and includes all headers
  mentioned below first. This is annoying but there does not seem to
  be any way around it.

 */

@header {
#ifndef BASE_GRAMMAR_INCLUDE
#define BASE_GRAMMAR_INCLUDE
extern "C++" {
 #include "SymbolicWaypointConfig.h"
 #include "LemonTranslator.h"
 #include "AttributeManager.h"
 #include "QueryManager.h"
 #include <iostream>

 #define P_ERR_IF(cond,msg...) {\
   if (cond) {\
         fprintf(stderr, "WARNING : ");\
     fprintf(stderr, msg);\
         fprintf(stderr, "\n");\
     haveErrors=true;\
   }\
    }

#define DP_CheckQuery(IsNew,query) {\
    QueryManager& qm = QueryManager::GetQueryManager();\
    if (!IsNew) {\
        string qName;\
        P_ERR_IF(qm.GetQueryName(qm.GetQueryID(query),qName), "Query Not registered");\
    } else {\
      QueryID bogus;\
      qm.AddNewQuery(query, bogus);\
    }\
}

extern string qry;
extern int tempCounter; // id for temporary variables}
extern bool haveErrors;
string StripQuotes(string str);
string NormalizeQuotes(string str);
string GenerateTemp(const char* pattern);

}
#endif //BASE_GRAMMAR_INCLUDE
}

type
    : ID typeTemplate?
    ;

typeTemplate
    : '<' typeTemplateParam (COMMA typeTemplateParam)* '>'
    ;

typeTemplateParam
    : type
    | INT
    ;

typeList
    : type (COMMA! type)*
    ;

ctAttList
    : /* nothing */ ->
    | ctAtt (COMMA! ctAtt )*
    ;

ctAtt  : STRING | INT | FLOAT;

attListWTypes
    : /* nothing */ ->
    | attWType ( COMMA ! attWType )*
    ;

attWType
    : att=ID COLON type -> ^(ATTWT $att type )
    ;

attCList
    : /* nothing */ ->
    | attC ( COMMA ! attC )*
    ;

attC
    : attCElem -> ^(ATTC attCElem)
    ;

fragment attCElem
    : ID (COLON! type)*
    ;

idList     :    ID ( COMMA ID)*
    ;

// We get the list to which we add our arguments as an argument
attributeList :    attribute  ( COMMA! attribute)*  ;

// extended list
attributeEList :    (attribute|synthAttribute)  ( COMMA! (attribute|synthAttribute))*  ;

attribute
@init {string longName;}
    :     rel=ID DOT att=ID {
            // check sanity of attribute
            longName = (char*)$rel.text->chars;
            longName += "_";
            longName += (char*)$att.text->chars;
            AttributeManager& am = AttributeManager::GetAttributeManager();
            SlotID slot = am.GetAttributeSlot(longName.c_str());
            WARNINGIF( !slot.IsValid(), "Attribute does not exist");
        } -> ATT[$att, longName.c_str()]
     ;

synthAttribute
@init {string longName; }
    :    ID {
      longName = qry;
      longName += "_";
      longName += (char*)$ID.text->chars;
      // check delayed
      // to check accumulate all atttributes of a query in a list
    } -> ATT[$ID, longName.c_str()]
  ;


/** Grammar from  http://www.antlr.org/grammar/1153358328744/C.g */

conditional_expression
    : logical_or_expression (QMARK^ expression COLON! expression)?
    ;

logical_or_expression
    //: logical_and_expression ('||'^ logical_and_expression)*
    : (logical_and_expression -> logical_and_expression) (
        o=LOR e=logical_and_expression -> ^(OPERATOR[$o, $o.text->chars] $logical_or_expression $e)
        )*
    ;

logical_and_expression
    //: inclusive_or_expression ('&&'^ inclusive_or_expression)*
    : (inclusive_or_expression -> inclusive_or_expression) (
        o=LAND e=inclusive_or_expression -> ^(OPERATOR[$o, $o.text->chars] $logical_and_expression $e)
        )*
    ;

inclusive_or_expression
    //: exclusive_or_expression ('|'^ exclusive_or_expression)*
    : (exclusive_or_expression -> exclusive_or_expression) (
        o=BOR e=exclusive_or_expression -> ^(OPERATOR[$o, $o.text->chars] $inclusive_or_expression $e)
        )*
    ;

exclusive_or_expression
    //: and_expression ('^'^ and_expression)*
    : (and_expression -> and_expression) (
        o=XOR e=and_expression -> ^(OPERATOR[$o, $o.text->chars] $exclusive_or_expression $e)
        )*
    ;

and_expression
    //: basic_bool_expression ('&'^ basic_bool_expression)*
    : (basic_bool_expression -> basic_bool_expression) (
        o=BAND e=basic_bool_expression -> ^(OPERATOR[$o, $o.text->chars] $and_expression $e)
        )*
    ;

basic_bool_expression
    : equality_expression
    | match_expression
    | BOOL_T
    ;

match_expression
    : MATCH_DP LPAREN expr=expression COMMA patt=STRING RPAREN
    -> ^(MATCH_DP $patt $expr)
  ;

case_expression
    : CASE_DP LPAREN test=expression COMMA yes=expression COMMA no=expression RPAREN
        ->^(CASE_DP $test $yes $no)
    ;

equality_expression
    //: relational_expression (('=='|'!=')^ relational_expression)*
    : (relational_expression -> relational_expression) (
        (o=ISEQUAL|o=NEQUAL) e=relational_expression -> ^(OPERATOR[$o, $o.text->chars] $equality_expression $e)
        )*
    ;

relational_expression
    //: shift_expression (('<'|'>'|'<='|'>=')^ shift_expression)*
    : (shift_expression -> shift_expression) (
        (o=LS|o=GT|o=LE|o=GE) e=shift_expression -> ^(OPERATOR[$o, $o.text->chars] $relational_expression $e)
        )*
    ;

shift_expression
    //: additive_expression (('<<'|'>>')^ additive_expression)*
    : (additive_expression -> additive_expression) (
        (o=SLEFT|o=SRIGHT) e=additive_expression -> ^(OPERATOR[$o, $o.text->chars] $shift_expression $e)
        )*
    ;

 additive_expression
    //: (multiplicative_expression) ('+'^ multiplicative_expression | '-'^ multiplicative_expression)*
    : (multiplicative_expression -> multiplicative_expression) (
        (o=PLUS|o=MINUS) e=multiplicative_expression -> ^(OPERATOR[$o, $o.text->chars] $additive_expression $e)
        )*
    ;

multiplicative_expression
    //: (unary_expression) ('*'^ unary_expression | '/'^ unary_expression | '%'^ unary_expression)*
    : (unary_expression -> unary_expression) (
        (o=TIMES|o=DIVIDE|o=MOD) e=unary_expression -> ^(OPERATOR[$o, $o.text->chars] $multiplicative_expression $e)
        )*
    ;


unary_expression
    :    primary_expression
    |    (o=PLUS|o=MINUS|o=LNOT|o=NOT)  e=unary_expression -> ^(UOPERATOR[$o, $o.text->chars] $e)
    ;

primary_expression
    : attribute
    | synthAttribute
    | constant
    | funct
    | case_expression
    | LPAREN! expression RPAREN!
    ;

funct
    :    name=ID LPAREN expressionList RPAREN -> ^(FUNCTION $name expressionList)
    |    UDF COLON name=ID functionTemplate LPAREN expressionList RPAREN retType=functionRetType ->^(FUNCTION $name $retType functionTemplate expressionList)
    ;

fragment functionRetType
    : /* nothing */
    | '->' ID -> ^(TYPE_ ID)
    ;

functionTemplate
    : /* nothing */
    | '<' a=functionTemplateArg (COMMA b=functionTemplateArg)* '>' ->
        ^(FUNCTEMPLATE $a $b*)
    ;

functionTemplateArg
    : LSQ attCList RSQ -> ^(LIST attCList)
    | attC
    | ctAtt
    ;

expressionList
    : /* nothing*/ ->
    | expression (COMMA! expression)* // -> $a
    ;

constant
    :   INT
    |   STRING
    |   FLOAT
    ;

expression
    :    conditional_expression
    ;



