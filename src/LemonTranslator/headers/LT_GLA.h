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
#ifndef _LT_GLA_H_
#define _LT_GLA_H_

#include "LT_Waypoint.h"
#include "GLAData.h"

class LT_GLA : public LT_Waypoint {

private:

    typedef vector<WayPointID> StateSourceVec;

    struct GLAInfo {
        string name;
        string defs;
        string constructExp;
        string expression;
        string initializer;
        StateSourceVec reqStates;
        bool retState;

        GLAInfo() {} // ctor for map compatibility
    GLAInfo(string _name, string _defs, string _constructExp, string _expression, string _initializer,
            StateSourceVec _reqStates, bool _retState ):
        name(_name), defs(_defs), constructExp(_constructExp), expression(_expression),
        initializer(_initializer), reqStates(_reqStates), retState(_retState) {}
    };

    // map from query to output attributes
    typedef EfficientMap<QueryID, SlotContainer> QueryToSlotContainer;
    QueryToSlotContainer glaAttribs;

    // info for each GLA attribute-container
    typedef map<QueryID, GLAInfo> GLAInfoMap;
    GLAInfoMap glaInfoMap;

    QueryToSlotSet synthesized;


 protected:
    void WriteM4FileAux(ostream& out);
    void WriteM4DataStructures(ostream& out);

public:

    LT_GLA(WayPointID id): LT_Waypoint(id)
    {}

    virtual WaypointType GetType() {return GLAWaypoint;}

    virtual void DeleteQuery(QueryID query);

    virtual void ClearAllDataStructure();

    //GLA, one per query basis
    virtual bool AddGLA(QueryID query,
                    SlotContainer& resultAtts,
                    string glaName,
                    string glaDef,
                    string constructorExp, /*expression in GLA constructor */
                    SlotSet& atts, string expr, string initializer,
                    StateSourceVec reqStates,
                    bool retState);

    virtual bool PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe);

    virtual bool PropagateDownTerminating(QueryID query, const SlotSet& atts/*blank*/, SlotSet& result, QueryExit qe);

    virtual bool PropagateUp(QueryToSlotSet& result);

    virtual void WriteM4File(ostream& out);

    virtual bool GetConfig(WayPointConfigureData& where);

};

#endif // _LT_GLA_H_
