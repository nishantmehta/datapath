#!/bin/bash

# Script to automatically create a library description file from a set of
# source files.
# The source files are expected to be contained in the following folder
# hierarchy:
#                       Libs
#                        |
#                     library
#                        |
#    ----------------------------------------------------------------
#    |           |             |           |        |       |       |
#  Types      Functions       GLAs        UDFs     GFs     GTs    GISTs
#
#This script will create a file called library.pgy in the Libs folder, where
#library is replaced with the name of the library being built.

# Make sure we have exactly one argument
if (($# != 1)); then
    echo "Usage: make_desk [lib_name]"
    echo
    echo "This utility generates a DataPath description file for a library by parsing"
    echo "the individual files in the library. The argument should be the name of the"
    echo "library, which should be located in a directory of the same name."
    exit 1
fi

LIB_NAME=$1
M4="m4"
M4_INCLUDES="../src/M4/m4"

DIRECTORIES="Types Functions GLAs UDFs GFs GTs GISTs"
OUT_FILE="${LIB_NAME}.pgy"

DATE_TIME=$(date +"%A, %B %d, %Y at %H:%M %Z")

# Make sure the library directory exists
if [ ! -e $LIB_NAME ] || [ ! -d $LIB_NAME ]; then
    echo "No directory found for library $LIB_NAME."
    exit 1
fi

# Erase any existing description file
if [ -e $OUT_FILE ]; then
    rm $OUT_FILE
fi
touch $OUT_FILE

# Write the header of the description file
echo "#ifndef _${LIB_NAME}_PGY_" >> $OUT_FILE
echo "#define _${LIB_NAME}_PGY_" >> $OUT_FILE
echo "// Piggy description file for library $LIB_NAME" >> $OUT_FILE
echo "// generated on $DATE_TIME" >> $OUT_FILE

for dir in $DIRECTORIES
do
    curDir=$LIB_NAME/$dir""
    if [ -e $curDir ] && [ -d $curDir ]; then
        for file in $curDir/*
        do
            if [ -e $file ] && [ -f $file ]; then
                echo >> $OUT_FILE
                echo "// $file" >> $OUT_FILE
                output=$($M4 --define=SOURCE_FILE=$file -I $M4_INCLUDES descfile.m4)
                if [ $? ]; then
                    echo "$output" >> $OUT_FILE
                else
                    echo "Error running M4 on file $file"
                fi
            fi
        done
    fi
done

# Write footer
echo "#endif // _${LIB_NAME}_PGY_" >> $OUT_FILE
