#!/bin/bash

# Get System Information

# Old versions of NUM_OF_PROCS and NUM_OF_THREADS
#NUM_OF_PROCS=$(cat /proc/cpuinfo | grep processor | wc -l);
#NUM_OF_THREADS=$(cat /proc/cpuinfo | grep "cpu cores" | awk '{SUM += $ 4} END {print SUM}');

# In order to find the number of physical processors, you need to find the
# number of distinct physical ids.
NUM_OF_PROCS=$(cat /proc/cpuinfo | grep "physical id" | awk 'BEGIN {MAX = -1} {if ($4 > MAX ) MAX = $ 4} END {print (MAX + 1)}');

# Note: the iine below actually finds the number of threads, not the number of
# physical cores.
NUM_OF_THREADS=$(cat /proc/cpuinfo | grep "processor" | wc -l);

# The line below uses the program in find_cores.awk to find the number of
# physical cores, if there is ever any use for this knowledge.
NUM_OF_CORES=$(cat /proc/cpuinfo | awk -f find_cores.awk);

TOTAL_MEMORY=$(cat /proc/meminfo | grep MemTotal | awk '{print $ 2}');
FREE_MEMORY=$(cat /proc/meminfo | grep MemFree | awk '{print $ 2}');

#echo $LINE_NUMBER, $NUM_OF_PROCS, $NUM_OF_THREADS, $TOTAL_MEMORY, $FREE_MEMORY

if [ "$1" = "make" ]; then
    # Compile DataPath using Makefile
    echo "Compiling DataPath using Makefile"
    echo
    sleep 2
    make clean; make -j$NUM_OF_THREADS
    exit 1
fi


#otherwise

#Helper functions
GetTwosExponent(){
    for((i = $1; i <= $2; i++))
    do
    if [ "$i" -ne 0 ] &&  [ $(($i & $i - 1)) -eq 0 ]; then
            echo "$i"
            break
    fi
    done
}

# check for libraries if not found, ask user to install them

# Prepare Constants.h.m4 file
cp Constants.h.m4 CONSTANTS_M4;

DEFAULT_NUM_SEGS=$(GetTwosExponent $(($NUM_OF_THREADS*2)) $(($NUM_OF_THREADS*3)))
DEFAULT_NUM_OTHER=$(($NUM_OF_THREADS*3/2))
DEFAULT_NUM_SLOTS=24

# Get User Defined Settings, if any
if [ ! -e configure ]; then
    cp configure.example configure
fi

UD_NUM_SLOTS=$(grep 'USER_DEFINED_NUM_SLOTS_IN_SEGMENT_BITS' configure | awk '{if (index($0,"//") == 0) str=substr($0,index($0,"=")+1); else str=-1; print str}');
UD_NUM_SEGS=$(grep 'USER_DEFINED_NUM_SEGS' configure | awk '{if (index($0,"//") == 0) str=substr($0,index($0,"=")+1); else str=-1; print str}');
UD_NUM_EETHREADS=$(grep 'USER_DEFINED_NUM_EXEC_ENGINE_THREADS' configure | awk '{if (index($0,"//") == 0) str=substr($0,index($0,"=")+1); else str=-1; print str}');
UD_NUM_DISK_TOKENS=$(grep 'USER_DEFINED_NUM_DISK_TOKENS' configure | awk '{if (index($0,"//") == 0) str=substr($0,index($0,"=")+1); else str=-1; print str}');

#echo $UD_NUM_SEGS $UD_NUM_SLOTS $UD_NUM_EETHREADS $UD_NUM_DISK_TOKENS

#Set NUM_SLOTS_IN_SEGMENT_BITS
if [ $UD_NUM_SLOTS -le 0 ]; then
    echo "Using default NUM_SLOTS_IN_SEGMENT_BITS = $DEFAULT_NUM_SLOTS"
    sed "/#define NUM_SLOTS_IN_SEGMENT_BITS/c #define NUM_SLOTS_IN_SEGMENT_BITS $DEFAULT_NUM_SLOTS" -i CONSTANTS_M4
else
    if [ $UD_NUM_SLOTS -gt 0 ] && [ $UD_NUM_SLOTS -le 24 ]; then
    echo "Using NUM_SLOTS_IN_SEGMENT_BITS = $UD_NUM_SLOTS"
    sed "/#define NUM_SLOTS_IN_SEGMENT_BITS/c #define NUM_SLOTS_IN_SEGMENT_BITS $UD_NUM_SLOTS" -i CONSTANTS_M4
    else
    echo "Warning: NUM_SLOTS_IN_SEGMENT_BITS should not be over 24 bits"
    echo "Do you want to continue with NUM_SLOTS_IN_SEGMENT_BITS = $UD_NUM_SLOTS? y/n"
    read resp
    if [ "$resp" = "n" -o "$resp" = "N" ]; then
        echo "Aborting compiling DataPath."
        exit 1
    fi
    echo "Using NUM_SLOTS_IN_SEGMENT_BITS = $UD_NUM_SLOTS"
    sed "/#define NUM_SLOTS_IN_SEGMENT_BITS/c #define NUM_SLOTS_IN_SEGMENT_BITS $UD_NUM_SLOTS" -i CONSTANTS_M4
    fi
fi

#Set NUM_SEGS
if [ $UD_NUM_SEGS -le 0 ]; then
    echo "Using default NUM_SEGS = $DEFAULT_NUM_SEGS"
    sed "/#define NUM_SEGS/c #define NUM_SEGS $DEFAULT_NUM_SEGS" -i CONSTANTS_M4
