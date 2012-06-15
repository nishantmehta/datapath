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

#ifndef _SELECTIONCG_H
#define _SELECTIONCG_H

#include "WayPointCG.h"
#include "Chunk.h"
#include "WayPointConfigureData.h"
#include "SelectionWayPointConfigureData.h"

/** Concrete class for a Selection CodeGenerator
	*
	*
	* Parameters expected in config object:
	* A. global parameters: NONE
	* B. per query parameters
	*   1. "selection": the selection predicate
	*      Snape: predicate
	*
	*
**/
class SelectionCG: public WayPointCG {
public:
	SelectionCG(SymbolicWaypointConfig &config);
	virtual ~SelectionCG();

	void GenerateM4Code(ofstream &where);
	void ExtractCode(void *handle, WayPointConfigureData &where);
};

#endif // _SELECTIONCG_H
