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
#ifndef _DATA_TYPE_MANAGER_
#define _DATA_TYPE_MANAGER_

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdio>

#include "ArgFormat.h"

using namespace std;

/** Class to facilitate type management.

    Warning: it is not thread safe so make sure only one component
    (like the parser) uses it.


    Types reffered to in relations schema or AttributeManager have to
    exist here. The parser checks that this is the case.

    Make sure only one of these can be created in the system.

*/

enum Associativity {
    LeftToRight,
    RightToLeft,
    NoAssoc
};

// This is used to store type information
/*
   This maintains only base type information and all synonyms will point to this base type
   It keeps what all types it can be converted to, synonyms are only convertible to each
   other if their base types are. No explicit conversions are stored for synonyms, all
   conversions are maintained w.r.t base types
   */
struct TypeInfo {
    string file;
    set<string> convertibleTo;
    bool noExtract;
};

// This is for function information per function
struct FuncInfo {
    string type; // In case function happens to be method of class type
    vector<string> args;
    string returnType;
    int priority;
    Associativity assoc;
    bool pure;

    // Two FuncInfos are the same if they have the same type and arguments
    // Don't need to check if the return types are the same, as two functions
    // with the same name, type, and arguments with different return types is
    // illegal.
    friend bool operator==( const FuncInfo &f1, const FuncInfo &f2 )
    {
        // Check if class type matches if function is method of class
        if (f1.type != f2.type)
            return false;

        // Check function signature
        if (!(f1.args == f2.args))
            return false;

        // Check associativity if needed
        if (f1.assoc != NoAssoc) {
            if (f1.assoc != f2.assoc)
                return false;
        }

        // Check priority if needed
        if (f1.priority != -1) {
            if (f1.priority != f2.priority)
                return false;
        }

        return true;
    }

    string toString( string fName )
    {
        // Generates a string of the form:
        // [type.]fName( arg1, arg2, ... )
        // where the part in square brackets is only included if the type
        // is not empty.

        stringstream ss;

        if( type != "" )
            ss << type << ".";

        ss << fName << "(";

        vector<string>::const_iterator argIter = args.begin();
        if( argIter != args.end() )
        {
            ss << *argIter;
            ++argIter;
        }

        for( ; argIter != args.end(); ++argIter )
        {
            ss << ", " << *argIter;
        }

        ss << ")";

        return ss.str();
    }
};



class DataTypeManager {

    typedef map<string, TypeInfo*> TypeToInfoMap;
    TypeToInfoMap mType;

    // Function name to function info map
    // Function name is mapped to a set of function infos, as there could
    // be multiple functions with the same name but different parameter lists.
    typedef map<string, set<FuncInfo*> > FuncToInfoMap;
    FuncToInfoMap mFunc;

    // Synonym to base type
    map<string, string> mSynonymToBase;

    static DataTypeManager* instance;

    // Helper functions
    void AddFunc (string type, string fName, vector<string>& args, string returnType, Associativity assoc, int priority, bool pure);
    bool IsCorrectFunc (string type, string fName, vector<string>& args, string& returnType, Associativity assoc, int priority, bool& pure, vector<ArgFormat>& actualArgs);

    // Prefix for conversion functions. For example, if the conversion prefix was "_TO_", then
    // any function named "_TO_T" that take a single argument will be assumed to convert
    // its argument to type T;
    // Note: changed to a define due to silliness with c++ and non-integral const members
    // This is currently the empty string, so constructors with exactly one parameter are
    // considered, just like C++ does.

#define CONVERSION_PREFIX ""

    // Performs a lookup in the function lists to see if there is a conversion function
    // that can be used to convert type source to type dest.

    // Requirements for a function to be a proper conversion function from source to dest:
    // 1. It must have the proper name ( CONVERSION_PREFIX + dest )
    // 2. It must be a function, not a method ( Function's associated type must be ""
    // 3. It must take exactly one argument.
    // 4. The sole argument must be of type source.
    // 5. The return type must be of type dest.
    bool ConvertFuncExists( string source, string dest );

    public:

    DataTypeManager();
    ~DataTypeManager();

