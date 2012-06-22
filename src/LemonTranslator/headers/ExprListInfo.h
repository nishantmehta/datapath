//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef _EXPR_LIST_INFO_
#define _EXPR_LIST_INFO_

#include <sstream>
#include "ArgFormat.h"

#ifdef DTM_DEBUG
#include <iostream>
#endif

/* Class to provide an interface for parsing expressions

*/

class ExprListInfo {
    static int cVarCNT; // counter for constants
    // used to generate constants of the form ct3

    struct Element {
        string sExpr; // the expression
        string type; // the normalized type
        bool isCT; // is the expression constant?

        Element(string _sExpr, string _type, bool _isCT):
            sExpr(_sExpr), type(_type), isCT(_isCT){}
    };

    std::vector<Element> list;
    std::vector<string> listTypes;
    bool isAllCT; // are all expressions constant?
    bool isAnyCT; // is any of the expr constant?

public:
    ExprListInfo(){ isAllCT=true; }

    // get the next constant number
    static int NextVar(void){ return cVarCNT++; }

    void Add(string sExpr, string type, bool isCT){
        list.push_back(Element(sExpr, type, isCT));
        listTypes.push_back(type);
        isAllCT = isAllCT & isCT;
        isAnyCT = isAnyCT | isCT;
    }

    bool IsListConstant(void){ return isAllCT; }
    bool IsAnyConstant(void){ return isAnyCT; }

    // returns a ',' separated list of types
    string GetTypesDesc(void){
        string rez;

        if( listTypes.size() > 0 )
            rez += listTypes[0];

        for( int i = 1; i < listTypes.size(); ++i ) {
            rez += (", " + listTypes[i]);
        }

        return rez;
    }

    std::vector<string>& GetListTypes(void){
        return listTypes;
    }

    // function to print an expression in a vector of strings
    // each element coresponds to the printout for each expression
    // Note that constants are not extracted in this method. Prepare() must
    // be called beforehand for constant extraction, as this relies on
    // information from the data type manager.
    std::vector<string> Generate( ){
        std::vector<string> rez;
        for (int i=0; i<list.size(); i++){
            string lRez;

            lRez = list[i].sExpr;

            rez.push_back(lRez);
        }

        return rez;
    }

    /**
      Prepares the expression for printout generation.
      This method modifies the types and expressions of each element as
      necessary, using the information about the actual types and necessary
      conversions of the arguments given by the data type manager.

      During this process, constants will be pulled out. Being overly-greedy
      about pulling out constants shouldn't be too much of an issue,
      the compiler should get rid of any unnecessary variables.
      */
    void Prepare( string& cstStr, std::vector<ArgFormat> args = std::vector<ArgFormat>() ) {
        for( size_t i = 0; i < list.size(); ++i ) {

            if( i < args.size() ) {
                list[i].type = args[i].getType();
                list[i].sExpr = args[i].format(list[i].sExpr);
            }

            // This argument is constant. Create a new variable declaration in cstStr,
            // and replace the argument expression with the name of the new constant.
            if( list[i].isCT && ( i >= args.size() || !args[i].forceNoExtract() ) ) {
                ostringstream ctName;
                ctName << "ct" << NextVar();
                ostringstream gen;
                gen << list[i].type << " " << ctName.str() << "("
                    << list[i].sExpr << ");\n";

                cstStr += "    " + gen.str();
                list[i].sExpr = ctName.str();
            }
        }
    }
};

#endif // _EXPR_LIST_INFO_
