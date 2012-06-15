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
/**
	This header file contains the messages used by DiskIO.
*/

dnl # // M4 PREAMBLE CODE
include(MessagesFunctions.m4)
dnl # END OF M4 CODE

#ifndef _DISKIO_MESSAGES_H_
#define _DISKIO_MESSAGES_H_

#include "Message.h"
#include "EventProcessor.h"
#include "DistributedCounter.h"
#include "ID.h"
#include "Chunk.h"
#include "QueryExit.h"
#include "QueryTerminationTracker.h"
#include "DiskIOData.h"
#include "Data.h"
#include "ExecEngineData.h"
#include "EEExternMessages.h"

///////////// DISK STATISTICS MESSAGE //////////

/** Message sent by each individual disk to the DiskArray with the
		latest statistics on how fast it works. The statistics consist in
		the estimate for the expected value and variance of the time/page
		each disk takes.

		Arguments:
			diskNo: which disk
			expectation: the expectation of the time
			variance: the variance of the time
*/
M4_CREATE_MESSAGE_TYPE(DiskStatistics,
	</(diskNo, int), (expectation, double), (variance, double)/>,<//>)


///////////// DISK OPERATION MESSAGE ////////////

/** Message sent by the ChunkReaderWriter to the DiskArray to ask it
		to perform an operation. The notification comes back from the
		HDThreaad that is the last one performing the operation, thus
		saving extra messages between the DiskArray and HDThread.

		Arguments:
			requestID: request identifier (so that the caller knows what is confirmed
			operation: either the DISK_READ or DISK_WRITE macros
			pages: the pages that need to be accessed
*/

M4_CREATE_MESSAGE_TYPE(DiskOperation,
	</(requestId, off_t),(operation,int)/>,
	</(requestor,EventProcessor), (requests, DiskRequestDataContainer)/>)


//////////// MEGA JOB MESSAGE  ///////////

/** Message sent by the DiskArray to each HDThread to set up a chunk read

	Arguments:
		requestID: request identifier (so that the caller knows what is confirmed
		operation: either the DISK_READ or DISK_WRITE macros
		counter: the counter used by all the HDThreads to know when to tell the ChunkReader that the MegaJob is fully processed
		pages: the pages that need to be accessed
*/
M4_CREATE_MESSAGE_TYPE(MegaJob,
	</(requestId, off_t),(operation,int),(counter,DistributedCounter*)/>,
	</(requestor,EventProcessor), (requests, DiskRequestDataContainer)/>)

/////////// MEGA JOB FINISHED MESSAGE /////////

/** Message sent by HDThreads when a mega-job is finished

	Arguments:
		requestId: request identifier  copied form the original message
		operation: either the READ or WRITE macros
		counter: the counter used by all the HDThreads to know when to tell the ChunkReader that the MegaJob is fully processed
*/
M4_CREATE_MESSAGE_TYPE(MegaJobFinished,
	</(requestId, off_t),(operation,int), (counter,DistributedCounter*)/>,<//>)

//////////// CHUNK READ MESSAGE //////////////
/** Message by execution engine to the ChunkReaderWriter to read a chunk.

	Arguments:
	   chunkID: which chunk to read
		 useUncompressed: if set to true, uncompressed data is used, otherwise compressed
		 lineage, request: used for routing the reply inside execEngine
		 dest: destination queryExits
		 colsToProcess: list of (logical,phisical) columns to process
*/

M4_CREATE_MESSAGE_TYPE(ChunkRead,
	</(requestor, WayPointID), (chunkID, off_t), (useUncompressed, bool)/>,
	</(lineage, HistoryList), (dest, QueryExitContainer), (token, GenericWorkToken), (colsToProcess, SlotPairContainer)/>)

//////////// CHUNK WRITE MESSAGE //////////////
/** Same as above but used for writing chunks
		
	Arguments:
     numTuples: number of tuples in chunk
		 lineage, request: used for routing the reply inside execEngine
		 dest: destination queryExits
		 colsToProcess: list of (logical,phisical) columns to process
		
*/
M4_CREATE_MESSAGE_TYPE(ChunkWrite,
	</(requestor, WayPointID)/>,
	</(chunk, Chunk), (lineage, HistoryList), (dest, QueryExitContainer), (token, GenericWorkToken), (colsToProcess, SlotPairContainer)/>)


//////////// COMMIT METADATA ///////////////////
/* Message sent to the ChunkReaderWriter to tell it to write the current metadata
	 to the Catalog. 		A reply is sent back to the requestor when the operation is done.

	 Arguments:
		requestID: the id of the original request
		append: if true, data is appended, else it overides existing
		requestor: the event processor that gets the ACK

*/
M4_CREATE_MESSAGE_TYPE(CommitMetadata,
	</(requestID, off_t), (append, bool)/>,
	</(requestor,EventProcessor)/>)

dnl // This is used to Flush all metadata on disk
M4_CREATE_MESSAGE_TYPE(Flush,
  <//>,<//>)

#endif // _DISKIO_MESSAGES_H_
