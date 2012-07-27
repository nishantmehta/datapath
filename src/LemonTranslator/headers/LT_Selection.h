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
#ifndef _LT_SELECTION_H_
#define _LT_SELECTION_H_

#include "LT_Waypoint.h"

class LT_Selection : public LT_Waypoint {
private:

    struct SynthInfo {
        SlotID att;
        string expression;
        string initializer;

        SynthInfo() {} // ctor for map compatibility
        SynthInfo(SlotID _att, string _expression, string _initializer):
            att(_att), expression(_expression), initializer(_initializer){}
    };
    // info for each synthesized attribute
    typedef map<SlotID, SynthInfo> SynthInfoMap;
    SynthInfoMap synthInfoMap;

    // query to filtering condition (string)
    typedef map<QueryID, string> QueryFilterToExpr;

    QueryFilterToExpr filters;
    QueryFilterToExpr initializers;
    QueryFilterToExpr definitions;

    QueryToSlotSet synthesized;

public:

    LT_Selection(WayPointID id): LT_Waypoint(id)
    {}

    virtual void ClearAllDataStructure();

    virtual WaypointType GetType() {return SelectionWaypoint;}

    virtual bool AddBypass(QueryID query);

    virtual void DeleteQuery(QueryID query);

    virtual bool AddFilter(QueryID query, SlotSet& atts, string expr, string initializer, string defs);

    virtual bool AddSynthesized(QueryID query, SlotID att, SlotSet& atts, string expr, string initializer, string defs);

    virtual bool PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe);

    virtual bool PropagateUp(QueryToSlotSet& result);

    virtual void WriteM4File(ostream& out);

    virtual bool GetConfig(WayPointConfigureData& where);
}; // class

#endif // _LT_SELECTION_H_
