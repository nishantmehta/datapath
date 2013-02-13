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

# $1 = base of the file names
# Note: this file is run in the context of the executable folder

prevIFS="$IFS"
IFS=":"

for path in $DATAPATH_M4_PATH; do
    canonicalPath=$(readlink -f $path)
    if [ $? == 0 ]; then
        M4INCLUDES+=" -I $canonicalPath"
    fi
done

for path in $DATAPATH_LIBRARY_PATH; do
    canonicalPath=$(readlink -f $path)
    if [ $? == 0 ]; then
        M4INCLUDES+=" -I $canonicalPath"
    fi
done

IFS="$prevIFS"

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
