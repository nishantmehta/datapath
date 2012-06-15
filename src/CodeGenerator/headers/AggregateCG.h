//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// for Emacs -*- c++ -*-

#ifndef _AGGREGATECG_H
#define _AGGREGATECG_H

#include "WayPointCG.h"
#include "Chunk.h"
#include "WayPointConfigureData.h"
#include "AggregateWayPointConfigureData.h"

#include <string>

using namespace std;

/** Concrete class for the Aggregate Code Generator
	*
	* Parameters expected in config object:
	* A. global parameters: NONE
	* B. per query parameters
	*   1. "aggList": list of expressions to print
	*      Snape: (el1,...)
	*      el1 is of the form (aggName, aggType, expression)
	*
**/
class AggregateCG: public WayPointCG {
public:
	AggregateCG(SymbolicWaypointConfig &config);
	virtual ~AggregateCG();

	void GenerateM4Code(ofstream &where);
	void ExtractCode(void *handle, WayPointConfigureData &where);
};

#endif // _AGGREGATECG_H
