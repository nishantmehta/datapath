#!/bin/bash

# Script to add a disk array. At least one disk array should be added
# to a medatada file before it can be used by DataPath.

# The disk array is characterized by:
# 1. name -- has to be unique in the system. First array is the default array
# 2. striping parameters -- control the hash function used for data partitioning
# 3. page size -- most important parameter. 256KB -- 1MB are good values on all systems
# 4. partition files -- files/partitions where the actual data is stored
#
# The stripes are grown until failure
# This script generates automatically the striping parameters


META_FILE=$1
ARRAYNAME=$2
PAGESIZE=$3
NUM_STRIPES=$4
STRIPES_PATTERN=$5
STRIPES_BASE=$6

echo "This script needs to update the main metadata repository of the system."

echo "We use $META_FILE as the metadata file."

if [ -s $META_FILE ]; then
	# file exists, test that it has the correct tables
	echo "Metadata file found!"
else
	# file does not exist
	echo "Could not find metadata file $META_FILE."
	echo "Please create it by running InitializeSystem.sh first."
	exit -1
fi


echo "Each disk array has a name. The name must be empty for the default disk array."

echo "The disk array uses a universal page size."
echo "The page size is the most important parameter of the disk array."
echo "Please pick a page size that masks the seek time (i.e., at least average_seek_time*transfer_rate)."
echo "Page size must be a mulltiple of 4KB and be expressed in KB."

PAGESIZE_VERB=$((PAGESIZE/4*4096))

stripeParam1=$((RANDOM*RANDOM))
stripeParam2=$((RANDOM*RANDOM))

echo "Disk array parameters:"
echo "	Page size = $PAGESIZE_VERB"
echo "	Striping parameters = ($stripeParam1, $stripeParam2)"

# test if the array was previously defined
cnt=`sqlite3 $META_FILE "SELECT COUNT(*) FROM DiskArrays WHERE name='$ARRAYNAME';"`
if [ $cnt != "0" ]; then
	# the disk array already exists
	echo "The disk array $ARRAYNAME already exists. Delete it first or create one with a different name."
	exit -1
fi

sqlite3 $META_FILE <<EOF
INSERT INTO DiskArrays(name, pageSize, stripeParam1, stripeParam2, firstFreePage)
VALUES ('$ARRAYNAME', $PAGESIZE_VERB, $stripeParam1, $stripeParam2, 0);
EOF

# the id of the array we just created
ARRAYID=`sqlite3 $META_FILE "SELECT arrayID FROM DiskArrays WHERE name='$ARRAYNAME';"`

echo "DataPath likes to use as many hard-disks as you have available."
echo "The disks are called stripes and the data is spread across them uniformly."
echo "The stripes are organized into disk arrays and there is always a default disk array."

for i in `seq 1 $NUM_STRIPES`; do
	let stripeNo=$STRIPES_BASE+$i-1
	my_file="`printf $STRIPES_PATTERN$stripeNo`"
	if [ ! -e $my_file ]; then
		# file does not exist; create it
		touch $my_file
	fi

	echo "Create stripe $my_file."

	let diskID=$i-1

sqlite3 $META_FILE <<EOF
INSERT INTO Stripes(diskID, arrayID, fileName) VALUES ($diskID, $ARRAYID, '$my_file');
EOF

done

echo "CHECKING THE CORRECTNESS OF THE FILE. DUMPING DATABASE CONTENT:"
sqlite3 $META_FILE <<EOF
.echo ON
select * from DiskArrays;
select * from Stripes;
EOF
