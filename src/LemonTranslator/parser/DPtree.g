// this is the tree grammar that postprocesses DataPath grammar
tree grammar DPtree;

options {
  language = C;
  tokenVocab = BaseLexer;
  ASTLabelType    = pANTLR3_BASE_TREE;
}

@header {
 #include "SymbolicWaypointConfig.h"
 #include "LemonTranslator.h"
 #include "AttributeManager.h"
 #include "QueryManager.h"
 #include "DataTypeManager.h"
 #include "ExprListInfo.h"
 #include "Catalog.h"
 #include "ExternalCommands.h"
 #include "Errors.h"
 #include <iostream>
 #include <fstream>
 #include <map>
 #include <vector>
 #include <set>

 #include <antlr3.h>
 #include "DescLexer.h"
 #include "DescParser.h"

/* Debugging */
#undef PREPORTERROR
#define PREPORTERROR assert(1=2)

// uncomment this to enforce types
#define ENFORCE_TYPES
#define ENFORCE_GLA_TYPES

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
#ifndef ADD_CST
#define ADD_CST(cstStr, cst) ((cstStr) += ("    " + (cst)))
#endif
#ifndef ADD_INCLUDE
#define ADD_INCLUDE(defs, file) if( waypointIncludes.find(file) == waypointIncludes.end() ) { \
    (defs) += ("m4_include(</" + (file) + "/>)\n"); \
    waypointIncludes.insert(file); \
    }
#endif
}

