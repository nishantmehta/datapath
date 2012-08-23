#!/bin/bash
# $1 = base of the file names

M4INCLUDES="-I ../../../M4/m4 -I ../../../WPFunctionModules -I ../../../GLAs -I ../../../UDFs -I ../../../Libs"

DESC_M4="descfile.m4"

M4_FILE="$1.m4"
H_FILE="$1.h"
DESC_FILE="$1.desc"
TMP_FILE="$1.tmp"

cd Generated

m4 $M4INCLUDES $M4_FILE > $H_FILE

if (($? != 0)); then
    echo "Failed to run M4 on file $M4_FILE"
    exit 1
fi

m4 $M4INCLUDES --define=SOURCE_FILE=$H_FILE $DESC_M4 > $TMP_FILE

if (($? != 0)); then
    echo "Failed to run M4 on file $H_FILE"
    exit 2
fi

# Just get the last line of the description file, as there could be multiple
# lines due to other files that were m4_included to satisfy dependencies.
tail --lines=1 $TMP_FILE > $DESC_FILE
rm $TMP_FILE
