#!/bin/bash

if (($# != 1 || $# != 2)); then
    echo "Usage: make_factor.sh <library> [factor file]"
    echo
    echo "This script generates factors for a library using a factor file with one line"
    echo "per factor of the form:"
    echo "    storage type|factor_name"
    echo
    echo "If no factor file is specified, the file <library>/FACTORS will be assumed."
    exit 1
fi

LIB_NAME=$1
LIB_DIR=$(readlink -f $LIB_NAME)
FACT_FILE=$LIB_DIR/FACTORS

if (($# == 2)); then
    FACT_FILE=$(readlink -f $2)
fi

M4_FILE=$(readlink -f ./Factor.h.m4)

if [ ! -f $M4_FILE ]; then
    echo "Error: Unable to locate Factor m4 file."
    exit 1
fi

if [ ! -d $LIB_DIR ]; then
    echo "Error: Library $LIB_NAME does not exist!"
    exit 1
fi

if [ ! -f $FACT_FILE ]; then
    echo "Error: Factor file $FACT_FILE does not exist or is not a regular file!"
    exit 1
fi

# Make sure the Types directory exists
[ -d $LIB_DIR/Types ] || mkdir $LIB_DIR/Types

while read line
do
    STORAGE_TYPE=$(echo $line | cut -d "|" -f 1 -)
    DICT_NAME=$(echo $line | cut -d "|" -f 2 -)

    echo "Creating factor \"$DICT_NAME\" with storage type \"$STORAGE_TYPE\""
    m4 -P "--define=STORAGE_TYPE=$STORAGE_TYPE" "--define=DICT_NAME=$DICT_NAME" $M4_FILE > "$LIB_DIR/Types/$DICT_NAME.h"
done < $FACT_FILE

# Rebuild library

./make_desc.sh $LIB_NAME
