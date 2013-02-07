#!/bin/bash
# Copyright 2013 Christopher Dudley
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Uses pkg-config to determine the existence of required libraries and
# get the correct include and link information for them.

# Note: this script will be executed in the context of the Generated directory

# $1 = file containing the library names, one per line.

prevIFS="$IFS"
IFS=":"

LIBS_PKGCONFIG=""

# check datapath global pkgconfig directories
for path in $DATAPATH_PKGCONFIG_PATH; do
    CANONICAL_PATH=$(readlink -f $path)
    if [ $? == 0 ]; then
        LIBS_PKGCONFIG+=":$CANONICAL_PATH"
    fi
done

# Check for pkgconfig directories in libraries
for path in $DATAPATH_LIBRARY_PATH; do
    CANONICAL_PATH=$(readlink -f $path/pkgconfig)
    if [ $? == 0 ]; then
        LIBS_PKGCONFIG+=":$CANONICAL_PATH"
    fi
done

IFS="$prevIFS"

export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}${LIBS_PKGCONFIG}

for lib in $(cat $1)
do
    pkg-config --exists $lib
    if (($? == 0));
    then
        echo  $(pkg-config --libs $lib)
    else
        echo "-l$lib"
    fi
done
