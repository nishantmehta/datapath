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
/** When a new waypoint type is added, the header for that waypoint
	* needs to be added to the include list and an entry in the case
	* statement added to deal with the new type
	*
	* It is asumend that the headers contain both the declaration and
	* definition of the code for each waypoint so we can generate a
	* single .o The code in the classes generated from WayPointCG should
	* not be called directly anyway
	*
**/

#include "SymbolicWaypointConfig.h"
#include "WayPointConfigureData.h"
#include "Errors.h"

// The headers so that we can deal with all types of waypoints
#include "SelectionCG.h"
#include "AggregateCG.h"
#include "JoinCG.h"
#include "PrintCG.h"

WayPointCG *CGFactory(SymbolicWaypointConfig &config){
	WayPointCG *rez;

	WaypointType t = config.GetType();
	switch(t){
		case SelectionWaypoint:
			rez = new SelectionCG(config);
			break;

		case AggregateWaypoint:
			rez = new AggregateCG(config);
			break;

		case JoinWaypoint:
			rez = new JoinCG(config);
			break;

		case PrintWaypoint:
			rez = new PrintCG(config);
			break;


		default:
			FATAL("Waypoint type not implemented");
	}

	return rez;
}
