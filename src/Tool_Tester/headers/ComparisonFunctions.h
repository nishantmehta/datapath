
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

#ifndef _COMPARISON_FUNCTIONS_H_
#define _COMPARISON_FUNCTIONS_H_

bool compareDouble( double d1, double d2, const double MAX_ABS_ERROR, const double MAX_REL_ERROR ) {

    if( d2 == d1 )
        return true;

    double absErr = d2 - d1;
    if( absErr < 0 )
        absErr = -absErr;

    if( absErr <= MAX_ABS_ERROR )
        return true;

    double absd2 = d2 >= 0 ? d2 : -d2;
    double absd1 = d1 >= 0 ? d1 : -d1;

    double relErr;

    if( absd2 > absd1 )
        relErr = absErr / absd2;
    else
        relErr = absErr / absd1;

    if( relErr <= MAX_REL_ERROR )
        return true;

    return false;
}

bool compareDouble( string expected, string actual ) {
    const double MAX_REL_ERROR = 0.00001;
    const double MAX_ABS_ERROR = 100 * DBL_EPSILON;

    errno = 0;

    double expct = strtod( expected.c_str(), NULL );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert expected value: %s to a double\n", expected.c_str());
        return false;
    }

    double act = strtod( actual.c_str(), NULL );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert actual value: %s to a double\n", actual.c_str());
        return false;
    }

    return compareDouble( expct, act, MAX_ABS_ERROR, MAX_REL_ERROR );
}

bool compareFloat( string expected, string actual ) {
    const double MAX_REL_ERROR = 0.0001;
    const double MAX_ABS_ERROR = 100 * FLT_EPSILON;

    errno = 0;

    double expct = strtod( expected.c_str(), NULL );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert expected value: %s to a double\n", expected.c_str());
        return false;
    }

    double act = strtod( actual.c_str(), NULL );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert actual value: %s to a double\n", actual.c_str());
        return false;
    }

    return compareDouble( expct, act, MAX_ABS_ERROR, MAX_REL_ERROR );
}

bool compareInt64( string expected, string actual ) {
    long expectedInt;
    long actualInt;

    errno = 0;

    expectedInt = strtol( expected.c_str(), NULL, 0 );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert expected value: %s to a long\n", expected.c_str());
        return false;
    }

    actualInt = strtol( actual.c_str(), NULL, 0 );
    if( errno != 0 ) {
        fprintf(stderr, "Failed to convert actual value: %s to a long\n", actual.c_str());
        return false;
    }

    return( expectedInt == actualInt );
}

bool compareString( string expected, string actual ) {
    return (expected == actual);
}

#endif // _COMPARISON_FUNCTIONS_H_
