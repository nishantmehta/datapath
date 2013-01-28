#!/bin/bash

LIBS_PKGCONFIG=$(readlink -f ../Libs/pkgconfig)
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${LIBS_PKGCONFIG}

for file in $*; do
    for lib in $(cat $file); do
        pkg-config --exists $lib
        if [ $? == 0 ]; then
            echo $(pkg-config --cflags $lib)
        fi
    done
done