@members {
static LemonTranslator* lT = NULL; // this is a pointe to the lemon translator used
 //set by parse to be used by all
static WayPointID wp; // this is the currect waypoint. If null, illegal to have a waypoint
 // related statement
static WaypointType wpType = InvalidWaypoint; //type of above waypoint
static QueryID qry; // current query. Empty means illegal ...

static bool isNew = false; // is this a new anything.

 // attribute manager that does the translation from attributes to SlotID
 // when it starts it will define the attributes of all relations
static AttributeManager& am = AttributeManager::GetAttributeManager();

 // get it ready for adding queries
static QueryManager& qm = QueryManager::GetQueryManager();

 // get the datatype manager
static DataTypeManager& dTM = DataTypeManager::GetDataTypeManager();

// the catalog
static Catalog& catalog = Catalog::GetCatalog();

// Set of includes for the current waypoint, used to reduce redundant includes.
static set<string> waypointIncludes;

extern int tempCounter; // id for temporary variables}
string StripQuotes(string str);
string NormalizeQuotes(string str);
string GenerateTemp(const char* pattern);

void parseDescFile( string path ) {
    pANTLR3_INPUT_STREAM input;
    input = antlr3FileStreamNew((pANTLR3_UINT8) path.c_str(), ANTLR3_ENC_8BIT);

    pDescLexer lexer;
    pANTLR3_COMMON_TOKEN_STREAM tstream;
    pDescParser parser;

    lexer = DescLexerNew(input);
    FATALIF(lexer == NULL, "Failed to instantiate description file lexer for \%s!\n", path.c_str());

    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    FATALIF(tstream == NULL, "Failed to instantiate token stream for description file \%s!\n", path.c_str());

    parser = DescParserNew(tstream);
    FATALIF(parser == NULL, "Failed to instantiate description file parser for \%s!\n", path.c_str());

    parser ->parse(parser);

    bool errors=false;
    if( parser->pParser->rec->state->errorCount > 0 ) {
        fprintf(stderr, "The description file parser returned \%d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount );
        errors = true;
    }

    parser ->free(parser);
    parser = NULL;
    tstream ->free(tstream);
    tstream = NULL;
    lexer ->free(lexer);
    lexer = NULL;

    FATALIF(errors, "Failed to parse description file \%s\n", path.c_str());
}

}

parse[LemonTranslator* trans] : {
      lT = trans;
    } complexStatement+ ;

complexStatement
  : ^(NEWSTATEMENT statement[true])
  | ^(OLDSTATEMENT statement[false])
  | ^(DELWAYPOINT ID) /* nothing for now, add */
  | ^(DELQUERY ID) { QueryID q=qm.GetQueryID(TXT($ID)); lT->DeleteQuery(q); }
  | ^(CRDATATYPE ID s=STRING) { dTM.AddBaseType(STR($ID), STRS($s)); }
  | ^(CRSYNONYM tp=ID s=ID) { dTM.AddSynonymType(STR($tp), STR($s)); }
  | ^(TYPEDEF_GLA name=ID g=glaDef) {
        string glaName = $g.name;
        dTM.AddSynonymType(glaName, STR($name));
      }
  | ^(FUNCTION ID (s=STRING) dType lstArgsFc){ dTM.AddFunctions(STR($ID), $lstArgsFc.vecT, $dType.type, STRS($s), true); }
  | ^(OPDEF n=STRING (s=STRING) dType lstArgsFc){ dTM.AddFunctions(STRS($n), $lstArgsFc.vecT, $dType.type, STRS($s), true); }
  | ^(CRGLA ID (s=STRING) ^(TPATT (ret=lstArgsGLA)) ^(TPATT (args=lstArgsGLA))) { dTM.AddGLA(STR($ID), $args.vecT, $ret.vecT, STRS($s) ); }
  | ^(CR_TMPL_FUNC name=ID file=STRING ) { dTM.AddFunctionTemplate( STR($name), STRS($file));}
  | ^(CR_TMPL_GLA name=ID file=STRING) { dTM.AddGLATemplate( STR($name), STRS($file)); }
  | relationCR
  | FLUSHTOKEN {/*dTM.Save();*/ catalog.SaveCatalog();}
  | runStmt
  | QUITTOKEN { exit(0); }
  ;

runStmt
@init{QueryIDSet qrys;}
@after{lT->Run(qrys);}
  : ^(RUN__ (ID{
        QueryID lq = qm.GetQueryID(TXT($ID));
        // check lq to ensur it is valid
        qrys.Union(lq);
        })+)
  ;

relationCR
@init { Schema newSch; int index = 0; }
@after { catalog.AddSchema(newSch);/* register the relatin with catalog */ }
  : ^(CRRELATION x=ID {newSch.SetRelationName(TXT($x));/* set relation name */}
      ( ^(TPATT n=ID t=ID)
        {
            Attribute att;
            att.SetName(TXT($n));

            string ty(string(TXT($t)));
            FATALIF(!dTM.IsType(ty), "Attempting to create relation with attribute of unknown type \%s, please ensure that all required libraries are loaded.", ty.c_str());

            att.SetType(ty);
            att.SetIndex(++index);
            newSch.AddAttribute(att);/* add attribte n with type t */ }
      )+ )
  ;

lstArgsFc returns [vector<string> vecT]
  :( dType { vecT.push_back($dType.type); } )*
  ;

lstArgsGLA returns [vector<string> vecT]
    : f=dType {vecT.push_back($f.type);} ( o=dType {vecT.push_back($o.type); } )*
    ;

dType returns[string type]
  : ID {$type=TXT($ID);}
  ;

statement[bool isNew]
    :    scanner
    | waypoint[isNew]
    | query
    | writer
    ;

writer :
    ^(WRITER__ a=ID b=ID {
        // set wp to current scanner
        wp = WayPointID::GetIdByName((const char*)$a.text->chars);
        // set the query
        SlotContainer attribs;
        string relName = STR($a);
        am.GetAttributesSlots(relName, attribs); // put attributes in attribs


        // Ensure that all the types are known by the system
        FOREACH_TWL(sID, attribs) {
            string name = am.GetAttributeName( sID );
            string type = am.GetAttributeType( name );
            FATALIF(!dTM.IsType( type ), "Attempting to write relation \%s containing unknown type \%s, please ensure all required libraries are loaded.", relName.c_str(), type.c_str());
        } END_FOREACH;

        WayPointID scanner = WayPointID::GetIdByName(relName.c_str());
        lT->AddScannerWP(scanner, relName, attribs);
        qry = qm.GetQueryID((const char*) $b.text->chars);
        lT->AddWriter(wp, qry);
        // now wp is set for connList
      } connList)
    ;

scanner
    @init { string sName; // scanner name
            string rName; // relatin name
          }
    :    ^(SCANNER__ a=ID { rName=(char*)$a.text->chars; sName=rName;}
            (b=ID {sName=(char*)$b.text->chars;})?) {
            SlotContainer attribs;
            am.GetAttributesSlots(rName, attribs); // put attributes in attribs
            //WayPointID scanner = WayPointID::GetIdByName(sName.c_str());

            // Ensure that all the types are known by the system
            FOREACH_TWL(sID, attribs) {
                string name = am.GetAttributeName( sID );
                string type = am.GetAttributeType( name );
                FATALIF(!dTM.IsType( type ), "Attempting to write relation \%s containing unknown type \%s, please ensure all required libraries are loaded.", rName.c_str(), type.c_str());
            } END_FOREACH;

            WayPointID scanner(sName);
            lT->AddScannerWP(scanner, rName, attribs);
        }
    ;

waypoint[bool isNew]
    :    ^(WAYPOINT__ ID {
            if (isNew){
                waypointIncludes.clear();
                WayPointID nWp((const char*)$ID.text->chars);
                wp = nWp;
            } else {
                wp = WayPointID::GetIdByName((const char*)$ID.text->chars);
            }
        }  wpDefinition? bypassRule* wpbodyStatement* )
    ;

query
  :    ^(QUERRY__ (ID { qry = qm.GetQueryID((const char*) $ID.text->chars); /*qm.AddNewQuery(string((const char*) $ID.text->chars), qry);*/ })+  qBodyStatement* )
  ;


qBodyStatement
    :    ^(WAYPOINT__ ID {
            wp = WayPointID::GetIdByName((char*)$ID.text->chars);
            //wpType=typeMap[wp];
        }  bodyStatement)
    ;

wpbodyStatement
  : ^(QUERRY__ ID {qry = qm.GetQueryID((const char*) $ID.text->chars);} bodyStatement)
  ;

bodyStatement
  :    rules*
  ;

rules :
    filterRule
  | synthRule
  | aggregateRule
  | printRule
  | joinRule
  | glaRule
  ;

filterRule
    @init {SlotContainer atts; /* the set of attributes */
      string cstStr; /* the constants used in the expression */
      string defs; /* definitions needed by expressions */
        }
   : ^(FILTER expr[atts, cstStr, defs]) { lT->AddFilter(wp, qry, atts, $expr.sExpr, cstStr, defs); }
  ;

synthRule
    @init {SlotContainer atts; /* the set of attributes */
      string cstStr; /* the constants used in the expression */
      string defs; /* definitions needed by expressions */
        }
    : ^(SYNTHESIZE__ a=ID t=dType expr[atts, cstStr, defs]) {
        string type = $t.type;
        FATALIF( !dTM.IsType( type ), "Cannot synthesize attribute \%s of unknown type \%t.", TXT($a), type.c_str());
      SlotID sID = am.AddSynthesizedAttribute(qry, (const char*)$a.text->chars, type);
      ADD_INCLUDE(defs, dTM.GetTypeFile( type ) );
      lT->AddSynthesized(wp,qry, sID, atts, $expr.sExpr, cstStr, defs);
    }
  ;

aggregateRule
    @init {SlotContainer atts; /* the set of attributes */
      string cstStr; /* the constants used in the expression */
      string defs; /* definitions needed by expressions */
        }
    : ^(AGGREGATE name=ID type=ID expr[atts, cstStr, defs]) {
            SlotID aggID = am.AddSynthesizedAttribute(qry, (const char*)$name.text->chars, "DOUBLE");
            lT->AddAggregate(wp,qry, aggID, (const char*)$type.text->chars, atts, $expr.sExpr, cstStr, defs);
    }
  ;

printRule
    @init {SlotContainer atts; /* the set of attributes */
      string cstStr; /* the constants used in the expression */
      string names;
      string types;
      string file;
      string defs; /* definitions needed by expressions */
        }
    : ^(PRINT expr[atts, cstStr, defs] printAtts[names, types] printFile[file] ){ lT->AddPrint(wp, qry, atts, $expr.sExpr, cstStr, names, types, file, defs); }
  ;

printAtts[string& names, string& types]
    : /*nothing*/
    | ^(ATTWT n=ID t=ID) { names+=TXT($n); types+=TXT($t); }
      ( ^(ATTWT n=ID t=ID) { names+=","; names+=TXT($n); types+=","; types+=TXT($t); } )*
    ;

ctAttList[string& ctArgs]
    : /*no arg, nothing in result*/
    | a=ctAtt {ctArgs+=$a.s;} (b=ctAtt  {ctArgs+=", "; ctArgs+=$b.s;} )*
    ;

ctAtt returns[string s]
    : (a=STRING | a=INT | a=FLOAT) {s=(char*)($a.text->chars);}
    ;

printFile[string& s]
    : /* nothing */
    | ^(LIST a=STRING ) {s=STRS(a);}
    ;

/* accumulate arguments form GLA definitions and form m4 code to call it */
glaDef returns [string name, string defs]
  : ID {$name=(const char*) $ID.text->chars;} glaTemplate[$name,$defs]
  ;

glaTemplate[string& name, string& defs]
@init { string args;

    }
    : /* simpleGLA */
    {
        string file;
        FATALIF( !dTM.GLAExists( name, file ), "No GLA named \%s known to the system!\n", name.c_str());

        ADD_INCLUDE(defs, file);
    }
    | ^(GLATEMPLATE  ({args+=",";} glaTemplArg[args, defs] )* )
        {
        string file;
        if( !dTM.IsGLATemplate(name, file) ) {
            FATAL("No GLA Template called \%s known.", name.c_str());
        }

         string tmp = "GLA_\%d_" + name;
         string tempName = GenerateTemp(tmp.c_str());
         string m4File = "Generated/" + tempName + ".m4";
         string descFile = "Generated/" + tempName + ".desc";

         // Create temporary file.
         ofstream outfile (m4File.c_str());

         // Add necessary includes
         outfile << "include(Resources-T.m4)dnl" << endl;
         outfile << "m4_include(GLA-templates.m4)dnl" << endl;
         outfile << endl;
         outfile << defs << endl;

         // form the template instantiation code and change name to temp
         outfile << endl << "m4_include(</" << file << "/>)" << endl;
         outfile << name << "(" << tempName << args << ")" << endl;

        // close the temporary file
         outfile.close();

         // Run M4 on the temporary file
         string call = "./processTemp.sh " + tempName;
         int sysret = execute_command(call.c_str());
         FATALIF(sysret != 0, "Failed to instantiate templated GLA \%s", name.c_str());

         parseDescFile(descFile);

         name=tempName; // new name
         defs.clear();
         ADD_INCLUDE(defs, tempName + ".h");
        }
    ;

glaTemplArg[string& args, string& defs]
    : ^(LIST {args+="</";} attC[args] ({args+=",";} attC[args])* {args+="/>";})
    | attC[args] /* single typed argument */
    | GLA glaDef {
      // glue the definitions accumulated
      defs+=$glaDef.defs;
      // add the name to current definition
      args+=$glaDef.name;
    }
    | s=STRING { args+=TXTN($s); }
    | i=INT { args+=TXT($i); }
    | f=FLOAT { args+=TXT($f); }
    ;

funcTemplate[string& fName, string& defs, bool isExternal] returns [string name]
@init{  $name = fName;
        string args;
    }
    : /* nothing */
    | ^(FUNCTEMPLATE ({args += ",";} funcTemplateArg[args, defs] )* )
        {
            string file;

            if( isExternal )
                file = $name + ".h.m4";
            else {
                FATALIF(!dTM.IsFunctionTemplate($name, file),
                    "No function template for \%s found!", fName.c_str());
            }

            string tmp = "FUNC_\%d_" + fName;
            string tempName = GenerateTemp(tmp.c_str());
            string m4File = "Generated/" + tempName + ".m4";
            string descFile = "Generated/" + tempName + ".desc";

            // Create temporary file.
            ofstream outfile (m4File.c_str());

            // Add necessary includes
            outfile << "include(Resources-T.m4)dnl" << endl;
            outfile << "m4_include(GLA-templates.m4)dnl" << endl;
            outfile << endl;
            outfile << defs << endl;

            // form the template instantiation code and change name to temp
            outfile << endl << "m4_include(</" << file << "/>)" << endl;
            outfile << name << "(" << tempName << args << ")" << endl;

            // close the temporary file
            outfile.close();

            // Run M4 on the temporary file
            string call = "./processTemp.sh " + tempName;
            int sysret = execute_command(call.c_str());
            FATALIF(sysret != 0, "Failed to instantiate templated function \%s", fName.c_str());

            parseDescFile(descFile);

            defs.clear();
            $name=tempName; // new name
        }
    ;

funcTemplateArg[string& args, string & defs]
    : ^(LIST {args += "</";} attC[args] ({args += ",";} attC[args])* {args += "/>";})
    | attC[args]
    | GLA glaDef {
      // glue the definitions accumulated
      defs+=$glaDef.defs;
      // add the name to current definition
      args+=$glaDef.name;
    }
    | s=STRING { args += TXTN($s); }
    | i=INT { args += TXT($i); }
    | f=FLOAT { args += TXT($f); }
    ;

funcRetType returns [string type, bool external]
    : /* nothing */ {$external = false;}
    | ^(TYPE_ t=ID) { $type = TXT($t); $external = true; }
    ;

attWT[string& args, string& defs]
    : ^(ATTWT att=ID type=ID) {
      args+="(";
      args+=(const char*) $att.text->chars;
      args+=",";
      args+=(const char*) $type.text->chars;
      args+=")";

      string t = STR($type);
      FATALIF(!dTM.IsType(t), "Attempted to use unknown type \%s", t.c_str());
      string file = dTM.GetTypeFile(t);
      ADD_INCLUDE(defs, file);
    }
    ;

attC[string& args]
    : ^(ATTC {args+="(";}  a=ID { args+=TXT($a); }
            ( b=ID { args+=','; args+=TXT($b);} )* 
        {args+=')';})
    ;

glaRule
    @init {
            SlotContainer atts; /* the set of attributes */
            SlotContainer outAtts; /**output attributes */
            string cstStr; /* the constants used in the expression */
            string sExpr; // the entire expression representing the arguments
            ExprListInfo lInfo;
            string ctArgs="("; /* constructor arguments*/
            std::vector<std::string> outTypes;
            string defs; /* the definitions needed by the expressions */
        }
    : ^(GLA ctAttList[ctArgs] glaDef attLWT[outAtts, outTypes, defs]* (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )* ) {
        // This is we get in return
            string glaName = $glaDef.name;
            string file = $glaDef.name + ".h";
            vector<string> paramTypes = lInfo.GetListTypes();
            // Check if operator exists
#ifdef ENFORCE_GLA_TYPES
           vector<ArgFormat> actArgs;
           if (!dTM.IsGLA(glaName, paramTypes, outTypes, file, actArgs)) {
               printf("\nERROR: GLA \%s with arguments \%s do not exist",
                      glaName.c_str(), lInfo.GetTypesDesc().c_str());
           } else {
               // Need to tell the expression info list about the actual types of the
               // arguments and any special formatting needed using the vector of
               // ArgFormats.
               lInfo.Prepare( cstStr, actArgs );
           }
           // Update the parameter types after the data type manager's information has been
           // considered.
           paramTypes = lInfo.GetListTypes();
           for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
               string file = dTM.GetTypeFile(*it);
               ADD_INCLUDE(defs, file);
           }
#else
           lInfo.Prepare( cstStr );
#endif
           bool isCT = lInfo.IsListConstant();

           std::vector<string> eVals = lInfo.Generate();
           sExpr = "(";
           for (int i=0; i<eVals.size(); i++){
               if (i>0) {
                   sExpr += ",";
               }

               sExpr += eVals[i];
           }
           sExpr += ")";

           ctArgs+=")";

           defs += $glaDef.defs;

            lT->AddGLA(wp,qry, outAtts, $glaDef.name, defs, ctArgs, atts, sExpr, cstStr);
    }
  ;

