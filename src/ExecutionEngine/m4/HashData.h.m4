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

#ifndef HASH_DATA_H
#define HASH_DATA_H

#include "Data.h"
#include "JoinWayPointID.h"
#include "Chunk.h"
#include <stdio.h>

// this stores data containers that relate to the central hash table

// when someone is adding data to the hash table, they use their first 100 or so
// probes to collect info on the state of the table; this ob ject stores that info
M4_CREATE_BASE_DATA_TYPE(HashEntrySummary, DataC,
 </(whichWayPoint, JoinWayPointID), (whichQueries, QueryID)/>,
 <//>)

// we will have one HashEntrySummary associated with each probe
typedef TwoWayList <HashEntrySummary> HashSegmentSample;

// this is just a linked list of ints
typedef Swapify <int> SwapifiedInt;
typedef TwoWayList <SwapifiedInt> IntList;

// this is used to store a chunk that has been extracted from the central hash table...
// whichWayPoint tells which waypoint all of the data in the chunk originally came from
// isLHS is nonzero if and only if this is a LHS chunk; otherwise, it is a RHS one
// whichSegment identifies the particular hash table segment that this chunk came from
M4_CREATE_BASE_DATA_TYPE(ExtractedChunk, DataC,
 </(isLHS, int), (whichWayPoint, WayPointID)/>,
 </(myChunk, Chunk)/>)

typedef TwoWayList <ExtractedChunk> ExtractionList;

#endif
