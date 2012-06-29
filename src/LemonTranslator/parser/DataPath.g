grammar DataPath;

options {
    language = C;
    output = AST;
}

import BaseLexer, BaseParser;


@members{

bool haveErrors = false;

static string wp; // this is the currect waypoint. If null, illegal to have a waypoint
 // related statement
static WaypointType wpType = InvalidWaypoint; //type of above waypoint

/* shared with all the other grammars */
string qry; // current query. Empty means illegal ...

static std::map<WayPointID, WaypointType> typeMap; // map of waypoints to types

static bool isNew = false; // is this a new anything.

int tempCounter=1; // id for temporary variables


/* These functions are needed in most parser files.
   Declared everywhere, defined here */
// Argument to this is STRING token, it must have quotes
string StripQuotes(string str) {
  string rez;
  rez = str.substr(1, str.size() - 2);
  return rez;
}
string NormalizeQuotes(string str) {
  string temp;
  temp = str.substr(1, str.size() - 2);
  string rez;
  rez += "\"";
  rez += temp;
  rez += "\"";
  return rez;
}

string GenerateTemp(const char* pattern){
  char buffer[100];
  sprintf(buffer, pattern, tempCounter++);
  string rez = buffer;
  return rez;
}


}

/* Keywords */

QUERRY     :    'query' | 'Query' | 'QUERY' ;
BYPASS    :    'bypass' | 'Bypass' | 'BYPASS' ;

SYNTHESIZE
    :    'synthesize' | 'Synthesize ' | 'SYNTHESIZE' ;

NEW     :     'new' | 'New' | 'NEW' ;
DELETE : 'delete' | 'Delete' | 'DELETE';
DROP : 'drop' | 'Drop' | 'DROP';
RUN : 'run' | 'Run' | 'RUN';

DATATYPE : 'datatype' | 'Datatype' | 'DATATYPE';
FROM : 'from' | 'From' | 'FROM';
FUNCTION : 'function' | 'Function' | 'FUNCTION';
OPKEYWORD : 'operator' | 'Operator' | 'OPERATOR';

WAYPOINT
    :    'waypoint'
    |    'Waypoint'
    |    'WayPoint'
    |    'WAYPOINT'
    ;

SCANNER
    :    'scanner'
    |    'Scanner'
    |    'SCANNER'
    ;


SELECT  :   'select' | 'Select' | 'SELECT' ;

CONNECTIONS
    :    ':-' ;

WRITER : 'writer' | 'Writer' | 'WRITER' ;

parse[LemonTranslator* trans] : {
      trans->FillTypeMap(typeMap);
    } complexStatement+ ;

complexStatement
  : NEW statement[true] -> ^(NEWSTATEMENT statement)
  | statement[false] -> ^(OLDSTATEMENT statement)
  | DELETE delStmt SEMICOLON -> delStmt
  | CREATE crStmt SEMICOLON -> crStmt
  | DROP drStmt  SEMICOLON -> drStmt
  | FLUSH SEMICOLON -> FLUSHTOKEN
  | QUIT SEMICOLON -> QUITTOKEN
  | r=RUN QUERRY i+=ID (COMMA i+=ID)* SEMICOLON -> ^(RUN__[$r] $i)
  | COMMENT -> /* no tree */
  ;

statement[bool isNew]
    : scanner[isNew]
    | waypoint[isNew]
    | query[isNew]
    | writer[isNew]
    ;

delStmt
  : WAYPOINT  ID (    COMMA ID  )* -> ^(DELWAYPOINT ID)+
  | QUERRY  ID (    COMMA ID  )* -> ^(DELQUERY ID)+
  ;

crStmt
  : DATATYPE ID (FROM s=STRING)?  lstSyn[(char*)$ID.text->chars] -> ^(CRDATATYPE ID $s) lstSyn
  | FUNCTION ID LPAREN lstArgsFc RPAREN '->' dType (FROM s=STRING)? -> ^(FUNCTION ID $s dType lstArgsFc)
  | OPKEYWORD STRING LPAREN lstArgsFc RPAREN '->' dType (FROM s=STRING)? -> ^(OPDEF STRING $s dType lstArgsFc)
  | RELATION ID LPAREN tpAttList RPAREN -> ^(CRRELATION ID tpAttList)
  | GLA ID (FROM s=STRING)? LPAREN lstArgsFc RPAREN '->' LPAREN lstRetFc RPAREN -> ^(CRGLA ID $s ^(TPATT lstRetFc) ^(TPATT lstArgsFc))
  ;


fctName
  : ID -> FCT[(const char*)$ID.text->chars]
  | STRING -> FCT[(const char*)$STRING.text->chars]
  ;

lstArgsFc
  : /* nothing */
  | dType (     COMMA ! dType)*
  ;

lstRetFc
  : dType ( COMMA ! dType)*
  ;

tpAttList
  : tpAtt (    COMMA ! tpAtt)*
  ;

tpAtt
  : ID dType -> ^(TPATT ID dType)
  ;


/* dType is just ID for now. We should extend in future to ID<other params> */
dType : ID;

lstSyn[char* tp]
  : /* nothing */
  | EQUAL ID (    COMMA ID)* ->^(CRSYNONIM ID[$tp] ID)+
  ;

drStmt :
  ;

writer[bool isNew]
: w=WRITER a=ID LPAREN b=ID RPAREN CONNECTIONS connList SEMICOLON-> ^(WRITER__[$w] $a $b connList)
;

scanner[bool isNew]
    : s=SCANNER ID scannerBody SEMICOLON {
      P_ERR_IF( (!isNew), "Scanner should always be under new statement" );
    } -> ^(SCANNER__[$s] ID scannerBody)
    ;

