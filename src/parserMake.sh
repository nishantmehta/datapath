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
# antlr3 executable

if [ $# -eq 1 ]
then
        DEBUG=-debug
        echo "Using debugging "
fi

# what are the grammars to compile
FILES="BaseLexer.g DataPath.g Piggy.g DPtree.g"
ANTLR="java -jar /usr/local/lib/antlr-3.3-complete.jar"
#ANTLR="java -jar /usr/local/lib/antlr-3.4-complete-no-antlrv2.jar"

cd "LemonTranslator/parser/"

# compile the grammars
for gram in $FILES; do
        echo "Compiling $gram via" "$ANTLR $gram $DEBUG"
        $ANTLR $DEBUG $gram
done

# move the .h and .cc files in place
for f in *.h; do mv $f ../headers; done
for f in *.c; do mv $f ../source/${f}c;  done

cd "../../"
