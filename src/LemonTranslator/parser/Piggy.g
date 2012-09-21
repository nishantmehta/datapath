grammar Piggy;
options {
    language = C;
    output = AST;
}

import BaseLexer, BaseParser;



/* Keywords */

LOAD : 'load' | 'Load' | 'LOAD' ;
READ : 'read' | 'Read' | 'READ' ;
USING : 'using' | 'Using' | 'USING';
FROM : 'from' | 'From' | 'FROM' ;
BY : 'by' | 'By' | 'BY' ;
STORE : 'store' | 'Store' | 'STORE';
AS : 'as' | 'As' | 'AS';
TO : 'to' | 'To' | 'TO';
INTO : 'into' | 'Into' | 'INTO';
IN : 'in' | 'In' | 'IN';
NOT_K : 'not' | 'Not' | 'NOT';
FOREACH : 'foreach' | 'Foreach' | 'FOREACH';
GENERATE : 'generate' | 'Generate' | 'GENERATE';
DEFINE : 'define' | 'Define' | 'DEFINE';
REQUIRES : 'requires' | 'Requires' | 'REQUIRES';
SELF : 'self' | 'Self' | 'SELF';

parse[LemonTranslator* trans]
    : {
      // create new query
      qry = GenerateTemp("Q\%d");
      DP_CheckQuery(true, qry);;
    } s=statements quitstatement?
    -> ^(NEWSTATEMENT ^(QUERRY__ ID[qry.c_str()]) ) $s ^(RUN__ ID[qry.c_str()]) quitstatement?
;

quitstatement
    : QUIT SEMICOLON -> QUITTOKEN
    ;

statements
    :(statement SEMICOLON!)+
    ;

statement
  : a=ID EQUAL actionBody -> ^(NEWSTATEMENT ^(WAYPOINT__ $a actionBody) )
    /* above always creates a new waypoint.  */
  | LOAD a=ID (AS b=ID)? -> ^(NEWSTATEMENT ^(SCANNER__ $a $b) )
  | PRINT a=ID USING exp=expressionList (AS header=attCList)? (INTO file=STRING)? (SEPARATOR sep=STRING)?
    -> ^(NEWSTATEMENT ^(WAYPOINT__ ID[$a, "print"] ^(PRINT TERMCONN $a)
        ^(QUERRY__ ID[$a,qry.c_str()] ^(PRINT $exp ($header)? ^(LIST $file)? ^(SEPARATOR $sep)? ) )))
  | STORE a=ID INTO b=ID
    -> ^(NEWSTATEMENT ^(WRITER__ $b ID[$a, qry.c_str()] TERMCONN $a))
  | CREATE createStatement -> createStatement
  | DEFINE defineStatement -> defineStatement
  ;

createStatement
  : RELATION n=ID LPAREN tpAttList RPAREN -> ^(CRRELATION $n tpAttList)
  ;

defineStatement
  : DATATYPE n=ID FROM f=STRING -> ^(CRDATATYPE $n $f)
  | SYNONYM syn=ID FROM base=ID -> ^(CRSYNONYM $base $syn)
  | FUNCTION n=ID LPAREN params=typeList RPAREN ARROW ret=type FROM f=STRING
    -> ^(FUNCTION $n $f $ret $params)
  | OPKEYWORD n=STRING LPAREN params=typeList RPAREN ARROW ret=type FROM f=STRING
    -> ^(OPDEF $n $f $ret $params)
  | GLA n=ID LPAREN params=typeList RPAREN ARROW LPAREN retList=typeList RPAREN FROM f=STRING
    -> ^(CRGLA $n $f ^(TPATT $retList) ^(TPATT $params))
  | GF n=ID LPAREN params=typeList RPAREN ARROW LPAREN retList=typeList RPAREN FROM f=STRING
    -> ^(CRGF $n $f ^(TPATT $retList) ^(TPATT $params))
  | TEMPLATE FUNCTION n=ID FROM f=STRING
    -> ^(CR_TMPL_FUNC $n $f)
  | TEMPLATE GLA n=ID FROM f=STRING
    -> ^(CR_TMPL_GLA $n $f)
  | TEMPLATE GF n=ID FROM f=STRING
    -> ^(CR_TMPL_GF $n $f)
  | GLA COLON n=ID AS GLA glaDef
    -> ^(TYPEDEF_GLA $n glaDef)
  | GF COLON n=ID AS GF gfDef
    -> ^(TYPEDEF_GF $n gfDef)
  | n=ID AS ty=type
    -> ^(CRSYNONYM $ty $n)
  ;

