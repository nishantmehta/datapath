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

# This script compiles a sub-library used by a DataPath library, and registers
# it with the system.
#
# Arguments:
#   $1 = DataPath Library name
#   $2 = Sub-Library name

if (($# != 2)); then
    echo "Usage:"
    echo "  make_sublib.sh lib_name sublib_name"
    echo
    echo "This script compiles a sub-library used by a DataPath library. The first argument"
    echo "should be the name of the DataPath library, and the second argument should be the"
    echo "name of the sub-library to be compiled."
    echo
    echo "There should exist a folder within lib_name called sublib_name, and it should"
    echo "contain a Makefile along with any files required to compile the library."
    echo "The Makefile should produce one or more .so files."
    exit 1
fi

if [ ! -d $1 ]; then
    echo "Error: $1 is not a valid directory! Please ensure that you have specified the"
    echo "correct name of the DataPath library."
    exit 1
fi

if [ ! -d $1/$2 ]; then
    echo "Error: $1/$2 is not a valid directory! Please ensure that you have specified the"
    echo "name of the sub-library correctly."
    exit 1
fi

if [ ! -e $1/$2/Makefile ] && [ ! -e $1/$2/makefile ]; then
    echo "Error: No makefile found!"
    exit 1
fi

TOP_DIR=$(pwd)
PKG_CONF_DIR=$TOP_DIR/pkgconfig
GLOBAL_LIB_DIR=$TOP_DIR/libraries

SUBLIB_DIR=$(readlink -f $1/$2)
NUM_OF_THREADS=$(cat /proc/cpuinfo | grep "processor" | wc -l);

PC_FILE=$PKG_CONF_DIR/$2.pc

cd $SUBLIB_DIR
make -j $NUM_OF_THREADS

if (($? != 0)); then
    echo "Error compiling sub-library!"
    exit 1
fi

if [ ! -e $PKG_CONF_DIR ]; then
    mkdir $PKG_CONF_DIR
fi

# Generate the pkgconfig file
if [ -e $PC_FILE ]; then
    rm $PC_FILE
fi
touch $PC_FILE

echo "Name: $2" >> $PC_FILE
echo "Description: Sub-library for DataPath library $1" >> $PC_FILE

# If the sublibrary has a VERSION file, use the contents as the version
if [ -f $SUBLIB_DIR/VERSION ]; then
    echo -n "Version: " >> $PC_FILE
    cat $SUBLIB_DIR/VERSION >> $PC_FILE
else
    echo "Version: 1.0" >> $PC_FILE
fi

# If the sublibrary requires other libraries, use them here
if [ -f $SUBLIB_DIR/REQUIRES ]; then
    echo -n "Requires: " >> $PC_FILE
    cat $SUBLIB_DIR/REQUIRES >> $PC_FILE
fi

if [ -f $SUBLIB_DIR/REQUIRES.PRIVATE ]; then
    echo -n "Requires.private: " >> $PC_FILE
    cat $SUBLIB_DIR/REQUIRES.PRIVATE >> $PC_FILE
fi

# If the sublibrary has an include directory, add that to the Cflags
if [ -d $SUBLIB_DIR/include ]; then
    echo "Cflags: -I$SUBLIB_DIR/include" >> $PC_FILE
fi

# Add all of the .so and .a files to the Libs
echo -n "Libs: -L$GLOBAL_LIB_DIR" >> $PC_FILE

for lib in *.so *.a
do
    if [ -e $lib ]; then
        # Link the library to the global libraries directory
        ln -sf $(readlink -f $lib) $GLOBAL_LIB_DIR/$lib
        NAMESPEC=$(echo $lib | sed -r "s/(lib)?([a-zA-Z0-9]+).(so|a)/\2/")
        echo -n " -l$NAMESPEC" >> $PC_FILE
    fi
done

echo >> $PC_FILE
