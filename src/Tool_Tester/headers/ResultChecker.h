
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

#ifndef _RESULT_CHECKER_H_
#define _RESULT_CHECKER_H_

#include <map>
#include <string>
#include <inttypes.h>
#include <vector>

using namespace std;

/**
 * This class is used to check all of the computed results versus the expected
 * results for all of the tests run by the test script. It uses
 * type-appropriate comparisons on the results and stores the results of the
 * tests in a SQLite database, allowing for reports to be generated later.
 */

class ResultChecker {

    typedef bool (*CompareFunc)(string, string);

    map<string, CompareFunc> compFuncs;

    struct FileTestRecord {
        string fileName;
        string fullPath;
        string description;
        string expectedResult;
        string actualResult;
        bool passed;
        string reason;
        double runningTime;
    };

    vector<FileTestRecord> records;

    ///// Functions on the SQLite Database /////

    // Initializes the database if it doesn't exist
    void Init( );

    // Saves the data from this test to the database.
    // Note: the times must be in local-time unix timestamps
    void Save( string startTime, string endTime );

    ///// Other functions /////

    // Adds a single test.
    void AddItem( string& fileName, string& description,
            vector<string>& expected, vector<string>& actual,
            vector<string>& types, double runningTime );

    // Extracts the file name from a path
    string getFileName( string path );

    // Turns a list of items separated by newlines or commas into a vector
    vector<string> tokenize( istream& str, const char DELIM );
    vector<string> tokenize( string str, const char DELIM );

    // Used to parse the result files and expected data files.
    void parseResultFile( string fileName, vector<string>& types, vector<string>& values );
    void parseDataFile( string fileName, string& sourceFile, double& runningTime, string& description,
            vector<string>& values);

public:

    ResultChecker( );
    ~ResultChecker( );

    // Starts checking the results for the given lists of expected results files
    // and actual results files.
    void Start( string startTime, string endTime,
            vector<string>& expectedFiles, vector<string>& resultFiles );

};

#endif // _RESULT_CHECKER_H_
