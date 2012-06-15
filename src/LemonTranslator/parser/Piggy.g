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

parse[LemonTranslator* trans] : {
      // create new query
      qry = GenerateTemp("Q\%d");
      DP_CheckQuery(true, qry);;
    } s=statements
    -> ^(NEWSTATEMENT ^(QUERRY__ ID[qry.c_str()]) ) $s ^(RUN__ ID[qry.c_str()])
;

statements
    :(statement SEMICOLON!)+
    ;

statement
  : a=ID EQUAL actionBody -> ^(NEWSTATEMENT ^(WAYPOINT__ $a actionBody) )
    /* above always creates a new waypoint.  */
  | LOAD a=ID (AS b=ID)? -> ^(NEWSTATEMENT ^(SCANNER__ $a $b) )
  | PRINT a=ID USING exp=expressionList (AS attListWTypes) (INTO file=STRING)?
    -> ^(NEWSTATEMENT ^(WAYPOINT__ ID[$a, "print"] ^(PRINT TERMCONN $a)
        ^(QUERRY__ ID[$a,qry.c_str()] ^(PRINT $exp attListWTypes ^(LIST $file)? ) )))
  | STORE a=ID INTO b=ID
    -> ^(NEWSTATEMENT ^(WRITER__ $b ID[$a, qry.c_str()] TERMCONN $a))
  ;

/* JOIN r1=ID BY l1=attEListAlt COMMA r2=ID BY l2=attEListAlt  */

actionBody
  : JOIN r1=ID BY l1=attEListAlt COMMA r2=ID BY l2=attEListAlt
    ->  ^(JOIN ^(ATTS $l1) $r1 TERMCONN $r2) ^(QUERRY__ ID[$r1,qry.c_str()] ^(JOIN ^(ATTS $l2)))
  | FILTER a=ID BY exp=expressionList
    -> ^(SELECT__ $a) ^(QUERRY__ ID[$a,qry.c_str()] ^(FILTER $exp))
  | GLA (PLUS)? gla=glaDef ct=constArgs (FROM? inp=ID) USING exp=expressionList (AS rez=attListWTypes)?
    -> ^(GLA (PLUS)? $inp) ^(QUERRY__ ID[$inp,qry.c_str()] ^(GLA (PLUS)? $ct $gla $rez $exp))
  | AGGREGATE type=ID (FROM? inp=ID) USING expr=expression AS name=ID
    -> ^(AGGREGATE $inp) ^(QUERRY__ ID[$inp,qry.c_str()] ^(AGGREGATE $name $type $expr))
  | READ FILE? f=STRING (COLON b=INT)? (SEPARATOR s=STRING)? ATTRIBUTES FROM c=ID
        -> ^(TEXTLOADER__ ^(ATTFROM $c) ^(SEPARATOR $s)?  ^(FILE__ $f $b) )
  ;

glaDef
  : COLON ID def=glaTemplateDef?  -> ID ^(GLATEMPLATE $def)?
  ;

glaTemplateDef
  : '<'! glaTemplArg (COMMA! glaTemplArg)* '>'!
  ;

glaTemplArg
  : LSQ attCList RSQ -> ^(LIST attCList)
  | attWType /* single typed argument */
  | ctAtt
  | GLA glaDef
  ;


/* constructor arguments */
constArgs
  : /* noting */
  | LPAREN! ctAttList RPAREN!
  ;

attEListAlt
  : (attribute|synthAttribute)
  | LPAREN! attributeEList RPAREN!
  ;