    static DataTypeManager& GetDataTypeManager();
    // load/save functions to SQLite
    void Load();
    void Save();
    void Initialize();

    // Used to reset internal data structures.
    void Clear();

    /*****  type management */

    // does the type exist?
    bool DoesTypeExist(string type);

    // add a base type to the system, dont add synonyms using this just to maintain one way consistency
    void AddBaseType(string type, string file);

    // this is used only when the user specifies the schema of relations
    // first argument should be base type to which all synonym can be converted to
    // Base type must be added before using this function, else will fail
    void AddSynonymType(string exitingType, string synonymT);

    // Get the base type to which all synonyms are convertible to
    string GetBaseType(string synonymT);

    // conversion capabiilty
    // All conversions are recorded w.r.t base type, that means, one base type is convertible to other
    // Synonyms shouldn't need conversion to each other, hence no maintainance required wrt synonyms
    void SetPossibleConversion(string fromType, string toType);

    // Is conversion valid
    bool IsConversionPossible(string fromType, string toType);

    // delete everything known about the type
    void DeleteType(string type);

    /**** functions/operators managements
      the Is ... functins return true if the function exists.
      They must use type conversion to decide.
      */
    void AddUnaryOperator(string op, string typearg, string rettype, int priority, bool pure);
    bool IsUnary(string op, string typearg,
            string& /* out param */ rettype, bool& /* out param */ pure,
            vector<ArgFormat>& /* out param */ actualArgs);

    //
    void AddBinaryOperator(string op, string typearg1, string typearg2, string rettype, Associativity assoc, int priority, bool pure);
    bool IsBinary(string op, string typearg1, string typearg2,
            string& /* out param */ rettype, bool& /* out param */ pure,
            vector<ArgFormat>& /* out param */ actualArgs );

    // generic functions. types are specified in order: Have(myType, int, double)
    void AddFunctions(string fName, vector<string>& typeargs, string rettype, bool pure);
    // is this a valid function and if YES what is the return type
    bool IsFunction(string fName, vector<string>& typeargs,
            string& /* out param */ rettype, bool& /* out param */ pure,
            vector<ArgFormat>& /* out param */ actualArgs);

    // same as function, but method to a type myType.Have(int, double)
    void AddMethod(string type, string mName, vector<string>& typeargs, string rettype, bool pure);
    bool IsMethod(string type, string mName, vector<string>& typeargs, string& rettype,
            bool& /* out param */ pure, vector<ArgFormat>& /* out param */ actualArgs );

    // filter functions. Functions that evaluate to bool
    void AddFilter(string fName, vector<string>& typeargs, bool pure);
    bool IsFilter(string fName, vector<string>& typeargs, bool& /* out param */ pure,
            vector<ArgFormat>& /* out param */ actualArgs);

    // Adding GLAs to the system. Represented by 2 functions representing
    // AddItem and GetResult
    void AddGLA(string glaName, vector<string>& typeargs, vector<string>& typeret, string filename );
    bool IsGLA( string glaName, vector<string>& typeargs, vector<string>& typeret,
            vector<ArgFormat>& /* out param */ actualArgs );

    // generate the set of includes for a given list of attributes
    // result should contain the #include statements
    void GenerateIncludes(vector<string>& types, string& result);

}; // DataTypeManager


/****************************** Class definition ******************************/

inline
DataTypeManager::DataTypeManager() {}

inline
DataTypeManager::~DataTypeManager() {

    Clear();
}

    inline
void DataTypeManager::Clear()
{
    TypeToInfoMap::const_iterator it = mType.begin();
    for (; it != mType.end(); ++it) {

        delete it->second;
    }

    mType.clear();

    FuncToInfoMap::const_iterator iter = mFunc.begin();
    for (; iter != mFunc.end(); ++iter) {
        set<FuncInfo*>::const_iterator iterest = iter->second.begin();
        for(; iterest != iter->second.end(); ++iterest )
        {
            delete *iterest;
        }
    }

    mFunc.clear();
}

inline
bool DataTypeManager :: DoesTypeExist(string type) {

    TypeToInfoMap::const_iterator it = mType.find(type);
    if (it != mType.end()) {
        return true;
    }
    else {
        // Check synonyms
        map<string, string>::const_iterator iter = mSynonymToBase.find(type);
        if (iter != mSynonymToBase.end())
            return true;
    }
    return false;
}

