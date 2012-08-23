#!/bin/bash
# $1 = input file
# $2 = output file

INC_DIRS="-I ../../Libs/"

cpp -C -P $INC_DIRS -include base.pgy $1 > $2
