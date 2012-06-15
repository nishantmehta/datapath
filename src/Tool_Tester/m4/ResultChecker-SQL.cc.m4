dnl #
dnl #  Copyright 2012 Christopher Dudley
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and  limitations under the License.
dnl #
dnl # M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 PREAMBLE CODE
COPYRIGHT_NOTICE(2012, Christopher Dudley)

#include <iostream>
#include <vector>
#include <string>
#include "ResultChecker.h"
#include "TestResultsDB.h"

using namespace std;

void ResultChecker :: Init( ) {

    // open database
    SQL_OPEN_DATABASE( GetTestResultsDB() );

    // create relation to hold the data concerning test runs
    // if it does not exists.
    SQL_STATEMENTS_NOREZ(</"
        CREATE TABLE IF NOT EXISTS runs (
            runID           INTEGER PRIMARY KEY ASC,
            start_datetime  DATE,
            end_datetime    DATE
        );
    "/>);

    // create relation to hold data for each individual test in a run.
    SQL_STATEMENTS_NOREZ(</"
        CREATE TABLE IF NOT EXISTS tests (
            runID           INTEGER,
            testID          INTEGER,
            fileName        TEXT,
            filePath        TEXT,
            description     TEXT,
            expectedValues  TEXT,
            actualValues    TEXT,
            passed          BOOLEAN,
            reason          TEXT,
            runningTime     DOUBLE,
            PRIMARY KEY( runID, testID ),
            FOREIGN KEY ( runID ) REFERENCES runs ( runID )
        );
    "/>);

    SQL_CLOSE_DATABASE;
}

void ResultChecker :: Save ( string startTime, string endTime ) {

    // open database
    SQL_OPEN_DATABASE( GetTestResultsDB() );

    // Find out the highest runID and add one to get our run ID.
    SQL_STATEMENT_SCALAR(</"
        SELECT MAX(runID)
        FROM runs;
    "/>, runID, int);

    if( runID <= 0 )
        runID = 0;

    ++runID;

    // Insert a row into runs to represent the test run that just occured.
    SQL_STATEMENT_PARAMETRIC_NOREZ(</"
        INSERT
        INTO runs (runID, start_datetime, end_datetime)
        VALUES (?1, datetime(?2, 'unixepoch', 'localtime'), datetime(?3, 'unixepoch', 'localtime'));
    "/>, </int, text, text/>);

    SQL_INSTANTIATE_PARAMETERS(runID, startTime.c_str(), endTime.c_str());

    SQL_PARAMETRIC_END;

    // Now store all of the test data into the tests table
    SQL_STATEMENT_PARAMETRIC_NOREZ(</"
        INSERT
        INTO tests( runID, testID, fileName, filePath, description, expectedValues, actualValues,
            passed, reason, runningTime)
        VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);
    "/>, </int, int, text, text, text, text, text, int, text, double/>);

    for( size_t testID = 0; testID < records.size(); ++testID ) {
        const char * fileName = records[testID].fileName.c_str();
        const char * fullPath = records[testID].fullPath.c_str();
        const char * description = records[testID].description.c_str();
        const char * expectedResult = records[testID].expectedResult.c_str();
        const char * actualResult = records[testID].actualResult.c_str();
        bool passed = records[testID].passed;
        const char * reason = records[testID].reason.c_str();
        double runningTime = records[testID].runningTime;

        SQL_INSTANTIATE_PARAMETERS(runID, testID+1, fileName, fullPath, description, expectedResult,
            actualResult, passed, reason, runningTime);
    }

    SQL_PARAMETRIC_END;

    SQL_CLOSE_DATABASE;

    records.clear();
}
