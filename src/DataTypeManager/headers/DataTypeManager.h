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
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>

#include "ArgFormat.h"

/** Class to facilitate type management.

    Warning: it is not thread safe so make sure only one component
    (like the parser) uses it.


    Types reffered to in relations schema or AttributeManager have to
    exist here. The parser checks that this is the case.

    Make sure only one of these can be created in the system.

*/

class DataTypeManager {

    // STL typedefs so I don't go insane adding std:: to everything
    typedef std::string string;
    typedef std::vector<std::string> string_vector;
    typedef std::vector<ArgFormat> argf_vector;
    typedef std::set<std::string> string_set;
    typedef std::stringstream stringstream;

public:
    enum Associativity {
        LeftToRight,
        RightToLeft,
        NoAssoc
    };

private:
    // This is used to store type information
    /*
       This maintains only base type information and all synonyms will point to this base type
       It keeps what all types it can be converted to, synonyms are only convertible to each
       other if their base types are. No explicit conversions are stored for synonyms, all
       conversions are maintained w.r.t base types
       */
    struct TypeInfo {
        string file;
        string_set convertibleTo;
        bool noExtract;
    };

    // This is for function information per function
    struct FuncInfo {
        string type; // In case function happens to be method of class type
        string_vector args;
        string returnType;
        int priority;
        Associativity assoc;
        string file;
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

            string_vector::const_iterator argIter = args.begin();
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

    enum TemplateType {
        T_INVALID,
        T_FUNC,
        T_GLA,
        T_GT,
        T_GF,
        T_GIST
    };

    // This struct will eventually also hold information about the types of
    // template parameters this template allows.
    struct TemplateInfo {
        TemplateType type;
        string file;

        TemplateInfo( TemplateType t, string file ) : type(t), file(file) {}

        TemplateInfo() : type(T_INVALID) {}
    };

    typedef std::map<string, TypeInfo*> TypeToInfoMap;
    TypeToInfoMap mType;

    // Function name to function info map
    // Function name is mapped to a set of function infos, as there could
    // be multiple functions with the same name but different parameter lists.
    typedef std::map<string, std::set<FuncInfo*> > FuncToInfoMap;
    FuncToInfoMap mFunc;

    typedef std::map<string, TemplateInfo> TempToInfoMap;
    TempToInfoMap mTemp;

    // Synonym to base type
    std::map<string, string> mSynonymToBase;

    static DataTypeManager* instance;

    // Helper functions
    void AddFunc (string type, string fName, string_vector& args, string returnType, string file, Associativity assoc, int priority, bool pure);
    bool IsCorrectFunc (string type, string fName, string_vector& args, string& returnType, string& file, Associativity assoc, int priority, bool& pure, argf_vector& actualArgs);

    void AddTemplate( string name, TemplateType type, string file );
    bool TemplateExists( string name, TemplateType type, string& file );

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

    // Function to determine if a given function call is an exact match to a function
    // registered with the system. Type conversions are not taken into account.
    bool IsCorrectFuncExact (string type, string fName, string_vector& args, string& returnType, string& file, Associativity assoc, int priority, bool& pure);

    // Due to the Singleton pattern, the constructor and destructor are private
    // and can only be called by the static methods of the class
    DataTypeManager();
    ~DataTypeManager();

public:

    static DataTypeManager& GetDataTypeManager();
    // load/save functions to SQLite
    //void Load();
    //void Save();
    //void Initialize();

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
    bool IsType(string& type);

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
    void AddUnaryOperator(string op, string typearg, string rettype, string file, int priority, bool pure);
    bool IsUnary(string op, string typearg,
            string& /* out param */ rettype, string& file, bool& /* out param */ pure,
            argf_vector& /* out param */ actualArgs);

    //
    void AddBinaryOperator(string op, string typearg1, string typearg2, string rettype, string file, Associativity assoc, int priority, bool pure);
    bool IsBinary(string op, string typearg1, string typearg2,
            string& /* out param */ rettype, string& file, bool& /* out param */ pure,
            argf_vector& /* out param */ actualArgs );

    // generic functions. types are specified in order: Have(myType, int, double)
    void AddFunctions(string fName, string_vector& typeargs, string rettype, string file, bool pure);
    void AddFunctionTemplate( string fName, string file );
    // is this a valid function and if YES what is the return type
    bool IsFunction(string fName, string_vector& typeargs,
            string& /* out param */ rettype, string& file, bool& /* out param */ pure,
            argf_vector& /* out param */ actualArgs);
    bool IsFunctionTemplate( string fName, string& file );

    // same as function, but method to a type myType.Have(int, double)
    void AddMethod(string type, string mName, string_vector& typeargs, string rettype, string file, bool pure);
    bool IsMethod(string type, string mName, string_vector& typeargs, string& rettype, string& file,
            bool& /* out param */ pure, argf_vector& /* out param */ actualArgs );

    // filter functions. Functions that evaluate to bool
    void AddFilter(string fName, string_vector& typeargs, string file, bool pure);
    bool IsFilter(string fName, string_vector& typeargs, string& file, bool& /* out param */ pure,
            argf_vector& /* out param */ actualArgs);

    // Adding GLAs to the system. Represented by 2 functions representing
    // AddItem and GetResult
    void AddGLA(string glaName, string_vector& typeargs, string_vector& typeret, string filename );
    bool IsGLA( string& glaName, string_vector& typeargs, string_vector& typeret, string& file,
            argf_vector& /* out param */ actualArgs );

    bool GLAExists( string& glaName, string& file );

    void AddGLATemplate( string glaName, string filename );
    bool IsGLATemplate( string glaName, string& filename );

    void AddGT(string gtName, string_vector& typeargs, string_vector& typeret, string filename );
    bool IsGT( string& gtName, string_vector& typeargs, string_vector& typeret, string& file,
            argf_vector& /* out param */ actualArgs );

    bool GTExists( string& gtName, string& file );

    void AddGTTemplate( string gtName, string filename );
    bool IsGTTemplate( string gtName, string& filename );

    void AddGF(string gtName, string_vector& typeargs, string filename );
    bool IsGF( string& gtName, string_vector& typeargs, string& file,
            argf_vector& /* out param */ actualArgs );

    bool GFExists( string& gtName, string& file );

    void AddGFTemplate( string gtName, string filename );
    bool IsGFTemplate( string gtName, string& filename );

    void AddGIST(string gistName, string_vector& stateargs, string_vector& typeret, string filename );
    bool IsGIST( string& gistName, string_vector& stateargs, string_vector& typeret, string& file );

    bool GISTExists( string& gistName, string& file );

    void AddGISTTemplate( string gistName, string filename );
    bool IsGISTTemplate( string gistName, string& filename );

    // generate the set of includes for a given list of attributes
    // result should contain the #include statements
    void GenerateIncludes(string_vector& types, string& result);

    string GetTypeFile(string type);

}; // DataTypeManager

#endif // _DATA_TYPE_MANAGER_
