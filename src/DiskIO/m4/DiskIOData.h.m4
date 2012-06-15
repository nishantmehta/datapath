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
include(DataFunctions.m4)

#ifndef DISK_IO_DATA_
#define DISK_IO_DATA_

#include "Data.h"
#include "TwoWayList.h"
#include "Column.h"
#include "EEExternMessages.h"

// the base class of the hierarchy
/** The common parts are:
		
		startPage: the starting page on the disk where data is read/written
		sizePages: the size of the column in pages
*/
		
M4_CREATE_BASE_DATA_TYPE(DiskData, Data, 
  </(startPage, off_t), (sizePages, off_t) />, <//>)


/** Used by the ChunkReaderWriter to get a description of 
		the column content to write on the disk. 

    Arguments:
		  slot: which column is this about
			sizePages:  size in number of pages
			startPage: the start of the column on disk

			Sizes in bytes:

			sizeCompressed: if !=0,  the column is compressed
			sizeUncompressed;
			
*/

M4_CREATE_DATA_TYPE(ColReadDescription, DiskData,
 	</(slot, int), (sizeCompressed, off_t), (sizeUncompressed, off_t)/>,
  <//>)

typedef TwoWayList< ColReadDescription > ColReadDescContainer;

/** Used by ChunkReaderWriter to get instruction of what columns to
    write

		Arguments:
		  	column: the column to be written
				off_t startPageCompr; // the first page where the compressed part goes
				off_t sizePagesCompr; // the size, in pages of the compressed part

*/

M4_CREATE_DATA_TYPE(ColWriteDescription, DiskData,
  </(startPageCompr, off_t), (sizePagesCompr, off_t)/>, </(column, Column)/>)

typedef TwoWayList< ColWriteDescription > ColWriteDescContainer;


/** These are requests going to the disk. The pageSize is the MMap page
		The requests are sent to the DiskArray who sends them to the HDThreads.

		Arguments:
		  memLoc: the memory location where the read/write is done

*/
M4_CREATE_DATA_TYPE(DiskRequestData, DiskData,
	</(memLoc, void*)/>, <//>)

typedef TwoWayList< DiskRequestData > DiskRequestDataContainer;
		
/** Internal datatype used by the ChunkReaderWriter to keep track of requests
		
		Arguments:
				chunkID: id of the chunk we are dealing with
				hMsg: the hopping message to send back to EE
				token: the token to send back
*/
M4_CREATE_DATA_TYPE(CRWRequest, Data,
  </(chunkID, off_t)/>, 
  </(hMsg, HoppingDataMsg), (token, GenericWorkToken)/>)

typedef Keyify<off_t> KOff_t; // keyified off_t
typedef EfficientMap< KOff_t, CRWRequest > IDToRequestMap;

#endif // DISK_IO_DATA_
