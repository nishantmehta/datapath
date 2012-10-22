#!/bin/bash

# Uses pkg-config to determine the existence of required libraries and
# get the correct include and link information for them.

# $1 = file containing the library names, one per line.

for lib in $(cat $1)
do
    if [$(pkg-config --exists $lib)];
    then
        echo  $(pkg-config --cflags --libs $lib)
    else
        exit 1;
    fi
done
