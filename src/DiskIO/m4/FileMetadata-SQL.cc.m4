dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "FileMetadata.h"
#include "Errors.h"
#include "MetadataDB.h"
#include "DiskArray.h"
#include "Constants.h"
#include <set>

using namespace std;

dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

// ======================== FileMetadata class definitions ===================

FileMetadata::FileMetadata(const char *_relName, int _numCols):
	chkFilled(-1), modified(false) {

	relName = strdup(_relName);
	newRelation=false; // we change it latter if not found

	SQL_OPEN_DATABASE( GetMetadataDB() );

	// Create all tables we need if not already existing
  SQL_STATEMENTS_NOREZ(</"
    /* Relations */
    CREATE TABLE IF NOT EXISTS Relations (
      relID          INTEGER,
      arrayID        INTEGER,
			relName        TEXT,
			numColumns     INTEGER,
			freeChunkId    INTEGER
    );

    /* Chunks */
    CREATE TABLE IF NOT EXISTS Chunks (
      /* numbering starts at 0 */
      chunkID        INTEGER NOT NULL,
      relID          INTEGER NOT NULL,
      numTuples      INTEGER NOT NULL
    );

    /* Columns */
    CREATE TABLE IF NOT EXISTS Columns (
      colNo          INTEGER NOT NULL,
      relID          INTEGER NOT NULL,
      chunkID        INTEGER NOT NULL,
      startPage      INTEGER NOT NULL,
      sizePages      INTEGER NOT NULL,
      sizeBytes      INTEGER NOT NULL,
      startPageCompr INTEGER NOT NULL,
      sizePagesCompr INTEGER NOT NULL,
      sizeBytesCompr INTEGER NOT NULL
    );

    /* Fragment */
    CREATE TABLE IF NOT EXISTS Fragment(
			relID           INTEGER,
			chunkID         INTEGER,
			colNo           INTEGER,
      endPos          INTEGER
    );

    /* FragmentTuples */
    CREATE TABLE IF NOT EXISTS FragmentTuples(
      tuples        INTEGER, /* number of tuples */
			relID           INTEGER,
			chunkID         INTEGER
    );

  "/>);

	// first decide if this relation is new or old
	SQL_STATEMENT_SCALAR(</"
		SELECT COUNT(*)
		FROM Relations
		WHERE relName='%s';
	"/>, haveRel, int, </relName/>);

	if (haveRel == 1 ) { // existing relation

 		SQL_STATEMENT_TABLE(</"
  		SELECT relID, numColumns, freeChunkID
	  	FROM Relations
		  WHERE relName='%s';
  	"/>, </(_relID_,int), (_numCols_,int), (_numChunks_,int)/>,
												</relName/>){
			relID = _relID_;
			numCols = _numCols_;
			numChunks = _numChunks_;
		}SQL_END_STATEMENT_TABLE;

		FATALIF( numCols != _numCols, "Did not indicate the correct number of columns"
						 "\nProbably trying to write the wrong relation\n");

		chunkMetaD.resize(numChunks);
		
		// fill datastructures for all chunks
		SQL_STATEMENT_TABLE(</"
  	  SELECT chunkID, numTuples
	    FROM Chunks
 		  WHERE relID=%d;
    "/>, </(_chunkID, int), (_numTuples,int)/>,
											</relID/>){
			chunkMetaD[_chunkID].Initialize(numCols, _numTuples);
		}SQL_END_STATEMENT_TABLE;

		// fill datastructures for all columns
		// for each column, populate the datastructures
		SQL_STATEMENT_TABLE(</"
		  SELECT chunkID, colNo, startPage, sizePages, sizeBytes, startPageCompr, sizePagesCompr, sizeBytesCompr
  		FROM Columns
	  	WHERE relID=%d;
  	"/>, </(_chunkID2, int), (_colNo, int), (_startPage,int), (_sizePages,int), (_sizeBytes,int), (_startPageCompr,int), (_sizePagesCompr,int), (_sizeBytesCompr,int)/>,
												</relID/>){
			chunkMetaD[_chunkID2].colMetaData[_colNo].Initialize(_startPage, _sizePages, _sizeBytes,
																													 _startPageCompr, _sizePagesCompr,
																													 _sizeBytesCompr);
		}SQL_END_STATEMENT_TABLE;

		SQL_STATEMENT_TABLE(</"
      SELECT chunkID, colNo, endPos
      FROM Fragment
	  	WHERE relID=%d
      ORDER BY chunkID, colNo, endPos;
    "/>, </(_chunkID3, int), (_colNo2, int), (endPos, int)/>,
												</relID/>){
			chunkMetaD[_chunkID3].colMetaData[_colNo2].fragments.Initialize(endPos);
		}SQL_END_STATEMENT_TABLE;

	} else { // new relation
	  newRelation=true;
	  numChunks = 0;
	  numCols = _numCols;

	  
	  // get a new relation id
	  SQL_STATEMENT_SCALAR(</"
	  	SELECT MAX(relID)
		  FROM Relations;
  	"/>, _relID, int);
	  
	  relID=_relID+1; // new id
	  

	}
	
	// close the database
	SQL_CLOSE_DATABASE;
}

void FileMetadata::Print(void){
  cout << "\nChunk information";
  for (int chunkit = 0; chunkit < chunkMetaD.size(); chunkit++) {
    printf("\n Chunkit = %d,   relID = %d,    Numtuples = %d", chunkit, relID, chunkMetaD[chunkit].numTuples);
  }
  
  for (int chunkit = 0; chunkit < chunkMetaD.size(); chunkit++) {
    for (int colit = 0; colit < chunkMetaD[chunkit].colMetaData.size(); colit++) {
      printf("\n Chunkit = %d, colit = %d, relID = %d, Startpage = %d, size pages = %d, sizeBytes = %d, startpageCompr = %d, sizePageCompr = %d, sizeBytesCompr = %d",
	     chunkit, colit, relID, chunkMetaD[chunkit].colMetaData[colit].startPage, chunkMetaD[chunkit].colMetaData[colit].sizePages, chunkMetaD[chunkit].colMetaData[colit].sizeBytes, chunkMetaD[chunkit].colMetaData[colit].startPageCompr, chunkMetaD[chunkit].colMetaData[colit].sizePagesCompr, chunkMetaD[chunkit].colMetaData[colit].sizeBytesCompr); fflush(stdout);
    }
  }
}

void FileMetadata::Flush(void) {
	// to write the informatinon we use write system call
	// since it allows any amout of information to be writen
	if (!modified)
		return;

	PDEBUG("FileMetadata::Flush()");

#ifdef DEBUG_FILEMETADATA
	Print();
#endif 

	SQL_OPEN_DATABASE( GetMetadataDB() );

	// if relation is new, first write the entry in relations
	if (newRelation){
	  cout << "CREATING A NEW RELATION" << endl;
	  
	  DiskArray& diskArray = DiskArray::GetDiskArray();
	  int arrayID = diskArray.getArrayID();
	  
	  SQL_STATEMENTS_NOREZ(</"
		  INSERT INTO Relations(relID, arrayID, relName, numColumns, freeChunkID)
  		VALUES (%d, %d, '%s', %d, 0);
	  "/>,</relID, arrayID, relName, numCols/>);
	}

	// update the freeChunkID of the relation
	SQL_STATEMENTS_NOREZ(</"
		UPDATE Relations SET freeChunkID=%ld
		WHERE relName='%s';
	"/>,</(unsigned long)numChunks, relName/>);

	// delete all previous info on this relation
	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM Chunks
		WHERE relID=%d;
	"/>,</relID/>);

	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM Columns
		WHERE relID=%d;
	"/>,</relID/>);

	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM Fragment
		WHERE relID=%d;
	"/>,</relID/>);

	SQL_STATEMENTS_NOREZ(</"
		DELETE FROM FragmentTuples
		WHERE relID=%d;
	"/>,</relID/>);

	// Now flush all chunk info first
	SQL_STATEMENT_PARAMETRIC_NOREZ(</"
		INSERT INTO Chunks(chunkID, relID, numTuples) VALUES (?1, ?2, ?3);
		"/>, </int, int, int/>);
			for (int chunkit = 0; chunkit < chunkMetaD.size(); chunkit++) {
				SQL_INSTANTIATE_PARAMETERS(chunkit, relID, chunkMetaD[chunkit].numTuples);
			}
	SQL_PARAMETRIC_END;

	// Now flush all column info
	SQL_STATEMENT_PARAMETRIC_NOREZ(</"
		INSERT INTO Columns(colNo, chunkID, relID, startPage, sizePages, sizeBytes, startPageCompr, sizePagesCompr, sizeBytesCompr)
		VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9);
		"/>, </int, int, int, int, int, int, int, int, int/>);
			for (int chunkit = 0; chunkit < chunkMetaD.size(); chunkit++) {
				for (int colit = 0; colit < chunkMetaD[chunkit].colMetaData.size(); colit++) {
					SQL_INSTANTIATE_PARAMETERS(colit, chunkit, relID, chunkMetaD[chunkit].colMetaData[colit].startPage,
							chunkMetaD[chunkit].colMetaData[colit].sizePages, chunkMetaD[chunkit].colMetaData[colit].sizeBytes,
							chunkMetaD[chunkit].colMetaData[colit].startPageCompr, chunkMetaD[chunkit].colMetaData[colit].sizePagesCompr, 
							chunkMetaD[chunkit].colMetaData[colit].sizeBytesCompr);
				}
			}
	SQL_PARAMETRIC_END;

	// Now flush all fragment info for each column
	SQL_STATEMENT_PARAMETRIC_NOREZ(</"
			INSERT INTO Fragment(relID, chunkID, colNo, endPos) VALUES (?1, ?2, ?3, ?4);
			"/>, </int, int, int, int/>);
			for (int chunkit = 0; chunkit < chunkMetaD.size(); chunkit++) {
				for (int colit = 0; colit < chunkMetaD[chunkit].colMetaData.size(); colit++) {
					// iterate through ranges
					for (int i = 0; i < chunkMetaD[chunkit].colMetaData[colit].fragments.startPositions.size(); i++) {
						SQL_INSTANTIATE_PARAMETERS(relID, chunkit, colit, chunkMetaD[chunkit].colMetaData[colit].fragments.startPositions[i]);
					}
				}
			}
  SQL_PARAMETRIC_END;

	// now ask the diskArray to flush as well
	DiskArray& diskArray = DiskArray::GetDiskArray();
	diskArray.Flush(SQL_DATABASE_OBJECT);

	// and that is about all
	SQL_CLOSE_DATABASE;

	modified=false;
}
