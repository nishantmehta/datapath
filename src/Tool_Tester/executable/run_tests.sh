#!/bin/bash

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


## run_tests.sh
##
## This script takes a list of directories and runs tests on all of the .pgy
## and .dp files in those directories and subdirectories. It uses M4 to
## extract the expected results for each query from the file being tested,
## as well as the name of the file containing the actual results of the query.
##
## After all of the tests have been run, the results are compared and the
## test is logged in a SQLite database.

# We need at least one directory passed to the script to do anything.
if (($# == 0)); then
    echo "You need to pass at least one file or folder to test!"
    exit 1
fi

# The checking program
CHECKER=$( readlink -n -f "../executable/checker" )

M4=$( which m4 )
M4_DATAFILE=$( readlink -n -f "./datafile.m4" )
M4_RESULTFILE=$( readlink -n -f "./resultfile.m4" )

# Create the files we'll use to keep track of the data and result files we
# make.
DATA_FILES=$( readlink -n -f "./datafiles" )
RESULT_FILES=$( readlink -n -f "./resultfiles" )

touch $DATA_FILES
touch $RESULT_FILES

# This function may need to be changed depending on how queries are run for
# the tests.

DP=$(readlink -n -f "../../Tool_DataPath/executable/dp")
DP_OPTS="-r -q -e"
DP_PATH=$(dirname $DP)
RESULTS_FOLDER=$( readlink -n -f "../../Tool_DataPath/executable/RESULTS" )

function RunDP {
    echo "Running DataPath on file $1"
    local cwd=$(pwd)
    cd $DP_PATH
    local startT=$(date "+%s")
    $DP $DP_OPTS $1
    local endT=$(date "+%s")
    cd $cwd
    runningTime=$(echo $endT - $startT | bc )
}

function HandleFile {
    if [[ $1 != *.pgy ]] && [[ $1 != *.dp ]]; then
        return
    fi

    CUR_DATA_FILE=$( readlink -n -f "$1.data" )
    touch $CUR_DATA_FILE
    echo $1 >> $CUR_DATA_FILE

    RunDP $1
    echo $runningTime >> $CUR_DATA_FILE

    $M4 -DFILE_NAME=$1 $M4_DATAFILE >> $CUR_DATA_FILE

    echo $CUR_DATA_FILE >> $DATA_FILES


    REZ_FILE=$( $M4 -DFILE_NAME=$1 $M4_RESULTFILE )
    CUR_RESULT_FILE="$RESULTS_FOLDER/$REZ_FILE"
    echo $CUR_RESULT_FILE >> $RESULT_FILES

    return
}

function RecurseDirs {
    local my_file=$1
    local my_contents="$my_file/*"
    for f in $my_contents
    do

        local file=$( readlink -n -f "$f" )
        if [[ -d "$file" ]]; then
            RecurseDirs $file
        else
            HandleFile $file
        fi
    done

    return
}

# Before we run the tests, get the current time
START_TIME=$( date "+%s" )

for arg in $@
do
    TOP_LEVEL_DIR=$( readlink -n -f "$arg" )
    RecurseDirs $TOP_LEVEL_DIR
done

# We've run all of the tests, get the ending time
END_TIME=$( date "+%s" )

# Run the checker program
$CHECKER $START_TIME $END_TIME $DATA_FILES $RESULT_FILES

# Cleanup
for file in $( cat $DATA_FILES )
do
    rm $file
done

#for file in $( cat $RESULT_FILES )
#do
    #rm $file
#done

rm $DATA_FILES
rm $RESULT_FILES

exit
