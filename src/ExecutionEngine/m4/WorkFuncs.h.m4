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

#ifndef WORK_FUNC_H
#define WORK_FUNC_H

#include "WorkerMessages.h"
#include "GLAData.h"
#include <string>

using namespace std;

// this contains all of the work func types

// this is the basic wrapper for the work functions that are sent into waypoints 
// at configuration time.  All functions to be run are of type "WorkFunc";
// see the file "WorkerMessages.h.m4" in CPUWorkers/m4 for the definition of "WorkFunc"
M4_CREATE_BASE_DATA_TYPE(WorkFuncWrapper, DataC,
 </(myFunc, WorkFunc)/>,
 <//>)

// this is a list of work functions that are used to configure a waypoint
typedef TwoWayList <WorkFuncWrapper> WorkFuncContainer;

// We should have one of these for each of the types of work funcs in the system.
// It does not have anything in it above and beyond what the WorkFuncWrapper has.
// but the fact that we have one of these for each of the work functions means that
// we can put the work functions in a list of swappable objects, and do things like
// search the list for the "AggFinishUpWorkFunc" function, or the "AggWorkFunc"
// function
//Join
M4_CREATE_DATA_TYPE(JoinLHSWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(JoinRHSWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(JoinLHSHashWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(JoinMergeWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(CleanerWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(WriterWorkFunc, WorkFuncWrapper, <//>, <//>)
//Aggregate
M4_CREATE_DATA_TYPE(AggOneChunkWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(AggFinishUpWorkFunc, WorkFuncWrapper, <//>, <//>)
// Selection
M4_CREATE_DATA_TYPE(SelectionPreProcessWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(SelectionProcessChunkWorkFunc, WorkFuncWrapper, <//>, <//>)
// Print
M4_CREATE_DATA_TYPE(PrintWorkFunc, WorkFuncWrapper, <//>, <//>)
// TableScan
M4_CREATE_DATA_TYPE(TableScanWorkFunc, WorkFuncWrapper, <//>, <//>)
// TextLoader
M4_CREATE_DATA_TYPE(TextLoaderWorkFunc, WorkFuncWrapper, <//>, <//>)

/** WorkFuncs for GLA*/
M4_CREATE_DATA_TYPE(GLAPreProcessWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAProcessChunkWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAPreFinalizeWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAFinalizeWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAFinalizeStateWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAMergeStatesWorkFunc, WorkFuncWrapper, <//>, <//>)

/** WorkFuncs for GTs */
M4_CREATE_DATA_TYPE(GTPreProcessWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GTProcessChunkWorkFunc, WorkFuncWrapper, <//>, <//>)

/** WorkFuncs for GISTs */

M4_CREATE_DATA_TYPE(GISTPreProcessWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTNewRoundWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTDoStepsWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTMergeStatesWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTShouldIterateWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTProduceResultsWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GISTProduceStateWorkFunc, WorkFuncWrapper, <//>, <//>)

#endif
