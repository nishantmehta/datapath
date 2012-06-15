#!/bin/bash

# This script initializes the system and prepares it for the main
# executable to run. The primary function is to create the right
# tables in the SQLite database so subsequent queries have the right
# relations created. The script is interactive since it is used
# infrequently. A similar script will be written to allow
# initialization from a configuration file.

# Note: This script needs to be run before AddDiskArray script

# Tables created:
# ---------------
# 	DiskArray -- info on the disk array such as parameters, etc.
# 		pageSize:int, stripeParam{1,2}:int, HDNo:int, firstFreePage:int
# 	Stripes -- info on the files making up the stripe
# 		fileName:text
# 	Relations -- info on relations such as name, etc.
# 		relID:int, relName:text,  numColumns:int, lastChunkID:int
# 	Chunks -- info on chunks (what relation they belong to, etc.)
# 		chunkID:int, relID:int, numTuples:int
# 	Columns -- info on columns/chunk (storage)
# 		relID:int, chunkID:int, startPage:int, sizeInPages:int, columnType:int,
#			varStartPage:int


META_FILE=$1

echo "Welcome to Datapath Setup."
echo ""
echo "This script creates a medatada database that keeps all the metadata "
echo "DataPath uses during execution. This file needs to be created before "
echo "the system is started."
echo ""

echo "We use $META_FILE as the metadata file."

if [ -s $META_FILE ]; then
	echo "The file $META_FILE already exists."
	echo "Erase the old metadata file: $META_FILE."
	rm -f $META_FILE
fi

sqlite3 $META_FILE <<EOF
	CREATE TABLE  DiskArrays (
		/* name must be null for the default array */
		name					TEXT,
		arrayID					INTEGER				PRIMARY KEY,
		pageSize 				INTEGER,
		stripeParam1				INTEGER				NOT NULL,
		stripeParam2				INTEGER				NOT NULL,
		firstFreePage				INTEGER				NOT NULL
	);

	CREATE TABLE Stripes (
		/* diskID is INT not INTEGER since it cannot be 0 */
		/* arrayID is a foreign key to DiskArrays table */
		diskID					INT,
		arrayID					INTEGER,
		fileName				TEXT
	);


	CREATE TABLE Relations (
		/* arrayID is a foreign key to DiskArrays table (array that stores the relation) */
		/* relName connects the storage with the catalog */
		relID					INTEGER				PRIMARY KEY,
		arrayID					INTEGER,
		relName					TEXT				NOT NULL,
		numColumns				INTEGER				NOT NULL,
		freeChunkId				INTEGER				NOT NULL
	);

	CREATE TABLE Chunks (
		/* numbering starts at 0 */
		chunkID					INTEGER				NOT NULL,
		relID					INTEGER				NOT NULL,
		numTuples				INTEGER				NOT NULL
	);

	CREATE TABLE Columns (
		colNo					INTEGER				NOT NULL,
		relID					INTEGER				NOT NULL,
		chunkID					INTEGER				NOT NULL,
		startPage				INTEGER				NOT NULL,
		sizeInPages				INTEGER				NOT NULL,
		columnType				INTEGER				NOT NULL,
		varStartPage				INTEGER				NOT NULL
	);


	/* Now the catalog */

	CREATE TABLE CatalogRelations (
		/* relation is the relation name (same as relName in table Relations)*/
		/* relID is an id for the catalog only (not related with table Relations) */
		/* size contains statistics of the relation */
		relation				TEXT,
		relID					INTEGER				PRIMARY KEY,
		size					INTEGER
	);

	CREATE TABLE CatalogAttributes (
		/* physical layout column */
		/* type is the system type of the attribute */
		colNo					INTEGER				PRIMARY KEY,
		relID					INTEGER,
		type					TEXT,
		attribute				TEXT,
		uniqueVals				INTEGER
	);

	CREATE TABLE CatalogConstants (
		name					TEXT,
		value					TEXT,
		type					TEXT
	);
EOF


echo "CHECK THE CORRECTNESS OF THE FILE. DUMP DATABASE CONTENT:"

sqlite3 $META_FILE <<EOF
.echo ON
.schema
EOF
