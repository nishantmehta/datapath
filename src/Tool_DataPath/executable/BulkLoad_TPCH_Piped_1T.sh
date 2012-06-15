#!/bin/bash
#
#  Copyright 2012 Alin Dobra and Christopher Jermaine
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
#

# The arguments to the script are
# $1: the name of the relation to be loaded
# $2: the number of fragments the generator produces
#     has to be the same as in relationname.m4
# $3: the option to dbgen that produced the desired relation
#     the option for lineitem is L
# $4: the scale factor (1 is 1GB)

REL_NAME=$1
NUM_FRAGMENTS=$2
TPCH_OPTION=$3

killall -9 dbgen

echo "Create the named pipes to talk to dbgen."
for i in `seq $2`; do
    rm -f $REL_NAME.tbl.$i
    mkfifo $REL_NAME.tbl.$i
done

echo "Starting the TPCH data generator."
./dbgen -C $NUM_FRAGMENTS -s 1000 -T $TPCH_OPTION > TPCH-OUT&
echo "./dbgen -C $NUM_FRAGMENTS -s 1000 -T $TPCH_OPTION"

echo "Now you can start the bulk loader. (as root)"
#./bulkLoader > OUTPUT &
