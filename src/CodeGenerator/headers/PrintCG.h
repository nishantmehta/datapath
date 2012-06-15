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

#ifndef _PRINTCG_H
#define _PRINTCG_H

#include "WayPointCG.h"
#include "Chunk.h"
#include "WayPointConfigureData.h"
#include "PrintWayPointConfigureData.h"

#include <string>

using namespace std;

/** Concrete class for the Print Code Generator
	*
	* Parameters expected in config object:
	* A. global parameters: NONE
	* B. per query parameters
	*   1. "printList": list of expressions to print
	*      Snape: (el1,...)
	*      el1 is of the form (colname, expression) or attribute
	*
	*
**/


class PrintCG: public WayPointCG {
public:
	PrintCG(SymbolicWaypointConfig &config);
	virtual ~PrintCG();

	void GenerateM4Code(ofstream &where);
	void ExtractCode(void *handle, WayPointConfigureData &where);
};

#endif // _PRINTCG_H
