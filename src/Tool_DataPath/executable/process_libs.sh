#!/bin/bash

# Uses pkg-config to determine the existence of required libraries and
# get the correct include and link information for them.

# $1 = file containing the library names, one per line.

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:../../../../Libs/pkgconfig

for lib in $(cat $1)
do
    if `pkg-config --exists $lib`;
    then
        echo  $(pkg-config --cflags --libs $lib)
    else
	echo "-l$lib"
    fi
done