attLWT [SlotContainer& outAtts, vector<string> &outTypes, string& defs]
    : ^(ATTWT att=ID type=ID) {
            SlotID glaID = am.AddSynthesizedAttribute(qry, (const char*)$att.text->chars,
                                                      (const char*)$type.text->chars);
            string t((const char*)($type.text->chars));

            FATALIF(!dTM.IsType( t ), "Attempted to use unknown type \%s", t.c_str());
            string file = dTM.GetTypeFile( t );
            ADD_INCLUDE(defs, file);

            outTypes.push_back(t);
            outAtts.Append(glaID);
        }
    ;

bypassRule :
    ^(BYPASS ID) {
      qry = qm.GetQueryID((const char*) $ID.text->chars);
      lT->AddBypass(wp,qry);
    }
  ;

joinType returns [LemonTranslator::JoinType type]
: /* none, normal */ {$type = LemonTranslator::Join_EQ;}
| JOIN_IN { $type = LemonTranslator::Join_IN; }
| JOIN_NOTIN { $type = LemonTranslator::Join_NOTIN; }
;

joinRule
    @init
    {
        SlotContainer atts; /* the set of attributes */
        set<string> includeFiles;
    }
    : ^(JOIN joinType attributeList[atts, includeFiles])
    {
        string defs;
        for( set<string>::const_iterator it = includeFiles.begin(); it != includeFiles.end(); ++it ) {
            ADD_INCLUDE(defs, *it);
        }

        lT->AddJoin(wp, qry, atts, $joinType.type, defs);
    }
  ;

