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
#ifndef _JOINCG_H
#define _JOINCG_H

#include "WayPointCG.h"
#include "Chunk.h"
#include "WayPointConfigureData.h"
#include "JoinWayPointConfigureData.h"

#include <string>

using namespace std;

/** Concrete class for the Join Code Generator */
class JoinCG: public WayPointCG {
public:
	JoinCG(SymbolicWaypointConfig &config);
	virtual ~JoinCG();

	void GenerateM4Code(ofstream &where);
	void ExtractCode(void *handle, WayPointConfigureData &where);
};

#endif // _JOINCG_H
