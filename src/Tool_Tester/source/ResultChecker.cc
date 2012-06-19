
//  Copyright 2012 Christopher Dudley
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
//  See the License for the specific language governing permissions and  limitations under the License.

#include <map>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <errno.h>

#include "ResultChecker.h"
#include "ComparisonFunctions.h"

#define DELIMITER ','

using namespace std;

// Comparison functions

ResultChecker :: ResultChecker() {
    compFuncs["FLOAT"] = &compareFloat;
    compFuncs["DOUBLE"] = &compareDouble;
    compFuncs["INT"] = &compareInt64;
    compFuncs["BIGINT"] = &compareInt64;
    compFuncs["HString"] = &compareString;
}

ResultChecker :: ~ResultChecker() { }

string ResultChecker :: getFileName( string path ) {
    size_t index = 0;

    index = path.rfind( "/" );

    return path.substr( index + 1, string::npos );
}

vector<string> ResultChecker :: tokenize( istream& str ) {
    vector<string> result;

    stringstream cur;

    char c;
    bool inQuote = false;

    while( str.get(c) ) {
        switch( c ) {
            case '"':
                inQuote = !inQuote;
                break;
            case '\\':
                if( !str.get(c) ) {
                    cerr << "Escape character found, but no character after it!" << endl;
                }
                else {
                    cur.put(c);
                }
                break;
            case DELIMITER:
                if( !inQuote ) {
                    result.push_back( cur.str() );
                    cur.str("");
                    cur.clear();
                }
                else {
                    cur.put( DELIMITER );
                }
                break;
            default:
                cur.put(c);
        }
    }

    return result;
}

/*
vector<string> ResultChecker :: tokenize( istream& str ) {
    string line;

    vector<string> result;

    while( getline( str, line ) ) {
        size_t index = 0;

        do {
            size_t prev = index;
            index = line.find( ",", index );

            size_t len = index == string::npos ? index : index - prev;

            result.push_back( line.substr( prev, len ) );

            if( index != string::npos )
                ++index;
        } while( index != string::npos && index < line.length() );
    }

    return result;
}
*/

vector<string> ResultChecker :: tokenize( string str ) {
    istringstream ss(str);
    return tokenize( ss );
}

void ResultChecker :: AddItem( string& fileName, string& description, vector<string>& expected,
        vector<string>& actual, vector<string> & types, double runningTime ) {

#define ADD_REASON(x) if(rec.reason != "") \
    rec.reason += ","; \
    rec.reason += (x);

    FileTestRecord rec;

    rec.fileName = getFileName(fileName);
    rec.fullPath = fileName;
    rec.description = description;
    rec.passed = false;
    rec.reason = "";
    rec.runningTime = runningTime;

    bool instaFail = false;

    if( expected.size() == 0 ) {
        ADD_REASON( "No expected values" )
        instaFail = true;
    }
    else {
        rec.expectedResult += expected[0];

        for( size_t i = 1; i < expected.size(); ++i ) {
            rec.expectedResult += ", ";
            rec.expectedResult += expected[i];
        }

        if( expected[0] == "<ERROR>" ) {
            instaFail = true;
            ADD_REASON( "There as an error extracting the expected results" )
        }
    }

    if( actual.size() == 0 ) {
        ADD_REASON( "No actual values" )
        instaFail = true;
    }
    else {
        rec.actualResult += actual[0];

        for( size_t i = 1; i < actual.size(); ++i ) {
            rec.actualResult += ", ";
            rec.actualResult += actual[i];
        }

        if( actual[0] == "<ERROR>" ) {
            instaFail = true;
            ADD_REASON( "There was an error running the query" )
        }
    }

    if( actual.size() != expected.size() ) {
        instaFail = true;
        ADD_REASON( "Expected result and actual result are of different sizes" )
    }

    if( instaFail ) {
        records.push_back( rec );
        return;
    }

    rec.passed = true;

    vector<string>::iterator expectedIter = expected.begin();
    vector<string>::iterator actualIter = actual.begin();
    vector<string>::iterator typeIter = types.begin();

    for( ; expectedIter != expected.end() && actualIter != actual.end();
            ++expectedIter, ++actualIter, ++typeIter ) {
        bool passVal = false;

        if( typeIter == types.end() )
            typeIter = types.begin();

        if( compFuncs.find(*typeIter) == compFuncs.end() ) {
            passVal = compareString( *expectedIter, *actualIter );
        }
        else {
            passVal = compFuncs[*typeIter]( *expectedIter, *actualIter );
        }

        rec.passed &= passVal;

        if( !passVal ) {
            ADD_REASON( "Expected: " + *expectedIter + " != Actual: " + *actualIter +
                    " (Type: " + *typeIter + ")")
        }
    }

    if( rec.passed )
        rec.reason = "Passed";

    records.push_back(rec);
}