attribute returns [SlotID slot, string file]
    :     att=ATT {
            string attName = STR($att);
            $slot = am.GetAttributeSlot(attName);
            FATALIF( !$slot.IsValid(), "Attribute \%s does not exist", attName.c_str());

            string type = am.GetAttributeType(attName);
            FATALIF( !dTM.IsType(type), "Attempting to access Attribute \%s of unknown type \%s" \
                ", ensure all necessary libraries have been included.", \
                attName.c_str(), type.c_str());

            $file = dTM.GetTypeFile( type );
        }
     ;

selectWP
    :    ^(SELECT__ {
            lT->AddSelectionWP(wp);
        }  connList )
    ;

joinWP
@init
    {
        SlotContainer atts;
        set<string> includeFiles;
    }
    : ^(JOIN attributeList[atts, includeFiles]
    {
        string defs;
        for( set<string>::const_iterator it = includeFiles.begin(); it != includeFiles.end(); ++it ) {
            ADD_INCLUDE(defs, *it);
        }

        lT->AddJoinWP(wp, atts, defs);
    } connList)
    ;

attributeList[SlotContainer& atts, set<string>& includeFiles]
: ^(ATTS (a=attribute {$atts.Append($a.slot); includeFiles.insert($a.file);})+ )
;

aggregateWP
  :    ^(AGGREGATE {
        lT->AddAggregateWP(wp);
    }
      connList )
  ;

