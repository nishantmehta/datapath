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
 #include <utility>

 #include <antlr3.h>
 #include "DescLexer.h"
 #include "DescParser.h"

/* Debugging */
#undef PREPORTERROR
#define PREPORTERROR assert(1==2)

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
#define ADD_INCLUDE(defs, file) if( waypointIncludes[wp].find(file) == waypointIncludes[wp].end() ) { \
    (defs) += ("m4_include(</" + (file) + "/>)\n"); \
    waypointIncludes[wp].insert(file); \
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
static map<WayPointID, set<string> > waypointIncludes;

// Keeps track of GLAs that return states and their types
static map<QueryID, map<WayPointID, string> > glaStateType;

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

/*
 *  Processes a template by generating an m4 file with the required includes,
 *  definitions, and macro call. Then calls a script to run m4 on the generated
 *  file to create the c++ header file for it, and also generate the description
 *  file that is parsed to log the new object in the system.
 *
 *  macroName:  The name of the macro that has to be called to generate the object.
 *  objName:    The name of the object being generated (first argument to macro)
 *  defs:       Any definitions that are needed before the macro is called.
 *  args:       Any additional arguments to the macro call.
 */
void ProcessTemplate( string macroName, string objName, string defs, string file, string args ) {
     string m4File = "Generated/" + objName + ".m4";
     string descFile = "Generated/" + objName + ".desc";

     // Create temporary file.
     ofstream outfile (m4File.c_str());

     // Add necessary includes
     outfile << "include(Resources-T.m4)dnl" << endl;
     outfile << "m4_include(GLA-templates.m4)dnl" << endl;
     outfile << endl;
     outfile << defs << endl;

     // form the template instantiation code and change name to temp
     outfile << endl << "m4_include(</" << file << "/>)" << endl;
     outfile << macroName << "(" << objName << args << ")" << endl;

    // close the temporary file
     outfile.close();

     // Run M4 on the temporary file
     string call = "./processTemp.sh " + objName;
     int sysret = execute_command(call.c_str());
     FATALIF(sysret != 0, "Failed to instantiate template \%s", macroName.c_str());

     parseDescFile(descFile);
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
        waypointIncludes[wp].clear();
      }
  | ^(TYPEDEF_GT name=ID gt=gtDef) {
        string gtName = $gt.name;
        dTM.AddSynonymType(gtName, STR($name));
        waypointIncludes[wp].clear();
  }
  | ^(TYPEDEF_GF name=ID gf=gtDef) {
        string gfName = $gf.name;
        dTM.AddSynonymType(gfName, STR($name));
        waypointIncludes[wp].clear();
  }
  | ^(TYPEDEF_GIST name=ID gist=gistDef) {
        string gistName = $gist.name;
        dTM.AddSynonymType(gistName, STR($name));
        waypointIncludes[wp].clear();
  }
  | ^(FUNCTION ID (s=STRING) dType lstArgsFc){ dTM.AddFunctions(STR($ID), $lstArgsFc.vecT, $dType.type, STRS($s), true); }
  | ^(OPDEF n=STRING (s=STRING) dType lstArgsFc){ dTM.AddFunctions(STRS($n), $lstArgsFc.vecT, $dType.type, STRS($s), true); }
  | ^(CRGLA ID (s=STRING) ^(TPATT (ret=lstArgsGLA)) ^(TPATT (args=lstArgsGLA))) { dTM.AddGLA(STR($ID), $args.vecT, $ret.vecT, STRS($s) ); }
  | ^(CRGT ID (s=STRING) ^(TPATT (ret=lstArgsGLA)) ^(TPATT (args=lstArgsGLA))) { dTM.AddGT(STR($ID), $args.vecT, $ret.vecT, STRS($s) ); }
  | ^(CRGF ID (s=STRING) ^(TPATT (args=lstArgsGLA))) { dTM.AddGF(STR($ID), $args.vecT, STRS($s) ); }
  | ^(CRGIST ID (s=STRING) states=reqStateList ^(TPATT ret=lstArgsGLA) )
    { dTM.AddGIST( STR($ID), $states.vect, $ret.vecT, STRS($s)); }
  | ^(CR_TMPL_FUNC name=ID file=STRING ) { dTM.AddFunctionTemplate( STR($name), STRS($file));}
  | ^(CR_TMPL_GLA name=ID file=STRING) { dTM.AddGLATemplate( STR($name), STRS($file)); }
  | ^(CR_TMPL_GT name=ID file=STRING) { dTM.AddGTTemplate( STR($name), STRS($file)); }
  | ^(CR_TMPL_GF name=ID file=STRING) { dTM.AddGFTemplate( STR($name), STRS($file)); }
  | ^(CR_TMPL_GIST name=ID file=STRING) { dTM.AddGISTTemplate( STR($name), STRS($file)); }
  | relationCR
  | FLUSHTOKEN {/*dTM.Save();*/ catalog.SaveCatalog();}
  | runStmt
  | QUITTOKEN { exit(0); }
  ;