inline
void DataTypeManager :: AddBaseType(string type, string file ) {

    // First check if base type already exists
    TypeToInfoMap::const_iterator it = mType.find(type);
    if (it != mType.end()) {
        cout << "\nError: Base type already exists";
        return;
    }

    // Make the new base type also be a synonym for itself. This will make
    // base type lookups easier, as you won't have to handle the case where
    // the type you're asking about is already a base type.
    mSynonymToBase[type] = type;

    // Add it as base type
    TypeInfo* t = new TypeInfo;
    t->file = file;
    t->noExtract = false;
    mType[type] = t;
}

inline
void DataTypeManager :: AddSynonymType(string baseType, string synonymType) {

    // Check if base type exist
    TypeToInfoMap::const_iterator it = mType.find(baseType);
    if (it == mType.end()) {
        cout << "\nError: Base type does not exist, please add it first";
        return;
    }
    // They should not be same
    if (baseType == synonymType) {
        cout << "\nError: Base type and synonym types are same";
        return;
    }
    // Does synonym already exists as base type?
    TypeToInfoMap::const_iterator iter = mType.find(synonymType);
    if (iter != mType.end()) {
        cout << "\nError: Synonym already exists as base type";
        return;
    }

    // Add into synonyms list
    mSynonymToBase[synonymType] = baseType;
}

inline
string DataTypeManager :: GetBaseType(string synonymType) {

    // First check the synonym
    map<string, string>::const_iterator it = mSynonymToBase.find(synonymType);
    if (it != mSynonymToBase.end())
        return it->second;

    //    cout << "\nError: No base type exist for the asked synonym type : " << synonymType;

    // Otherwise, return nothing
    return string("");
}

inline
void DataTypeManager :: SetPossibleConversion(string fromType, string toType) {

    // Assumption is, all conversions should be recorded w.r.t base type, hence get basetype first
    string baseType = GetBaseType(fromType);
    if (baseType.size() == 0) {
        cout << "\nError: Base type not found";
        return;
    }

    // If base type found. Just insert.
    TypeToInfoMap::const_iterator it = mType.find(baseType);
    if (it != mType.end())
        (it->second)->convertibleTo.insert(toType);
}

inline
bool DataTypeManager :: IsConversionPossible(string fromType, string toType) {

    // Assumption is, all conversions should be recorded w.r.t base type, hence get basetype first
    string baseType = GetBaseType(fromType);
    if (baseType.size() == 0) {
        cout << "\nError: Base type not found";
        return false;
    }

    // If base type found. Just check
    TypeToInfoMap::const_iterator it = mType.find(baseType);
    if (it != mType.end()) {
        set<string>::const_iterator iter = (it->second)->convertibleTo.find(toType);
        if (iter != (it->second)->convertibleTo.end())
            return true;
    }
    return false;
}

inline
void DataTypeManager :: DeleteType(string type) {

    // Remove from synonyms list
    map<string, string>::iterator iter = mSynonymToBase.find(type);
    if (iter != mSynonymToBase.end())
        mSynonymToBase.erase(iter);

    // If base, remove from base list
    TypeToInfoMap::iterator it = mType.find(type);
    if (it != mType.end()) {
        delete it->second;
        mType.erase(it);
    }

    // Now check function agruments, if found, just remove that function altogether
    FuncToInfoMap::iterator itf = mFunc.begin();
    for (; itf != mFunc.end(); ++itf) {
        set<FuncInfo*>::const_iterator fiter = itf->second.begin();
        for( ; fiter != itf->second.end(); ++fiter )
        {
            vector<string>::const_iterator iv = find(((*fiter)->args).begin(), ((*fiter)->args).end(), type);
            if ((*fiter)->type == type || (*fiter)->returnType == type || iv != ((*fiter)->args).end()) {
                delete *fiter;
                itf->second.erase(fiter);
            }
        }
    }
}

