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
#ifndef _LT_GLA_large_H_
#define _LT_GLA_large_H_

#include "LT_GLA.h"
#include "GLAData.h"

class LT_GLA_large : public LT_GLA {
public:

	LT_GLA_large(WayPointID id): LT_GLA(id)
	{}

	virtual WaypointType GetType() {return GLALargeWaypoint;}

	virtual void WriteM4File(ostream& out);

	virtual bool GetConfig(WayPointConfigureData& where);

	virtual ~LT_GLA_large(void);
};

#endif // _LT_GLA_large_H_