glaWP
    @init {
        bool isLarge = false;
    }
    : ^(GLA (PLUS {isLarge = true;})? {
        if( isLarge )
            lT->AddGLALargeWP(wp);
        else
            lT->AddGLAWP(wp);
    }
      connList )
  ;

printWP
    :    ^(PRINT {
      lT->AddPrintWP(wp);
      }
      connList )
  ;

connList
    : wayPointCN+
    ;

wayPointCN
    :    ID {
          WayPointID nWP = WayPointID::GetIdByName((char*)$ID.text->chars);
          lT->AddEdge(nWP, wp);
        }
    | TERMCONN ID{
          WayPointID nWP = WayPointID::GetIdByName((char*)$ID.text->chars);
          lT->AddTerminatingEdge(nWP, wp);
        }
    ;

textloaderWP
@init { SlotContainer attribs; char sep; string tablePattern; int count = 0;}
@after { lT->AddTextLoaderWP(wp, attribs, sep, attribs, tablePattern, count); }
  :
    ^(TEXTLOADER__ (
        ^(ATTFROM ID) { am.GetAttributesSlots(TXT($ID), attribs);}
  | ^(SEPARATOR a=STRING) {sep = char(*(StripQuotes(TXT($a)).c_str()));}
  | ^(FILE__ b=STRING { tablePattern = TXTS($b); count=1; } (INT {count= atoi(TXT($INT));})? )
  )+ )
