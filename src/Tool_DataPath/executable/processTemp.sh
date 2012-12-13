#!/bin/bash
# $1 = base of the file names
# Note: this file is run in the context of the executable folder

DP_EXEC_DIR=$(readlink -f .)
DP_SRC_DIR=$(readlink -f $DP_EXEC_DIR/../..)
M4_DIR=$DP_SRC_DIR/M4/m4
WPF_DIR=$DP_SRC_DIR/WPFunctionModules
GLA_DIR=$DP_SRC_DIR/GLAs
UDF_DIR=$DP_SRC_DIR/UDFs
LIB_DIR=$(readlink -f $DP_EXEC_DIR/Libs)

M4INCLUDES="-I $M4_DIR -I $WPF_DIR -I $GLA_DIR -I $UDF_DIR -I $LIB_DIR"

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
