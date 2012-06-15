#!/usr/bin/env python2

#  Copyright 2012 Christopher Dudley
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and  limitations under the License.


# Python 2 version of run_tests.sh if that ends up being too slow

## run_tests.py

"""
This script takes a list of directories and runs tests on all of the .pgy
and .dp files in those directories and subdirectories. It uses M4 to
extract the expected results for each query from the file being tested,
as well as the name of the file containing the actual results of the query.

After all of the tests have been run, the results are compared and the
test is logged in a SQLite database.
"""

from os.path import realpath
import os
import time
import subprocess
import fnmatch
import argparse

# Set up the argument parser
parser = argparse.ArgumentParser(
        description='Run a batch of tests on DataPath and store the results.')
parser.add_argument('file',
        nargs='+',
        help='a file or a folder with which to run tests.')

# Parse the arguments
args = parser.parse_args()

# The checking program
CHECKER = realpath("./checker")

M4 = "m4"
M4_DATAFILE = realpath("./datafile.m4")
M4_RESULTFILE = realpath("./resultfile.m4")

# Create the files we'll use to keep track of the data and result files we make
DATA_FILES = realpath("./datafiles")
RESULT_FILES = realpath("./resultfiles")

dataFiles = open(DATA_FILES, "w")
resultFiles = open(RESULT_FILES, "w")

DP = realpath( "../../Tool_DataPath/executable/dp" )
DP_OPTS = ["-r", "-q", "-e"]
RESULTS_FOLDER = realpath( "../../Tool_DataPath/executable/RESULTS" )

# Set of paths we've already visited to prevent infinitely recursing through
# directories due to symbolic links.
visitedPaths = set()

def RunDP( fileName ):
    print "Running DataPath on file", fileName
    startT = time.time()

    dpDir = os.path.dirname( DP )
    procCall = list()
    procCall.append(DP)
    procCall.extend(DP_OPTS)
    procCall.append(fileName)

    try:
        subprocess.check_call(procCall, cwd=dpDir )
    except:
        print "Error running file", fileName
    endT = time.time()

    return endT - startT

def HandleFile( fileName ):
    if not fnmatch.fnmatch( fileName, "*.pgy") and not fnmatch.fnmatch( fileName, "*.dp") :
        return

    if fileName in visitedPaths:
        return

    visitedPaths.add(fileName)

    CUR_DATA_FILE = realpath( fileName + ".data")
    curDataFile = open( CUR_DATA_FILE, "w" )

    curDataFile.write(fileName + "\n")

    # Run datapath on the file and get the running time
    runningTime = RunDP( fileName )

    curDataFile.write( str( runningTime ) + "\n" )

    m4error = False

    try:
        stuff = subprocess.check_output([M4, "-DFILE_NAME=" + fileName, M4_DATAFILE])
    except:
        print "Error generating data file for", fileName
        m4error = True

    if not m4error:
        curDataFile.write(stuff)
        dataFiles.write(CUR_DATA_FILE + "\n")
    else:
        dataFiles.write("<ERROR>\n")

    m4error = False

    try:
        REZ_FILE = subprocess.check_output([M4, "-DFILE_NAME=" + fileName, M4_RESULTFILE])
    except:
        print "Error getting result file name from", fileName
        m4error = True

    if not m4error:
        CUR_RESULTS_FILE = os.path.join(RESULTS_FOLDER , REZ_FILE)
        resultFiles.write(CUR_RESULTS_FILE + "\n")
    else:
        resultFiles.write("<ERROR>\n")

    return

def RecurseDirs( dirPath ):
    if dirPath in visitedPaths:
        return

    #print "Visiting:", dirPath
    visitedPaths.add( dirPath )

    for fileName in os.listdir( dirPath ):
        if( fileName[0] == '.' ) :
            continue;

        absFileName = realpath( os.path.join( dirPath, fileName  ) )

        if os.path.isdir( absFileName ):
            #print "Found directory:", absFileName
            RecurseDirs( absFileName )
        else:
            #print "Found file:", absFileName
            HandleFile( absFileName )

    return

# Before we start running tests, get the current time (unix timestamp)
startTime = int(time.time())

# For all arguments given to the script, run tests on them!
for fileName in args.file:
    absFileName = realpath( fileName )

    if os.path.isdir( absFileName ):
        RecurseDirs( absFileName )
    else:
        HandleFile( absFileName )

# Now that we've run all of the tests, get the stop time
endTime = int(time.time())

dataFiles.flush()
resultFiles.flush()

dataFiles.close()
resultFiles.close()

dataFiles = open( DATA_FILES, 'r' )
resultFiles = open( RESULT_FILES, 'r' )

# Run the checker program!
try:
    subprocess.check_call([CHECKER, str( startTime ), str( endTime ), DATA_FILES, RESULT_FILES])
except subprocess.CalledProcessError as e:
    print "Error running the checker! Return code:", e.returncode

# Cleanup

for fileName in dataFiles:
    fileName = fileName.strip() # fileName still has the newline on the end
    os.remove(fileName)

for fileName in resultFiles:
    fileName = fileName.strip()
    os.remove(fileName)

dataFiles.close()
resultFiles.close()

os.remove(DATA_FILES)
os.remove(RESULT_FILES)