inline
bool DataTypeManager :: ConvertFuncExists ( string source, string dest ) {
    string funcName = CONVERSION_PREFIX + dest;

    set<FuncInfo*> funcSet = mFunc[funcName];

    for( set<FuncInfo*>::const_iterator iter = funcSet.begin(); iter != funcSet.end(); ++iter ) {
        FuncInfo * curFunc = *iter;

        // Requirements for a function to be a proper conversion function from source to dest:
        // 1. It must have the proper name (already satisfied)

        // 2. It must be a function, not a method
        if( curFunc->type != string("") )
            continue;

        // 3. It must take exactly 1 argument
        if( curFunc->args.size() != 1 )
            continue;

        // 4. The sole argument must be of type source
        if( curFunc->args[0] != source )
            continue;

        // 5. The return type must be of type dest
        if( curFunc->returnType != dest )
            continue;

        // If we have gotten to this point, it is safe to call funcName( source ), and that
        // call will return type dest

        return true;
    }

    // Didn't find a matching function.
    return false;
}

inline
void DataTypeManager :: AddFunc (string type, string fName, vector<string>& args, string returnType, Associativity assoc, int priority, bool pure) {

    // Make sure we are dealing with base types.
    // TODO: Implement better detection for when a corresponding base type for
    // a given type is not found.
    type = GetBaseType( type );
    if( returnType != "" )
        returnType = GetBaseType( returnType );

    for( int i = 0; i < args.size(); ++i )
    {
        args[i] = GetBaseType( args[i] );
    }

    // Create a new struct
    FuncInfo* f = new FuncInfo;
    f->type = type;
    f->args = args;
    f->returnType = returnType;
    f->priority = priority;
    f->assoc = assoc;
    f->pure = pure;

    // Check if function already exists
    FuncToInfoMap::const_iterator it = mFunc.find(fName);
    if (it != mFunc.end()) {
        // Found a function with the same name, now check to see if parameter
        // list of any of the functions with that name is identical

        set<FuncInfo*>::const_iterator iter = it->second.begin();
        for(; iter != it->second.end(); ++iter)
        {
            if( **iter == *f )
            {
                // Identical type and parameter list found for this function
                // name already.
                cout << "\nError: Function/Operator already exists";
                delete f;
                return;
            }
        }
    }



    // Add function name and function info into the map
    mFunc[fName].insert(f);
}