else
    if [ $UD_NUM_SEGS -ge $(($NUM_OF_THREADS*2)) ] && [ $UD_NUM_SEGS -le $(($NUM_OF_THREADS*3)) ]; then
    echo "Using NUM_SEGS = $UD_NUM_SEGS"
    sed "/#define NUM_SEGS/c #define NUM_SEGS $UD_NUM_SEGS" -i CONSTANTS_M4
    else
    echo "Warning: For this system, NUM_SEGS should be between $(($NUM_OF_THREADS*2)) and $(($NUM_OF_THREADS*3)), ideally $DEFAULT_NUM_SEGS"
    echo "Do you want to continue with NUM_SEGS = $UD_NUM_SEGS? y/n"
    read resp
    if [ "$resp" = "n" -o "$resp" = "N" ]; then
        echo "Aborting compiling DataPath."
        exit 1
    fi
    echo "Using NUM_SEGS = $UD_NUM_SEGS"
    sed "/#define NUM_SEGS/c #define NUM_SEGS $UD_NUM_SEGS" -i CONSTANTS_M4
    fi
fi

#Set NUM_EXEC_ENGINE_THREADS and MAX_CLEANER_CPU_WORKERS
if [ $UD_NUM_EETHREADS -le 0 ] || [ $UD_NUM_EETHREADS -eq $DEFAULT_NUM_OTHER ]; then
    echo "Using default NUM_EETHREADS = $DEFAULT_NUM_OTHER"
    sed "/#define NUM_EXEC_ENGINE_THREADS/c #define NUM_EXEC_ENGINE_THREADS $DEFAULT_NUM_OTHER" -i CONSTANTS_M4
    sed "/#define MAX_CLEANER_CPU_WORKERS/c #define MAX_CLEANER_CPU_WORKERS $DEFAULT_NUM_OTHER" -i CONSTANTS_M4
else
    echo "Warning: For this system, NUM_EXEC_ENGINE_THREADS should be $DEFAULT_NUM_OTHER)"
    echo "Do you want to continue with NUM_EXEC_ENGINE_THREADS = $UD_NUM_EETHREADS? y/n"
    read resp
    if [ "$resp" = "n" -o "$resp" = "N" ]; then
    echo "Aborting compiling DataPath."
    exit 1
    fi
    echo "Using NUM_EXEC_ENGINE_THREADS = $UD_NUM_EETHREADS"
    sed "/#define NUM_EXEC_ENGINE_THREADS/c #define NUM_EXEC_ENGINE_THREADS $UD_NUM_EETHREADS" -i CONSTANTS_M4
    sed "/#define MAX_CLEANER_CPU_WORKERS/c #define MAX_CLEANER_CPU_WORKERS $UD_NUM_EETHREADS" -i CONSTANTS_M4
fi

#Set NUM_DISK_TOKENS and MAX_CLEANER_DISK_REQUESTS
if [ $UD_NUM_DISK_TOKENS -le 0 ] || [ $UD_NUM_DISK_TOKENS -eq $DEFAULT_NUM_OTHER ]; then
    echo "Using default NUM_DISK_TOKENS = $DEFAULT_NUM_OTHER"
    sed "/#define NUM_DISK_TOKENS/c #define NUM_DISK_TOKENS $DEFAULT_NUM_OTHER" -i CONSTANTS_M4
    sed "/#define MAX_CLEANER_DISK_REQUESTS/c #define MAX_CLEANER_DISK_REQUESTS $DEFAULT_NUM_OTHER" -i CONSTANTS_M4

else
    echo "Warning: For this system, NUM_DISK_TOKENS should be $DEFAULT_NUM_OTHER"
    echo "Do you want to continue with NUM_DISK_TOKENS = $UD_NUM_DISK_TOKENS? y/n"
    read resp
    if [ "$resp" = "n" -o "$resp" = "N" ]; then
    echo "Aborting compiling DataPath."
    exit 1
    fi
    echo "Using NUM_DISK_TOKENS = $UD_NUM_DISK_TOKENS"
    sed "/#define NUM_DISK_TOKENS/c #define NUM_DISK_TOKENS $UD_NUM_DISK_TOKENS" -i CONSTANTS_M4
    sed "/#define MAX_CLEANER_DISK_REQUESTS/c #define MAX_CLEANER_DISK_REQUESTS $UD_NUM_DISK_TOKENS" -i CONSTANTS_M4
fi


#cat CONSTANTS_M4
#grep 'USER_DEFINED_' configure |  \
#awk \
#'{print "m4_define(</"$1"/>,</"$NF"/>)"}' \
#> userdefined;

#sed "$LINE_NUMBER r userdefined" -i  CONSTANTS_M4;

mv CONSTANTS_M4 Global/m4/Constants.h.m4

echo
# Clean old stuff
if [ -f maker ]; then
    if [ executables.lemon -nt Makefile ]; then
        #Compile maker.cc
    echo "An updated executables.lemon file is found."
    sleep 2

    echo "Creating Makefile"
    echo
    # Prepare Makefile using maker
    sleep 2
   ./maker executables.lemon Makefile_prelude
    fi
else
    #Compile maker.cc
    echo "No maker exists. Compiling maker.cc"
    echo
    sleep 2
    g++ -o maker maker.cc
    # Prepare Makefile using maker
    ./maker executables.lemon Makefile_prelude

fi

# Compile DataPath using Makefile
    echo "Compiling DataPath using Makefile"
    echo
    sleep 2
    make clean; make -j$NUM_OF_THREADS

exit 0
