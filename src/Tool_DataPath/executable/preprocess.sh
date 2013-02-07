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

# $1 = input file
# $2 = output file

INC_DIRS=""

prevIFS="$IFS"
IFS=":"
for path in $DATAPATH_LIBRARY_PATH; do
    canonicalPath=$(readlink -f $path)
    if [ $? == 0 ]; then
        INC_DIRS+=" -I $canonicalPath"
    fi
done
IFS="$prevIFS"

cpp -C -P $INC_DIRS -include base.pgy $1 > $2