inline
bool DataTypeManager :: IsCorrectFunc (string type, string fName, vector<string>& args, string& returnType, Associativity assoc, int priority, bool& pure, vector<ArgFormat>& actualArgs) {

    // Make sure we are dealing with base types.
    type = GetBaseType( type );

    for( int i = 0; i < args.size(); ++i )
    {
        string temp = args[i];
        temp = GetBaseType( args[i] );

        // If the type is empty, we failed to find a base type for the argument
        // type, and thus this function is invalid.
        if( temp.size() == 0 ) {
            printf("\nFailed to get base type for %s.\n", args[i].c_str());
            return false;
        }

        args[i] = temp;
    }

    // Check if function exists
    FuncToInfoMap::const_iterator it = mFunc.find(fName);
    if (it == mFunc.end()) {
        return false;
    }

    // Need to keep track of any possible matches due to conversions as well as
    // if we have come across an exact match.
    bool exact_match = false;
    vector<FuncInfo*> matches;

    FuncInfo* f;
    set<FuncInfo*>::const_iterator iter = it->second.begin();
    for( ; !exact_match && iter != it->second.end(); ++iter )
    {
        // Check if all details match or are convertible (as applicable)
        exact_match = true;
        bool matchesSoFar = true;

        f = *iter;

        // Check if class type matches if function is method of class
        if (type != f->type)
        {
            exact_match = false;
            matchesSoFar = false;
        }

        // Check function signature

        // Check to see if functions have same number of parameters
        if( f->args.size() != args.size() )
        {
            matchesSoFar = false;
            exact_match = false;
        }

        // If same number of parameters, make sure that the parameters in order
        // are of the same type or of convertible types.
        vector<string>::const_iterator argIter = args.begin();
        vector<string>::const_iterator fArgIter = f->args.begin();
        for( ; matchesSoFar && argIter != args.end() && fArgIter != f->args.end(); ++argIter, ++fArgIter )
        {
            if( *argIter != *fArgIter )
            {
                exact_match = false;

                if( !IsConversionPossible( *argIter, *fArgIter ) &&
                        !ConvertFuncExists(*argIter, *fArgIter) ) {
                    matchesSoFar = false;
                }
            }
        }

        // Check associativity if needed
        if (assoc != NoAssoc) {
            if (f->assoc != assoc)
            {
                matchesSoFar = false;
                exact_match = false;
            }
        }

        // Check priority if needed
        if (priority != -1) {
            if (f->priority != priority)
            {
                matchesSoFar = false;
                exact_match = false;
            }
        }

        // If the function has not been ruled out so far, add it as a possible
        // match
        if( matchesSoFar ) {
            matches.push_back( f );

#ifdef DTM_DEBUG

            //cout << "\nAdded match " << f->toString( fName );

#endif
        }
    }

    if( matches.size() == 0 )
    {
        //printf("None\n");
        // No matches found, the requested function does not exist.
        cout << "\nNo matches found for function ";
        if( type != "" )
            cout << type << ".";

        cout << fName << "(";
        vector<string>::const_iterator argIter = args.begin();

        if( argIter != args.end() )
        {
            cout << *argIter;
            ++argIter;
        }

        for( ; argIter != args.end(); ++argIter )
        {
            cout << ", " << *argIter;
        }

        cout << ")";

        return false;
    }
    else if( exact_match )
    {

        // We found an exact match for the function.
        // The FuncInfo for the exact match will be the last element in the
        // matches vector.
        returnType = matches.back()->returnType;
        pure = matches.back()->pure;

        actualArgs.clear();

        FuncInfo * cur = matches.back();
        for( size_t i = 0; i < args.size(); ++i ) {
            if( args[i] == cur->args[i] || IsConversionPossible( args[i], cur->args[i] ) ) {
                TypeInfo * tInfo = mType[args[i]];
                bool noExtract = tInfo->noExtract;
                actualArgs.push_back( ArgFormat(cur->args[i], "%s", noExtract) );
            }
            else {
                string wrap;
                wrap += CONVERSION_PREFIX + cur->args[i];
                wrap += "(%s)";

                actualArgs.push_back( ArgFormat(cur->args[i], wrap, false ) );

            }
        }

#ifdef DTM_DEBUG
        printf("\n%s(", fName.c_str() );

        if( args.size() > 0 )
            printf("%s", args[0].c_str() );

        for( int x = 1; x < args.size(); ++x )
            printf(" %s", args[x].c_str() );

        printf(") -> %s", returnType.c_str());
#endif

        return true;
    }
    else if( matches.size() == 1 )
    {
        //printf("One\n");
        // We didn't find an exact match, but we found one possible match using
        // type conversions. Print a warning stating this.

        // Warning is of format:
        // "Exact match not found for function [type.]fName(param list),
        // using [f->type.]fName(f param list) instead."
        // The parts in brackets will only be printed if type is not empty.

        cout << "\nExact match not found for function ";

        if( type != "" )
            cout << type << ".";

        cout << fName << "(";
        vector<string>::const_iterator argIter = args.begin();

        if( argIter != args.end() )
        {
            cout << *argIter;
            ++argIter;
        }

        for( ; argIter != args.end(); ++argIter )
        {
            cout << ", " << *argIter;
        }

        cout << "), using function ";

        f = matches.back();

        cout << f->toString( fName );

        // Set return type and return
        returnType = f->returnType;
        pure = f->pure;

        actualArgs.clear();

        FuncInfo * cur = matches.back();
        for( size_t i = 0; i < args.size(); ++i ) {
            if( args[i] == cur->args[i] || IsConversionPossible( args[i], cur->args[i] ) ) {
                TypeInfo * tInfo = mType[args[i]];
                bool noExtract = tInfo->noExtract;
                actualArgs.push_back( ArgFormat(cur->args[i], "%s", noExtract) );
            }
            else {
                string wrap;
                wrap += CONVERSION_PREFIX + cur->args[i];
                wrap += "(%s)";

                actualArgs.push_back( ArgFormat(cur->args[i], wrap, false ) );

            }
        }

        return true;
    }
    else
    {
        //printf("Multiple\n");
        // We found multiple possible matches using type conversions, and thus
        // the function call is ambiguous.

        // Error message is of format:
        // "Exact match not found for [type.]fName(param list)"
        // "\nPossible matches:"
        // For each possible match f:
        // "\n[f->type.]fName(f param list)"

        cout << "\nExact match not found for function ";

        if( type != "" )
            cout << type << ".";

        cout << fName << "(";
        vector<string>::const_iterator argIter = args.begin();

        if( argIter != args.end() )
        {
            cout << *argIter;
            ++argIter;
        }

        for( ; argIter != args.end(); ++argIter )
        {
            cout << ", " << *argIter;
        }

        cout << ")";
        cout << "\nPossible matches:";

        vector<FuncInfo*>::const_iterator fIter = matches.begin();
        for( ; fIter != matches.end(); ++fIter ){
            f = *fIter;
            cout << "\n" << f->toString( fName );
        }

        return false;
    }
}

