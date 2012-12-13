#!/bin/bash

# Uses pkg-config to determine the existence of required libraries and
# get the correct include and link information for them.

# Note: this script will be executed in the context of the Generated directory

# $1 = file containing the library names, one per line.

LIBS_PKGCONFIG=$(readlink -f ../Libs/pkgconfig)
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${LIBS_PKGCONFIG}

for lib in $(cat $1)
do
    pkg-config --exists $lib
    if (($? == 0));
    then
        echo  $(pkg-config --cflags --libs $lib)
    else
        echo "-l$lib"
    fi
done