/*
 * The format of the result file is as follows (note: <BOF> = Beginning of File,
 * <EOF> = End of File):
 *
 * <BOF>
 * comma separated list of variable names
 * comma separated list of types
 * comma separated list of values (one line per tuple)
 * <EOF>
 */
void ResultChecker :: parseResultFile( string fileName, vector<string>& types, vector<string>& values ) {
    // Make sure there wasn't an error generating the result file.
    // If the file name is "<ERROR>", then DataPath failed to run the query
    // and thus there are no results.
    if( fileName == "<ERROR>" ) {
        values.push_back("<ERROR>");
        return;
    }

    ifstream fin( fileName.c_str() );

    // Make sure the file is actual valid for reading.
    if( !fin ) {
        values.push_back("<ERROR>");
        return;
    }

    string line;

    // First line is the name of the variables, we don't really care about those.
    if (!getline( fin, line )) {
        values.push_back("<ERROR>");
        return;
    }

    // Next line is the types of the variables, this we care about.
    if (!getline( fin, line )) {
        values.push_back("<ERROR>");
        return;
    }

    types = tokenize( line );

    // Everything else are the values. Just tokenize them.
    values = tokenize( fin );
}

/*
 * The format of the data file is as follows:
 *
 * <BOF>
 * full path to source file
 * running time of query
 * description of query
 * comma separated list of expected results (possibly on more than one line)
 * <EOF>
 */
void ResultChecker :: parseDataFile( string fileName, string& sourceFile, double& runningTime, string& description,
        vector<string>& values ) {

    // Make sure there wasn't an error somehow in generating the data file.
    if( fileName == "<ERROR>" ) {
        values.push_back("<ERROR>");
        return;
    }

    ifstream fin( fileName.c_str() );

    // Make sure that the file is valid for reading
    if( !fin ) {
        values.push_back("<ERROR>");
        return;
    }

    string line;

    // Get the full path out of the file
    if (!getline( fin, line )) {
        values.push_back("<ERROR>");
        return;
    }

    sourceFile = line;

    // Get the running time from the file
    if (!getline( fin, line )) {
        values.push_back("<ERROR>");
        return;
    }

    runningTime = strtod( line.c_str(), NULL );

    // Get the description from the file
    if (!getline( fin, line )) {
        values.push_back("<ERROR>");
        return;
    }

    description = line;

    // The rest of the file consists of values. Tokenize them.
    values = tokenize( fin );
}

void ResultChecker :: Start( string startTime, string endTime, vector<string>& expectedFiles,
        vector<string>& resultFiles ) {

    if( expectedFiles.size() != resultFiles.size() ) {
        cerr << "Got a different number of result and data files!" << endl;
        return;
    }

    Init( );

    vector<string>::iterator expectedFilesIter = expectedFiles.begin();
    vector<string>::iterator resultFilesIter = resultFiles.begin();

    for( ; expectedFilesIter != expectedFiles.end() && resultFilesIter != resultFiles.end();
            ++expectedFilesIter, ++resultFilesIter) {
        // Get information from the data file.
        string sourceFile;
        double runningTime;
        string description;
        vector<string> expectedValues;

        parseDataFile( *expectedFilesIter, sourceFile, runningTime, description, expectedValues );

        // Get information from the results file
        vector<string> types;
        vector<string> actualValues;

        parseResultFile( *resultFilesIter, types, actualValues );

        AddItem( sourceFile, description, expectedValues, actualValues, types, runningTime );
    }

    Save( startTime, endTime );
}