inline
void DataTypeManager :: AddUnaryOperator(string op, string typearg, string rettype, int priority, bool pure) {

    vector<string> vec;
    vec.push_back(typearg);
    string type;
    AddFunc (type, op, vec, rettype, NoAssoc, priority, pure);
}

inline
bool DataTypeManager :: IsUnary(string op, string typearg, string &rettype, bool& pure,
        vector<ArgFormat>& actualArgs) {

    vector<string> vec;
    vec.push_back(typearg);
    string type;
    return IsCorrectFunc (type, op, vec, rettype, NoAssoc, -1, pure, actualArgs);
}

inline
void DataTypeManager :: AddBinaryOperator(string op, string tp1, string tp2, string rettype, Associativity assoc, int priority, bool pure) {

    vector<string> vec;
    vec.push_back(tp1);
    vec.push_back(tp2);
    string type;
    AddFunc (type, op, vec, rettype, assoc, priority, pure);
}

inline
bool DataTypeManager :: IsBinary(string op, string tp1, string tp2, string &rettype, bool& pure,
        vector<ArgFormat>& actualArgs) {

    vector<string> vec;
    vec.push_back(tp1);
    vec.push_back(tp2);
    string type;
    return IsCorrectFunc (type, op, vec, rettype, NoAssoc, -1, pure, actualArgs);
}

inline
void DataTypeManager :: AddFunctions(string fName, vector<string>& types, string rettype, bool pure) {

    vector<string> vec(types);
    string type;
    AddFunc (type, fName, vec, rettype, NoAssoc, -1, pure);
}

inline
bool DataTypeManager :: IsFunction(string fName, vector<string>& types, string& rettype, bool& pure,
        vector<ArgFormat>& actualArgs) {

    vector<string> vec(types);
    string type;
    return IsCorrectFunc (type, fName, vec, rettype, NoAssoc, -1, pure, actualArgs);
}

inline
void DataTypeManager :: AddMethod(string type, string mName, vector<string>& types, string rettype, bool pure) {

    vector<string> vec(types);
    AddFunc (type, mName, vec, rettype, NoAssoc, -1, pure);
}

inline
bool DataTypeManager :: IsMethod(string type, string mName, vector<string>& types, string& rettype, bool& pure,
        vector<ArgFormat>& actualArgs) {

    vector<string> vec(types);
    return IsCorrectFunc (type, mName, vec, rettype, NoAssoc, -1, pure, actualArgs);
}

inline
void DataTypeManager :: AddFilter(string fName, vector<string>& typeargs, bool pure) {

    vector<string> vec(typeargs);
    string ret;
    string type;
    AddFunc (type, fName, vec, ret, NoAssoc, -1, pure);
}

inline
bool DataTypeManager :: IsFilter(string fName, vector<string>& typeargs, bool& pure,
        vector<ArgFormat>& actualArgs) {

    vector<string> vec(typeargs);
    string ret;
    string type;
    return IsCorrectFunc (type, fName, vec, ret, NoAssoc, -1, pure, actualArgs);
}