;

wpDefinition
  : selectWP
    | joinWP
    | aggregateWP
    | printWP
    | textloaderWP
    | glaWP
  ;

expr[SlotContainer& atts, string& cstStr, string& defs] returns [string sExpr] :
    (a=expression[atts, cstStr, defs] {sExpr+=$a.sExpr;} )
    (b=expression[atts, cstStr, defs] {sExpr+=", "+$b.sExpr;} ) *
    | ^('?' a=expression[atts, cstStr, defs] {sExpr+=$a.sExpr;} b=expression[atts, cstStr, defs] {sExpr+="?";sExpr+=$b.sExpr;} c=expression[atts, cstStr, defs] {sExpr+=":";sExpr+=$c.sExpr;})
    ;

// There could be more than one consts, hence we must have cstArray or something alike
expression[SlotContainer& atts, string& cstStr, string& defs] returns [string sExpr, string type, bool isCT]
@init { ExprListInfo lInfo; string rType; string fName; }
  :
    ^(OPERATOR a=expression[atts, cstStr, defs] { lInfo.Add($a.sExpr, $a.type, $a.isCT); }
      b=expression[atts, cstStr, defs] { lInfo.Add($b.sExpr, $b.type, $b.isCT); } ) // binary
  {
      string funcName((char*)($OPERATOR.text->chars));
      bool funcPure = true;
      string file;
      vector<string> paramTypes = lInfo.GetListTypes();
      // Check if operator exists
#ifdef ENFORCE_TYPES
      vector<ArgFormat> actArgs;
      if (!dTM.IsFunction(funcName, paramTypes, $type, file, funcPure, actArgs)) {
        printf("\nERROR: Operator \%s with arguments \%s do not exist",
               funcName.c_str(), lInfo.GetTypesDesc().c_str());
      }
      else {
          // Need to tell the expression info list about the actual types of the
          // arguments and any special formatting needed using the vector of
          // ArgFormats.
          lInfo.Prepare( $cstStr, actArgs );
      }
      paramTypes = lInfo.GetListTypes();
      for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
            string file = dTM.GetTypeFile(*it);
            ADD_INCLUDE(defs, file);
      }

      ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
