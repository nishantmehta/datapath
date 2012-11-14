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
#  See the License for the specific language governing permissions and
#  limitations under the License.

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
LIB_HEADER_FILE="${LIB_NAME}.h"

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

# Erase any existing header file
if [ -e $LIB_HEADER_FILE ]; then
    rm $LIB_HEADER_FILE
fi
touch $LIB_HEADER_FILE

# Write the header of the header file
echo "#ifndef _${LIB_NAME}_H_" >> $LIB_HEADER_FILE
echo "#define _${LIB_NAME}_H_" >> $LIB_HEADER_FILE
echo "// C/C++ header file for library $LIB_NAME" >> $LIB_HEADER_FILE
echo "// generated on $DATE_TIME" >> $LIB_HEADER_FILE

for dir in $DIRECTORIES
do
    curDir=$LIB_NAME/$dir

	dirHeaderFile="$curDir.h"
	# Create directory header file
	if [ -e $dirHeaderFile ]; then
		rm $dirHeaderFile
	fi
	touch $dirHeaderFile

	echo "#ifndef _${LIB_NAME}_${dir}_H_" >> $dirHeaderFile
	echo "#define _${LIB_NAME}_${dir}_H_" >> $dirHeaderFile
	echo "// C/C++ header file for directory ${dir} of library $LIB_NAME" >> $dirHeaderFile
	echo "// generated on $DATE_TIME" >> $dirHeaderFile

	# Add the directory header file to the library header file
    echo "#include \"${dirHeaderFile}\"" >> $LIB_HEADER_FILE

    if [ -e $curDir ] && [ -d $curDir ]; then
        for file in $curDir/*.h
        do
            if [ -e $file ] && [ -f $file ]; then
                echo >> $OUT_FILE
                echo "// $file" >> $OUT_FILE
                output=$($M4 --define=SOURCE_FILE=$file -I $M4_INCLUDES descfile.m4)
                if (($? == 0)); then
                    echo "$output" >> $OUT_FILE
                else
                    echo "Error running M4 on file $file"
                fi

				# Add to directory header file
				echo "#include \"${file}\"" >> $dirHeaderFile
            fi
        done
        for file in $curDir/*.m4
        do
            if [ -e $file ] && [ -f $file ]; then
                echo >> $OUT_FILE
                echo "// $file" >> $OUT_FILE
                output=$($M4 --define=SOURCE_FILE=$file -I $M4_INCLUDES descfile.m4)
                if (($? == 0)); then
                    echo "$output" >> $OUT_FILE
                else
                    echo "Error running M4 on file $file"
                fi
            fi
        done
    fi

	# Add footer to directory header file
	echo "#endif//_${LIB_NAME}_${dir}_H_" >> $dirHeaderFile
done

# Write footer of description file
echo "#endif // _${LIB_NAME}_PGY_" >> $OUT_FILE

# Write footer of header file
echo "#endif//_${LIB_NAME}_H_" >> $LIB_HEADER_FILE
