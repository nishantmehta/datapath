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

prevIFS="$IFS"
IFS=":"

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

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${LIBS_PKGCONFIG}

for file in $*; do
    for lib in $(cat $file); do
        pkg-config --exists $lib
        if [ $? == 0 ]; then
            echo $(pkg-config --cflags $lib)
        fi
    done
done