#else
      lInfo.Prepare( $cstStr );
#endif

      $isCT = lInfo.IsListConstant() && funcPure;
      if( file != "" )
        ADD_INCLUDE(defs, file);

      std::vector<string> eVals = lInfo.Generate();
      $sExpr = "(";
      $sExpr += eVals[0];
      $sExpr += funcName;
      $sExpr += eVals[1];
      $sExpr += ")";
    }
  | ^(UOPERATOR a=expression[atts, cstStr, defs]) // unary
    {
      lInfo.Add($a.sExpr, $a.type, $a.isCT);

      string funcName((char*)($UOPERATOR.text->chars));
      bool funcPure = true;
      string file;
      vector<string> paramTypes = lInfo.GetListTypes();
      // Check if operator exists
#ifdef ENFORCE_TYPES
      vector<ArgFormat> actArgs;
      if (!dTM.IsFunction(funcName, paramTypes, $type, file, funcPure, actArgs)) {
        printf("\nERROR: Operator \%s with arguments \%s do not exist",
               funcName.c_str(), lInfo.GetTypesDesc().c_str());
      }
      else {
          // Need to tell the expression info list about the actual types of the
          // arguments and any special formatting needed using the vector of
          // ArgFormats.
          lInfo.Prepare( $cstStr, actArgs );
      }
      paramTypes = lInfo.GetListTypes();
      for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
            string file = dTM.GetTypeFile(*it);
            ADD_INCLUDE(defs, file);
      }
      ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
#else
      lInfo.Prepare( $cstStr );
#endif
      $isCT = lInfo.IsListConstant() && funcPure;
      if( file != "" )
          defs += "#include \"" + file + "\"\n";

      std::vector<string> eVals = lInfo.Generate();
      $sExpr = "(";
      $sExpr += funcName.c_str();
      $sExpr += eVals[0];
      $sExpr += ")";
    }

  | ^(FUNCTION i=ID {fName = TXT($i);} rt=funcRetType t=funcTemplate[fName, defs, $rt.external]  (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )*) // Function
    {
      string funcName = $t.name;
      bool funcPure = true;
      string file = fName + ".h";
      vector<string> paramTypes = lInfo.GetListTypes();
      // This is we get in return
      // This is we get in return
      // Check if operator exists
#ifdef ENFORCE_TYPES
      vector<ArgFormat> actArgs;
      if ( !$rt.external && !dTM.IsFunction(funcName, paramTypes, $type, file, funcPure, actArgs)) {
        printf("\nERROR: Operator \%s with arguments \%s do not exist",
               funcName.c_str(), lInfo.GetTypesDesc().c_str());
      }

      // Need to tell the expression info list about the actual types of the
      // arguments and any special formatting needed using the vector of
      // ArgFormats.
      lInfo.Prepare( $cstStr, actArgs );
      paramTypes = lInfo.GetListTypes();
      for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
            string file = dTM.GetTypeFile(*it);
            ADD_INCLUDE(defs, file);
      }
      ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
#else
      lInfo.Prepare( $cstStr );