inline
void DataTypeManager :: AddGLA(string glaName, vector<string>& typeargs,
        vector<string>& typeret, string fileName = string() ) {
    if( !DoesTypeExist( glaName ) ) {
        AddBaseType( glaName, fileName );
    }

    string fName_add = "AddItem";
    string fName_ret = "GetResult";

    string ret = "";

    vector<string> args_vec(typeargs);
    vector<string> ret_vec(typeret);

    AddFunc( glaName, fName_add, args_vec, ret, NoAssoc, -1, false );
    AddFunc( glaName, fName_ret, ret_vec, ret, NoAssoc, -1, false );
}

inline
bool DataTypeManager ::  IsGLA( string glaName, vector<string>& typeargs,
        vector<string>& typeret,
        vector<ArgFormat>& /* out param */ actualArgs ) {

    vector<string> arg_vec(typeargs);
    vector<string> ret_vec(typeret);
    string ret("");

    //cerr << "GLA: " << glaName << endl;

    //cerr << "Number arguments " << typeargs.size() << endl;

    //for( int i = 0; i < typeargs.size(); ++i ) cerr << typeargs[i] << endl;

    //cerr << "Number returns " << typeargs.size() << endl;

    //for( int i = 0; i < typeargs.size(); ++i ) cerr << typeargs[i] << endl;

    string fName_add = "AddItem";
    string fName_ret = "GetResult";

    bool pure;

    // Check the arguments and get any formatting necessary.
    bool correct = IsCorrectFunc( glaName, fName_add, arg_vec, ret, NoAssoc, -1, pure, actualArgs );

    if( !correct )
        return false;

    // We don't want any transformations on the return arguments
    // so check for an EXACT match for the results function.
    for( int i = 0; i < ret_vec.size(); ++i ) {
        ret_vec[i] = GetBaseType( ret_vec[i] );
    }

    set<FuncInfo*> & possibleFuncs = mFunc[fName_ret];
    for( set<FuncInfo*>::iterator it = possibleFuncs.begin(); it != possibleFuncs.end(); ++it ) {
        FuncInfo *cur = *it;

        /* cerr << "Current candidate:" << endl; */
        /* cerr << "Type: " << cur->type << endl; */
        /* cerr << "Args: "; */
        /* for( int i = 0; i < cur->args.size(); ++i ) */
        /*   cerr << cur->args[i] << " "; */
        /* cerr << endl; */
        /* cerr << "Return type: " << cur->returnType << endl; */

        if( cur->type != glaName ) {
            //cerr << "failed match on return function type name" << endl;
            continue;
        }

        bool goodArgs = true;
        // If same number of parameters, make sure that the parameters in order
        // are of the same type or of convertible types.
        vector<string>::const_iterator argIter = ret_vec.begin();
        vector<string>::const_iterator fArgIter = cur->args.begin();
        for( ; argIter != ret_vec.end() && fArgIter != cur->args.end(); ++argIter, ++fArgIter )
        {
            if( *argIter != *fArgIter )
            {
                //cerr << "Teh failz" << endl;
                goodArgs = false;
                break;
            }
        }

        if( !goodArgs ) {
            //cerr << "failed match on return function args" << endl;
            continue;
        }

        if( cur->returnType != ret ) {
            //cerr << "failed match on return function ret type" << endl;
            continue;
        }

        // Don't need to check anything else really, we found an exact match.
        return true;
    }

    // Didn't find a matching return function :(
    return false;
}

inline
void DataTypeManager :: GenerateIncludes(vector<string>& types, string& result) {

    result.clear();
    for (int i = 0; i < types.size(); i++) {
        string baseType = GetBaseType(types[i]);
        TypeToInfoMap::const_iterator iter = mType.find(baseType);
        if (iter != mType.end()) {
            result += "#include " + (iter->second)->file;
            result += "\n";
        } else {
            cout << "\nError : No entry exists for type " << (types[i]).c_str();
        }
    }
}

inline
DataTypeManager& DataTypeManager::GetDataTypeManager() {
    if (instance == NULL) {
        instance = new DataTypeManager;
        instance->Load();
    }
    return *instance;
}

#endif // _DATA_TYPE_MANAGER_
