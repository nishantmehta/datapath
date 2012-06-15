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
#ifndef _LT_AGGREGATE_H_
#define _LT_AGGREGATE_H_

#include "LT_Waypoint.h"

class LT_Aggregate : public LT_Waypoint {

private:

	struct AggInfo {
		SlotID att;
		string type;
		string expression;
		string initializer;

		AggInfo() {} // ctor for map compatibility		
		AggInfo(SlotID _att, string _type, string _expression, string _initializer):
			att(_att), type(_type), expression(_expression), initializer(_initializer){}
	};

	// set of aggregate attributes for each query
	QueryToSlotSet aggAttribs; 
	// info for each attribute
	typedef map<SlotID, AggInfo> AggInfoMap;
	AggInfoMap aggInfoMap;
	// initializers for expressions
	typedef map<QueryID, string> QueryAggToExpr;
	QueryAggToExpr initializers;


	QueryToSlotSet synthesized;

public:

	LT_Aggregate(WayPointID id): LT_Waypoint(id)
	{}

	virtual WaypointType GetType() {return AggregateWaypoint;}

	virtual void DeleteQuery(QueryID query);
	
	virtual void ClearAllDataStructure();

	virtual bool AddAggregate(QueryID query, 
										SlotID slot, /* attribute corresponding to aggregate */
										string aggregateType, /* type, just pass along */ 
										SlotSet& atts, string expr, string initializer);

	virtual bool PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe);

	virtual bool PropagateUp(QueryToSlotSet& result);

	virtual void WriteM4File(ostream& out);

	virtual bool GetConfig(WayPointConfigureData& where);
};

#endif // _LT_AGGREGATE_H_