#endif
      $isCT = lInfo.IsListConstant() && funcPure;
      if( file != "" )
          defs += "#include \"" + file + "\"\n";

      if( $rt.external )
        $type = $rt.type;

      std::vector<string> eVals = lInfo.Generate();
      $sExpr = $t.name;
      $sExpr += "(";
      for (int i=0; i<eVals.size(); i++){
        if (i>0) {
          $sExpr += ",";
        }

        $sExpr += eVals[i];
      }
      $sExpr += ")";
}
| ^(MATCH_DP patt=STRING a=expression[atts, cstStr, defs] { lInfo.Add($a.sExpr, $a.type, $a.isCT); } ) // pattern matcher
    {
        // for a pattern matcher, we have to build an expression
        // of the form: PatternMather ctObj(pattern)
        // then on use do ctObj.IsMatch(expr)
        $type = "bool";
        ADD_INCLUDE(defs, dTM.GetTypeFile($type) );

        $isCT = lInfo.IsListConstant();

        lInfo.Prepare( $cstStr );
        vector<string> paramTypes = lInfo.GetListTypes();
        for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
            string file = dTM.GetTypeFile(*it);
            ADD_INCLUDE(defs, file);
        }
        std::vector<string> eVals = lInfo.Generate();
        // new constant
        int ctNo = ExprListInfo::NextVar();
        // add def of matcher object
        ostringstream match;
        match << "PatternMatcher ct" << ctNo << "( string("
              <<  TXTN($patt) << ") );" << endl;

        ADD_CST($cstStr, match.str());
        // now the expression
        ostringstream expr;
        expr << "ct" << ctNo << ".IsMatch(" << eVals[0] << ")";
        $sExpr+=expr.str();
   }
| ^(CASE_DP (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )*) // cases
   {
        // just like a function. For now we only support the 3-argument case
        $isCT = lInfo.IsListConstant();

        lInfo.Prepare( $cstStr );
        vector<string> paramTypes = lInfo.GetListTypes();
        for( vector<string>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it ) {
            string file = dTM.GetTypeFile(*it);
            ADD_INCLUDE(defs, file);
        }
        std::vector<string> eVals = lInfo.Generate();
        FATALIF(eVals.size()!=3, "We only support CASE(test, true_expr, false_expr) for now");
        $sExpr = $sExpr+"("+eVals[0]+") ? ("+eVals[1]+") : ("+eVals[2]+")";
        // TODO: Make sure all of the action statements return the same type.
        $type = paramTypes[1];
   }
| att=ATT  // Attribute
{
        string longName = (char*)$att.text->chars;
        SlotID slot = am.GetAttributeSlot(longName.c_str());
        FATALIF( !slot.IsValid(), "Attribute does not exist, how did this happen?");
        atts.Append(slot);
        // Add the attribute long name string to the expression
        $sExpr = longName;
        // Find the correct function to get attr type
        $type = am.GetAttributeType(longName.c_str());
        if( !dTM.IsType($type) ) {
            FATAL("Attempting to access attribute \%s of unknown type \%s, "
            "please ensure that all of the required libraries are included.\n",
            longName.c_str(), $type.c_str());
        }
        string file = dTM.GetTypeFile($type);
        ADD_INCLUDE(defs, file );
        $isCT = false;
}
| INT
{
    string sExpr_text((char*)($INT.text->chars));
    $sExpr = sExpr_text;
    if( sExpr_text[sExpr_text.size()-1] == 'L') // Literal long int
        $type = "BIGINT";
    else
        $type = "INT";          // Literal int
    $isCT = true;
    ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
}
| BOOL_T
{
    $sExpr = (char*)($BOOL_T.text->chars);
    $type = "bool";
    $isCT = true;
    ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
}
|    STRING
{
    $sExpr = TXTN($STRING);
    $type = "STRING_LITERAL";
    $isCT = true;
    ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
}
|    FLOAT
{
    string sExpr_text((char*) ($FLOAT.text->chars));
    $sExpr = (char*)($FLOAT.text->chars);
    if( sExpr_text[sExpr_text.size()-1] == 'L' )    // Literal long double
        $type = "DOUBLE";
    else if( sExpr_text[sExpr_text.size() -1] == 'f' )  // Literal float
        $type = "FLOAT";
    else    // Literal double
        $type = "DOUBLE";
    $isCT = true;
    ADD_INCLUDE(defs, dTM.GetTypeFile($type) );
}
;