scannerBody
    : /* nothng */
    | LPAREN ID RPAREN -> ID
    ;

waypoint[bool isNew]
  :    w=WAYPOINT ID {
      wp = (char*)$ID.text->chars;
    if (isNew){
      // should not be in type map
    } else {
      // should be in type map
    }
  } wpDefinition[isNew] { wpType=typeMap[wp]; } wpBody[wpType] -> ^(WAYPOINT__[$w] ID wpDefinition wpBody)
 ;

query[bool isNew]
  :    q=QUERRY rez=(id1=ID {qry=(char*)$id1.text->chars; DP_CheckQuery(isNew,qry) ;} (    COMMA id2=ID {DP_CheckQuery(isNew, (char*)$id2.text->chars);} )* ) SEMICOLON -> ^(QUERRY__[$q] $rez )
    | QUERRY id1=ID {qry=(char*)$id1.text->chars; DP_CheckQuery(isNew,qry) ;} queryBody
    -> ^(QUERRY__ $id1 queryBody)
    ;

wpBody [WaypointType which]     :    '{'! bypass? wpbodyStatement[which]* '}'!
    |    SEMICOLON!
    ;

queryBody
    :    '{'! qBodyStatement* '}'!
    ;

wpbodyStatement [WaypointType  which]
    : q=QUERRY ID {qry=(char*)$ID.text->chars;/*DP_CheckQuery(false,qry);*/} '{' bodyStatement[which] '}' -> ^(QUERRY__[$q] ID bodyStatement)
    ;

qBodyStatement
    :    w=WAYPOINT ID {
            wp = (const char*)$ID.text->chars;
            wpType=typeMap[wp];
        } '{' bodyStatement[wpType] '}'
    -> ^(WAYPOINT__[$w] ID bodyStatement)
    ;

bodyStatement [WaypointType which]
    :    {which==SelectionWaypoint}? selectBody
    |     {which==AggregateWaypoint}? aggregateBody
    |    {which==PrintWaypoint}? printBody
    |    {which==JoinWaypoint}? joinBody
    |    {which==GLAWaypoint}? glaBody
  |  /* default*/ { P_ERR_IF(true, "Invalid type of waypoint. Probably the waypoint \%s is not defined", wp.c_str()); }
    ;

selectBody
    :    selectRules*
    ;

selectRules
    :    FILTER^ logical_or_expression SEMICOLON!
    |    SYNTHESIZE^ ID COLON! ID EQUAL! expression SEMICOLON!
    ;


aggregateBody
    :    aggregateRules*
    ;

aggregateRules
    :    AGGREGATE^ ID EQUAL! ID LPAREN! expression RPAREN! SEMICOLON! {
      // add ID to the list of synthesized attributes of qry
    }
    ;

printBody
    :    printRules*
    ;

printRules
    :    PRINT expression (     COMMA expression)*  SEMICOLON -> ^(PRINT expression)+
    ;

joinBody
    :    joinRules*
    ;

joinRules
    :    JOIN attributeEList SEMICOLON -> ^(JOIN ^(ATTS attributeEList))
    ;

glaBody
    :    glaRules*
    ;

glaRules
    :    GLA (PLUS)? LPAREN attListWTypes RPAREN EQUAL ID (LSQ ctAttList RSQ)? LPAREN expressionList  RPAREN  SEMICOLON  -> ^(GLA (PLUS)? ctAttList ID attListWTypes expressionList )
    ;

bypass     :    BYPASS ID (     COMMA ID )*  SEMICOLON -> ^(BYPASS ID)+
    ;

queryList
    :    idList
    ;

wpDefinition [bool isNew]
    :    {isNew==false}? /* nothing */ -> /*no tree */
    |    {isNew==true}? EQUAL! ( selectWP | joinWP | aggregateWP | printWP | textloaderWP | glaWP)
    ;

selectWP
    :    s=SELECT {
            typeMap[wp]=SelectionWaypoint;
        } CONNECTIONS connList -> ^(SELECT__[$s] connList)
    ;

joinWP
    : JOIN {
            typeMap[wp]=JoinWaypoint;
        }LPAREN attributeList RPAREN CONNECTIONS connList -> ^(JOIN ^(ATTS attributeList) connList)
    ;

aggregateWP
    :    AGGREGATE^ {
      typeMap[wp]=AggregateWaypoint;
    } CONNECTIONS! connList
  ;

printWP    :    PRINT^ {
            typeMap[wp]=PrintWaypoint;
        }CONNECTIONS! connList
  ;

glaWP    :    GLA^ {
            typeMap[wp]=GLAWaypoint;
        } PLUS? CONNECTIONS! connList
  ;

connList
    :    wayPointCN (     COMMA! wayPointCN )*
    ;

textloaderWP :
    TEXTLOADER^ {typeMap[wp]=TextLoaderWaypoint;} '{'! textloaderBody+ '}'!
    ;

textloaderBody :
    ATTRIBUTES FROM ID SEMICOLON -> ^(ATTFROM ID)
    | SEPARATOR STRING SEMICOLON -> ^(SEPARATOR STRING)
    | FILE STRING INT SEMICOLON -> ^(FILE__ STRING INT)
    ;

wayPointCN
    :    t=termConnOpt ID {
          string nWP = (char*)$ID.text->chars;
          // check that ID is valid
          WaypointType wType = typeMap[nWP];
        } -> $t ID
    ;
termConnOpt
    : /* nothing */ ->
    | QMARK -> TERMCONN
    ;
