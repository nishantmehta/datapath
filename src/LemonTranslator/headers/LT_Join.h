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
#ifndef _LT_JOIN_H_
#define _LT_JOIN_H_

#include "LT_Waypoint.h"

class LT_Join : public LT_Waypoint {
private:

	// Common attributes LHS
	SlotSet LHS_atts;

	// Data received from terminting edge during bottom up analysis to be used in top down later
	QueryToSlotSet RHS_terminating;
	// Data for RHS while adding query attributes pairs
	QueryToSlotSet RHS;
	// attributes to copy for each query 
	QueryToSlotSet LHS_copy; 
	QueryToSlotSet RHS_copy; 

	QueryIDSet ExistsTarget; // set of queries for which we run an exists predicate
	QueryIDSet NotExistsTarget; // same for notExists

	// id of cleaner so we can write config messages
	WayPointID cleanerID;

public:

 LT_Join(WayPointID id, const SlotSet& atts, WayPointID _cleanerID): LT_Waypoint(id), LHS_atts(atts), cleanerID(_cleanerID)
	{}

	virtual WaypointType GetType() {return JoinWaypoint;}

	virtual void ClearAllDataStructure();

	virtual void ReceiveAttributesTerminating(QueryToSlotSet& atts);

	virtual bool AddBypass(QueryID query);

	virtual void DeleteQuery(QueryID query);

	virtual bool AddJoin(QueryID query, SlotSet& RHS_atts, LemonTranslator::JoinType type);

	// Consider this as LHS
	virtual bool PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe);

	virtual bool PropagateDownTerminating(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe);

	virtual bool PropagateUp(QueryToSlotSet& result);

	virtual void WriteM4File(ostream& out);
	virtual void WriteM4LHSHashFile(ostream& out);
	virtual void WriteM4JoinMergeFile(ostream& out);

	virtual bool GetConfig(WayPointConfigureData& where);

	virtual bool GetConfigs(WayPointConfigurationList& where);

	virtual void GetAccumulatedLHSRHS(set<SlotID>& LHS, set<SlotID>& RHS, QueryIDSet& queries);
	virtual void GetAccumulatedLHSRHSAtts(set<SlotID>& LHS, set<SlotID>& RHS);

	virtual void GetQueryExitToSlotMapLHS(QueryExitToSlotsMap& qe);
	virtual void GetQueryExitToSlotMapRHS(QueryExitToSlotsMap& qe);
};


#endif // _LT_JOIN_H_
