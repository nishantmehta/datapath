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
#include "LT_GIST.h"
#include "AttributeManager.h"

using namespace std;

static string GetAllAttrAsString(const set<SlotID>& atts) {
    AttributeManager& am = AttributeManager::GetAttributeManager();
    string rez;
    bool first = true;
    for (set<SlotID>::iterator it = atts.begin(); it != atts.end(); it++) {
        if (first)
            first = false;
        else
            rez += ", ";

        SlotID slot = *it;
        rez += am.GetAttributeName(slot);
    }
    return rez;
}

bool LT_GIST :: GetConfig(WayPointConfigureData& where) {
    // Get the ID
    WayPointID myID = GetId();

    // Set up work functions.
    GISTPreProcessWorkFunc preprocess(NULL);
    GISTNewRoundWorkFunc newRound(NULL);
    GISTDoStepsWorkFunc doSteps(NULL);
    GISTMergeStatesWorkFunc mergeStates(NULL);
    GISTShouldIterateWorkFunc shouldIterate(NULL);
    GISTProduceResultsWorkFunc produceResults(NULL);
    GISTProduceStateWorkFunc produceState(NULL);

    WorkFuncContainer myWorkFuncs;

    myWorkFuncs.Insert(preprocess);
    myWorkFuncs.Insert(newRound);
    myWorkFuncs.Insert(doSteps);
    myWorkFuncs.Insert(mergeStates);
    myWorkFuncs.Insert(shouldIterate);
    myWorkFuncs.Insert(produceResults);
    myWorkFuncs.Insert(produceState);

    // Manage query exits
    QueryExitContainer myEndingQueryExits;
    QueryExitContainer myFlowThroughQueryExits;
    GetQueryExits (myFlowThroughQueryExits, myEndingQueryExits);

    // Managed required states
    QueryToReqStates myReqStates;
    QueryIDToBool myReturnStates;
    for( GISTInfoMap::iterator it = gistInfoMap.begin(); it != gistInfoMap.end(); ++it ) {
        QueryID curID = it->first;
        StateSourceVec& reqStates = it->second.reqStates;

        ReqStateList stateList;

        for( StateSourceVec::iterator iter = reqStates.begin(); iter != reqStates.end(); ++iter) {
            WayPointID curSource = *iter;
            stateList.Append(curSource);
        }

        myReqStates.Insert( curID, stateList );

        Swapify<bool> retState = it->second.retState;
        curID = it->first;

        myReturnStates.Insert(curID, retState);
    }

    // Create the configuration data.
    GISTConfigureData myConfig(myID, myWorkFuncs, myEndingQueryExits,
            myFlowThroughQueryExits, myReqStates, myReturnStates);

    where.swap(myConfig);

    return true;
}

void LT_GIST :: DeleteQuery(QueryID query) {
    DeleteQueryCommon(query);
    synthesized.erase(query);
    QueryID qID;
    SlotContainer slotCont;
    gistAttribs.Remove(query, qID, slotCont);

    gistInfoMap.erase(query);
}

void LT_GIST :: ClearAllDataStructure() {
    ClearAll();
    gistAttribs.Clear();
    gistInfoMap.clear();
    synthesized.clear();
}

bool LT_GIST :: AddGIST( QueryID query,
        SlotContainer& resultAtts,
        string gistName,
        string gistDef,
        string constructorExp,
        StateSourceVec reqStates,
        bool retState ) {
    SlotSet attsSet;
    FOREACH_TWL(iter, resultAtts) {
        attsSet.insert(iter);
    } END_FOREACH;

    QueryID qCopy = query;
    gistAttribs.Insert(qCopy, resultAtts);

    GISTInfo gistInfo(gistName, gistDef, constructorExp, reqStates, retState);
    gistInfoMap[query] = gistInfo;
    queriesCovered.Union(query);
    CheckQueryAndUpdate(query, attsSet, synthesized);

    return true;
}

// atts coming from top is dropped as it will not be propagated down
// that also means, all attributes coming from top are synthesized ones
// 1. result is blank, as this waypoint uses no attributes
// 2. Correctness : Atts coming from top is subset of synthesized
bool LT_GIST :: PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe) {
    // atts coming from top should be subset of synthesized
    if( !IsSubSet(atts, synthesized[query]) ) {
        cerr << "GISTWP : Aggregate error: attributes coming from top are not subset of synthesized ones" << endl;
        cerr << "PropgateDown for Waypoint " << GetWPName() << endl;
        cerr << "Query: " << query.ToString() << endl;
        cerr << "atts: " << GetAllAttrAsString(atts) << endl;
        cerr << "synthesized[query]: " << GetAllAttrAsString(synthesized[query]) << endl;
        return false;
    }

    result.clear();
    queryExit.Insert(qe);
    return true;
}

bool LT_GIST :: PropagateDownTerminating(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe ) {
    // atts coming from top should be subset of synthesized
    if( !IsSubSet(atts, synthesized[query]) ) {
        cerr << "GISTWP : Aggregate error: attributes coming from top are not subset of synthesized ones" << endl;
        cerr << "PropgateDownTerminating for Waypoint " << GetWPName() << endl;
        cerr << "Query: " << query.ToString() << endl;
        cerr << "atts: " << GetAllAttrAsString(atts) << endl;
        cerr << "synthesized[query]: " << GetAllAttrAsString(synthesized[query]) << endl;
        return false;
    }

    queryExitTerminating.Insert(qe);
    return true;
}

// GIST uses no attributes, so just say that our result is the attributes we're producing.
bool LT_GIST :: PropagateUp(QueryToSlotSet& result) {
    result.clear();
    result = synthesized;

    return true;
}

void LT_GIST :: WriteM4DataStructures(ostream& out) {
    out << "m4_divert(0)" << endl;

    FOREACH_EM(key, data, gistAttribs){
        out << " /* Generating datastructures for query "
            << GetQueryName(key) << "*/" << endl;
        GISTInfo gistInfo = gistInfoMap[key];
        out << gistInfo.defs << endl;
    } END_FOREACH;
}

/**
 * need to produce
 * M4_GIST_MODULE( M4_WPName, M4_GISTDesc )
 *
 * where M4_GISTDesc is a list of the form
 * (query name, gistName, unused, </(constructorExp)/>, </(outAtt1, outAtt2, ...)/>)
 */
void LT_GIST :: WriteM4File(ostream& out) {
    WriteM4DataStructures(out);

    out << "M4_GIST_MODULE(";

    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    AttributeManager& am = AttributeManager::GetAttributeManager();

    out << wpname << ", ";
    out << "</";

    // GISTDesc starts here
    bool first_gistAttribs = true;
    FOREACH_EM(key, data, gistAttribs) {
        if( first_gistAttribs )
            first_gistAttribs = false;
        else
            cout << ", ";

        // Begin entry for a query
        out << "(";

        // Argument 1: Query
        out << GetQueryName(key);

        // Argument 2: GIST name
        GISTInfo gistInfo = gistInfoMap[key];
        out << ", " << gistInfo.name;

        // Argument 3: unused
        out << ", <//>";

        // Argument 4: constructor expression
        out << ", </" << gistInfo.constructExp << "/>";

        // Argument 5: output attributes
        out << ", </(";
        bool first = true;
        FOREACH_TWL(iter, data) {
            if(first)
                first = false;
            else
                out << ", ";

            out << am.GetAttributeName(iter);
        } END_FOREACH;
        out << ")/>";

        // End entry for this query
        out << ")";
    } END_FOREACH;

    // End of GISTDesc
    out << "/>";

    // Macro call end
    out << ")" << endl;
}