reqStateList returns [vector<string> vect]
    : ^(STATE_LIST (t=dType {$vect.push_back($t.type);})+ )
    | /* nothing */
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
                FATALIF(!dTM.IsType( type ), "Attempting to read relation \%s containing unknown type \%s, please ensure all required libraries are loaded.", rName.c_str(), type.c_str());
            } END_FOREACH;

            WayPointID scanner(sName);
            lT->AddScannerWP(scanner, rName, attribs);
        }
    ;

waypoint[bool isNew]
    @after { wp = WayPointID(); }
    :    ^(WAYPOINT__ ID {
            if (isNew){
                /*waypointIncludes.clear();*/
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
  | gtRule
  | gfRule
  | gistRule
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
      vector< vector< string > > header;
      string file;
      string defs; /* definitions needed by expressions */
      string separator = ",";
        }
    : ^(PRINT expr[atts, cstStr, defs] printAtts[header] printFile[file] printSep[separator] )
    {
        string printHeader;

        for( size_t i = 0; i < header.size(); ++i ) {
            for( size_t j = 0; j < header[i].size(); ++ j ) {
                if( j > 0 )
                    printHeader += separator;

                printHeader += header[i][j];
            }

            printHeader += "\n";
        }

        lT->AddPrint(wp, qry, atts, $expr.sExpr, cstStr, printHeader, file, defs, separator);
    }
  ;

printAtts[vector< vector< string > >& header]
    @init{
        int level = -1;
        int nAtts = 0;
        int maxLevel = -1;
    }
    : /*nothing*/
    | (
        ^(ATTC {level=-1;}
            (a=ID
                {
                    ++level;
                    if( level > maxLevel ) {
                        header.push_back(vector<string>(nAtts));
                        maxLevel = level;
                    }

                    header[level].push_back(STR($a));
                }
            )+
            {
                // If we didn't get all the way up to max level this time,
                // pad out the extra levels with empty strings.
                while( level < maxLevel ) {
                    ++level;
                    header[level].push_back(string());
                }
            }
        ) {++nAtts;}
      )+
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

printSep[string& s]
    : /* nothing */
    | ^(SEPARATOR sep=STRING)
    {
        s = STRS($sep);
    }
    ;

/* accumulate arguments form GLA definitions and form m4 code to call it */
glaDef returns [string name, string defs]
  : ^(GLA_DEF ID {$name=(const char*) $ID.text->chars;} glaTemplate[$name,$defs])
  ;

gtDef returns [string name, string defs]
  : ^(GT_DEF ID {$name = TXT($ID); } gtTemplate[$name, $defs])
  ;

gfDef returns [string name, string defs]
    : ^(GF_DEF ID {$name = TXT($ID); } gfTemplate[$name, $defs])
    ;

gistDef returns [string name, string defs]
    : ^(GIST_DEF ID {$name = STR($ID); } gistTemplate[$name, $defs])
    ;

stateDef[string& defs, string& args]
    : gla=glaDef {
        defs += $gla.defs;
        args += $gla.name;
    }
    | gist=gistDef {
        defs += $gist.defs;
        args += $gist.name;
    }
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

        ProcessTemplate( name, tempName, defs, file, args );

        name=tempName; // new name
        defs.clear();
        ADD_INCLUDE(defs, tempName + ".h");
    }
    ;

glaTemplArg[string& args, string& defs]
    : ^(LIST {args+="</";} attC[args] ({args+=",";} attC[args])* {args+="/>";})
    | attC[args] /* single typed argument */
    | stateDef[defs, args]
    | s=STRING { args+=TXTN($s); }
    | i=INT { args+=TXT($i); }
    | f=FLOAT { args+=TXT($f); }
    ;

gtTemplate[string& name, string& defs]
@init { string args;

    }
    : /* simpleGT */
    {
        string file;
        FATALIF( !dTM.GTExists( name, file ), "No GT named \%s known to the system!\n", name.c_str());

        ADD_INCLUDE(defs, file);
    }
    | ^(GTTEMPLATE  ({args+=",";} gtTemplArg[args, defs] )* )
    {
        string file;
        if( !dTM.IsGTTemplate(name, file) ) {
            FATAL("No GT Template called \%s known.", name.c_str());
        }

        string tmp = "GT_\%d_" + name;
        string tempName = GenerateTemp(tmp.c_str());

        ProcessTemplate( name, tempName, defs, file, args );

        name=tempName; // new name
        defs.clear();
        ADD_INCLUDE(defs, tempName + ".h");
    }
    ;

gtTemplArg[string& args, string& defs]
    : ^(LIST {args+="</";} attC[args] ({args+=",";} attC[args])* {args+="/>";})
    | attC[args] /* single typed argument */
    | stateDef[defs, args]
    | s=STRING { args+=TXTN($s); }
    | i=INT { args+=TXT($i); }
    | f=FLOAT { args+=TXT($f); }
    ;

gfTemplate[string& name, string& defs]
@init { string args;

    }
    : /* simpleGF */
    {
        string file;
        FATALIF( !dTM.GFExists( name, file ), "No GF named \%s known to the system!\n", name.c_str());

        ADD_INCLUDE(defs, file);
    }
    | ^(GFTEMPLATE  ({args+=",";} gfTemplArg[args, defs] )* )
    {
        string file;
        if( !dTM.IsGFTemplate(name, file) ) {
            FATAL("No GF Template called \%s known.", name.c_str());
        }

        string tmp = "GF_\%d_" + name;
        string tempName = GenerateTemp(tmp.c_str());

        ProcessTemplate( name, tempName, defs, file, args );

        name=tempName; // new name
        defs.clear();
        ADD_INCLUDE(defs, tempName + ".h");
    }
    ;

gfTemplArg[string& args, string& defs]
    : ^(LIST {args+="</";} attC[args] ({args+=",";} attC[args])* {args+="/>";})
    | attC[args] /* single typed argument */
    | stateDef[defs, args]
    | gfDef {
        defs += $gfDef.defs;
        args += $gfDef.name;
    }
    | s=STRING { args+=TXTN($s); }
    | i=INT { args+=TXT($i); }
    | f=FLOAT { args+=TXT($f); }
    ;

gistTemplate[string& name, string& defs]
@init { string args;

    }
    : /* simple GIST */
    {
        string file;
        FATALIF( !dTM.GISTExists( name, file ), "No GIST named \%s known to the system!\n", name.c_str());

        ADD_INCLUDE(defs, file);
    }
    | ^(GISTTEMPLATE  ({args+=",";} gistTemplArg[args, defs] )* )
    {
        string file;
        if( !dTM.IsGISTTemplate(name, file) ) {
            FATAL("No GIST Template called \%s known.", name.c_str());
        }

        string tmp = "GIST_\%d_" + name;
        string tempName = GenerateTemp(tmp.c_str());

        ProcessTemplate( name, tempName, defs, file, args );

        name=tempName; // new name
        defs.clear();
        ADD_INCLUDE(defs, tempName + ".h");
    }
    ;

gistTemplArg[string& args, string& defs]
    : ^(LIST {args+="</";} attC[args] ({args+=",";} attC[args])* {args+="/>";})
    | attC[args] /* single typed argument */
    | stateDef[defs, args]
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

            ProcessTemplate( name, tempName, defs, file, args );

            defs.clear();
            $name=tempName; // new name
        }
    ;

