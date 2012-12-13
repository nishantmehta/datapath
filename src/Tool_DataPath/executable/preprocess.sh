#!/bin/bash
# $1 = input file
# $2 = output file

LIB_DIR=$(readlink -f ./Libs)
INC_DIRS="-I $LIB_DIR"

cpp -C -P $INC_DIRS -include base.pgy $1 > $2
