
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

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "ResultChecker.h"

using namespace std;

int main( int argc, char ** argv ) {
    if( argc != 5 ) {
        cerr << "The result checker program got called with an incorrect"
            << " number of arguments!" << endl;
        exit(EXIT_FAILURE);
    }

    string startTime = argv[1];
    string endTime = argv[2];

    vector<string> expectedFiles;
    vector<string> resultFiles;

    ifstream expectedIn( argv[3] );

    string line;

    while( getline( expectedIn, line ) ) {
        expectedFiles.push_back(line);
    }

    expectedIn.close();

    ifstream resultIn( argv[4] );

    while( getline( resultIn, line ) ) {
        resultFiles.push_back(line);
    }

    resultIn.close();

    ResultChecker rs;

    rs.Start( startTime, endTime, expectedFiles, resultFiles );
}