funcTemplateArg[string& args, string & defs]
    : ^(LIST {args += "</";} attC[args] ({args += ",";} attC[args])* {args += "/>";})
    | attC[args]
    | glaDef {
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
      string name = STR($att);
      string t = STR($type);
      FATALIF(!dTM.IsType(t), "Attempted to use unknown type \%s", t.c_str());
      string file = dTM.GetTypeFile(t);
      ADD_INCLUDE(defs, file);

      args+="(";
      args+=name;
      args+=",";
      args+=t;
      args+=")";
    }
    ;

attC[string& args]
    : ^(ATTC {args+="(";}  a=ID { args+=TXT($a); }
            ( b=ID { args+=','; args+=TXT($b);} )* 
        {args+=')';})
    ;

stateArgs[string& defs, vector<WayPointID>& stateSources, vector<string>& stateTypes]
    : /* nothing */
    | (TERMCONN s=ID {
            string sourceName = STR($s);
            WayPointID sourceID = WayPointID::GetIdByName(sourceName.c_str());
            if( glaStateType[qry].find(sourceID) != glaStateType[qry].end() ) {
                lT->AddTerminatingEdge(sourceID, wp);

                stateSources.push_back(sourceID);
                stateTypes.push_back(glaStateType[qry][sourceID]);
                glaStateType[qry].erase(sourceID);

            }
            else {
                FATAL("State required from waypoint \%s, but that waypoint not registered as returning a state.", sourceName.c_str());
            }
        }
    )+
    ;