tpAttList
  : tpAtt ( COMMA! tpAtt)*
  ;

tpAtt
  : var=ID COLON dtype=ID -> ^(TPATT $var $dtype)
  ;

inStmt
    : IN -> JOIN_IN
    | NOT_K IN -> JOIN_NOTIN
    ;

actionBody
    : JOIN r1=ID BY l1=attEListAlt COMMA r2=ID BY l2=attEListAlt
        ->  ^(JOIN ^(ATTS $l1) $r1 TERMCONN $r2) ^(QUERRY__ ID[$r1,qry.c_str()] ^(JOIN ^(ATTS $l2)))
    | FILTER a=ID BY exp=expressionList
        -> ^(SELECT__ $a) ^(QUERRY__ ID[$a,qry.c_str()] ^(FILTER $exp))
    | FILTER r1=ID USING l1=attEListAlt inStmt r2=ID LPAREN l2=attEListAlt RPAREN
        ->  ^(JOIN ^(ATTS $l1) $r1 TERMCONN $r2) ^(QUERRY__ ID[$r1,qry.c_str()] ^(JOIN inStmt ^(ATTS $l2)))
    | GLA gla=glaDef ct=constArgs (FROM? inp=ID) st=stateArgs USING exp=expressionList (AS rez=glaRez)?
        -> ^(GLA $inp) ^(QUERRY__ ID[$inp,qry.c_str()] ^(GLA $ct $st $gla $rez $exp))
    | GF gf=gfDef ct=constArgs (FROM? inp=ID) st=stateArgs USING exp=expressionList (AS res=attListWTypes)
        -> ^(GF__ $inp) ^(QUERRY__ ID[$inp, qry.c_str()] ^(GF__ $ct $st $gf $res $exp))
    | AGGREGATE t=ID (FROM? inp=ID) USING expr=expression AS name=ID
        -> ^(AGGREGATE $inp) ^(QUERRY__ ID[$inp,qry.c_str()] ^(AGGREGATE $name $t $expr))
    | READ FILE? f=STRING (COLON b=INT)? (SEPARATOR s=STRING)? ATTRIBUTES FROM c=ID
        -> ^(TEXTLOADER__ ^(ATTFROM $c) ^(SEPARATOR $s)?  ^(FILE__ $f $b) )
    | FOREACH a=ID GENERATE generateList
        -> ^(SELECT__ $a) ^(QUERRY__ ID[$a,qry.c_str()] generateList )
    ;

generateItem
    : e=expression AS a=ID COLON t=ID -> ^(SYNTHESIZE__ $a $t $e) ;

generateList
    : generateItem (COMMA! generateItem)* ;

glaDef
  : COLON ID def=glaTemplateDef?  -> ID ^(GLATEMPLATE $def)?
  ;

glaTemplateDef
  : '<'! glaTemplArg (COMMA! glaTemplArg)* '>'!
  ;

glaTemplArg
  : LSQ attCList RSQ -> ^(LIST attCList)
  | attC /* single typed argument */
  | ctAtt
  | GLA glaDef
  ;

glaRez
    : attListWTypes
    | SELF -> STATE__
    ;

gfDef
    : COLON ID def=gfTemplateDef? -> ID ^(GFTEMPLATE $def)?
    ;

gfTemplateDef
    : '<'! gfTemplArg (COMMA! gfTemplArg)* '>'!
    ;

gfTemplArg
    : LSQ attCList RSQ -> ^(LIST attCList)
    | attC
    | ctAtt
    | GLA glaDef
    ;

/* constructor arguments */
constArgs
  : /* noting */
  | LPAREN! ctAttList RPAREN!
  ;

stateArgs
    : REQUIRES! stateArg+
    | /* nothing */
    ;

stateArg
    : ID -> TERMCONN ID
    ;

attEListAlt
  : (attribute|synthAttribute)
  | LPAREN! attributeEList RPAREN!
  ;