glaRez[SlotContainer& outAtts, vector<string>& outTypes, string& defs, string& glaName] returns [bool retState]
@init{ $retState = false; }
    : attLWT[outAtts, outTypes, defs]*
    | STATE__ {
        outTypes.push_back("STATE");
        $retState = true;

        string attName = glaName + "_state";
        string attType = "STATE";
        SlotID attID = am.AddSynthesizedAttribute(qry, attName, attType);
        outAtts.Append(attID);

        string file = dTM.GetTypeFile(attType);
        ADD_INCLUDE(defs, file);
    }
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
            vector<WayPointID> reqStateSources;
            vector<string> reqStateTypes;
        }
    : ^(GLA__ ctAttList[ctArgs] stateArgs[defs, reqStateSources, reqStateTypes] glaDef res=glaRez[outAtts, outTypes, defs, $glaDef.name] (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )* ) {
        // This is we get in return
            string glaName = $glaDef.name;
            string file = $glaDef.name + ".h";
            vector<string> paramTypes = lInfo.GetListTypes();
            // Check if operator exists
#ifdef ENFORCE_GLA_TYPES
           vector<ArgFormat> actArgs;
           // TODO: Make sure that the gLA also requires the constant states that were given.
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
           // Make sure we have the definitions for the output types
           for( vector<string>::const_iterator it = outTypes.begin(); it != outTypes.end(); ++it ) {
               string file = dTM.GetTypeFile(*it);
               ADD_INCLUDE(defs, file);
           }

           if( $res.retState ) {
                glaStateType[qry][wp] = glaName;
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

           lT->AddGLA(wp,qry, outAtts, $glaDef.name, defs, ctArgs, atts, sExpr, cstStr, reqStateSources, $res.retState);
    }
  ;

gtRule
    @init {
            SlotContainer atts; /* the set of attributes */
            SlotContainer outAtts; /**output attributes */
            string cstStr; /* the constants used in the expression */
            string sExpr; // the entire expression representing the arguments
            ExprListInfo lInfo;
            string ctArgs="("; /* constructor arguments*/
            std::vector<std::string> outTypes;
            string defs; /* the definitions needed by the expressions */
            vector<WayPointID> reqStateSources;
            vector<string> reqStateTypes;
        }
    : ^(GT__ ctAttList[ctArgs] stateArgs[defs, reqStateSources, reqStateTypes] gtDef res=attLWT[outAtts, outTypes, defs]* (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )* ) {
        // This is we get in return
            string gtName = $gtDef.name;
            string file = $gtDef.name + ".h";
            vector<string> paramTypes = lInfo.GetListTypes();

            // Check if operator exists
#ifdef ENFORCE_GLA_TYPES
           vector<ArgFormat> actArgs;
           if (!dTM.IsGT(gtName, paramTypes, outTypes, file, actArgs)) {
               printf("\nERROR: GT \%s with arguments \%s do not exist",
                      gtName.c_str(), lInfo.GetTypesDesc().c_str());
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
           // Make sure we have the definitions for the output types
           for( vector<string>::const_iterator it = outTypes.begin(); it != outTypes.end(); ++it ) {
               string file = dTM.GetTypeFile(*it);
               ADD_INCLUDE(defs, file);
           }
#else
           lInfo.Prepare( cstStr );
#endif
           bool isCT = lInfo.IsListConstant();

           std::vector<string> eVals = lInfo.Generate();
           for (int i=0; i<eVals.size(); i++){
               if (i>0) {
                   sExpr += ",";
               }

               sExpr += eVals[i];
           }

           ctArgs+=")";

           defs += $gtDef.defs;

           lT->AddGT(wp,qry, outAtts, $gtDef.name, defs, ctArgs, atts, sExpr, cstStr, reqStateSources);
    }
  ;

gfRule
    @init {
            SlotContainer atts; /* the set of attributes */
            string cstStr; /* the constants used in the expression */
            string sExpr; // the entire expression representing the arguments
            ExprListInfo lInfo;
            string ctArgs="("; /* constructor arguments*/
            string defs; /* the definitions needed by the expressions */
            vector<WayPointID> reqStateSources;
            vector<string> reqStateTypes;
        }
    : ^(GF__ ctAttList[ctArgs] stateArgs[defs, reqStateSources, reqStateTypes] gfDef (a=expression[atts, cstStr, defs] {   lInfo.Add($a.sExpr, $a.type, $a.isCT); } )* ) {
        // This is we get in return
            string gfName = $gfDef.name;
            string file = $gfDef.name + ".h";
            vector<string> paramTypes = lInfo.GetListTypes();

            // Check if operator exists
#ifdef ENFORCE_GLA_TYPES
           vector<ArgFormat> actArgs;
           if (!dTM.IsGF(gfName, paramTypes, file, actArgs)) {
               printf("\nERROR: GT \%s with arguments \%s do not exist",
                      gfName.c_str(), lInfo.GetTypesDesc().c_str());
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

           sExpr = "(";
           std::vector<string> eVals = lInfo.Generate();
           for (int i=0; i<eVals.size(); i++){
               if (i>0) {
                   sExpr += ",";
               }

               sExpr += eVals[i];
           }
           sExpr += ")";

           ctArgs+=")";

           defs += $gfDef.defs;

           lT->AddFilter(wp, qry, atts, sExpr, cstStr, defs, $gfDef.name, ctArgs, reqStateSources);
    }
  ;

gistRule
    @init {
            SlotContainer outAtts; /**output attributes */
            string ctArgs="("; /* constructor arguments*/
            std::vector<std::string> outTypes;
            string defs; /* the definitions needed by the expressions */
            vector<WayPointID> reqStateSources;
            vector<string> reqStateTypes;
        }
    : ^(GIST__ ctAttList[ctArgs] stateArgs[defs, reqStateSources, reqStateTypes] gistDef res=glaRez[outAtts, outTypes, defs, $gistDef.name] ) {
        // This is we get in return
            string gistName = $gistDef.name;
            string file = $gistDef.name + ".h";
#ifdef ENFORCE_GLA_TYPES
           if (!dTM.IsGIST(gistName, reqStateTypes, outTypes, file)) {
               printf("\nERROR: GIST \%s does not exist",
                      gistName.c_str());
           }
           // Make sure we have the necessary includes for any required states
           for( vector<string>::const_iterator it = reqStateTypes.begin(); it != reqStateTypes.end(); ++it ) {
               string file = dTM.GetTypeFile(*it);
               ADD_INCLUDE(defs, file);
           }
           // Make sure we have the definitions for the output types
           for( vector<string>::const_iterator it = outTypes.begin(); it != outTypes.end(); ++it ) {
               string file = dTM.GetTypeFile(*it);
               ADD_INCLUDE(defs, file);
           }

           if( $res.retState ) {
                glaStateType[qry][wp] = gistName;
           }
#endif

           ctArgs+=")";

           defs += $gistDef.defs;

           lT->AddGIST(wp,qry, outAtts, $gistDef.name, defs, ctArgs, reqStateSources, $res.retState);
    }
  ;

attLWT [SlotContainer& outAtts, vector<string> &outTypes, string& defs]
    : ^(ATTWT att=ID type=ID) {
            string name = STR($att);
            string t = STR($type);
            FATALIF(!dTM.IsType( t ), "Attempted to use unknown type \%s", t.c_str());
            string file = dTM.GetTypeFile( t );
            ADD_INCLUDE(defs, file);

            SlotID glaID = am.AddSynthesizedAttribute(qry, name, t);

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
    : ^(GLA__ {
        lT->AddGLAWP(wp);
    }
      connList )
  ;

gtWP
    : ^(GT__ {
        lT->AddGTWP(wp);
    }
      connList )
  ;

gistWP
    : GIST_WP {
        lT->AddGISTWP(wp);
        lT->AddEdgeFromBottom(wp);
    }
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
@init {
    SlotContainer attribs;
    char sep;
    string tablePattern;
    int count = 0;
    string defs;
}
@after {
    lT->AddTextLoaderWP(wp, attribs, sep, attribs, tablePattern, count, defs);
}
  :
    ^(TEXTLOADER__ (
        ^(ATTFROM ID) {
            am.GetAttributesSlots(TXT($ID), attribs);

            FOREACH_TWL(sID, attribs) {
                string name = am.GetAttributeName( sID );
                string type = am.GetAttributeType( name );
                FATALIF( !dTM.IsType( type ), "Attempting to read file \%s containing type \%s, please ensure all required libraries are loaded.", tablePattern.c_str(), type.c_str() );
                string file = dTM.GetTypeFile( type );
                ADD_INCLUDE(defs, file);
            } END_FOREACH;
        }
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
    | gtWP
    | gistWP
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
        ADD_INCLUDE(defs, file);

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
        ADD_INCLUDE(defs, file);

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
