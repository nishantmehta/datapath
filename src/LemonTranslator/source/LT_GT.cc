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
#include "LT_GT.h"
#include "AttributeManager.h"

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

bool LT_GT::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID gfIDOne = GetId ();

    // first, get the function we will send to it
    //    WorkFunc tempFunc = NULL;
    GTPreProcessWorkFunc GTPreProcessWF(NULL);
    GTProcessChunkWorkFunc GTProcessChunkWF (NULL);
    WorkFuncContainer myGLAWorkFuncs;
    myGLAWorkFuncs.Insert (GTPreProcessWF);
    myGLAWorkFuncs.Insert (GTProcessChunkWF);

    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer myGLAEndingQueryExits;
    QueryExitContainer myGLAFlowThroughQueryExits;
    GetQueryExits (myGLAFlowThroughQueryExits, myGLAEndingQueryExits);
    PDEBUG("Printing query exits for GT WP ID = %s", gfIDOne.getName().c_str());
#ifdef DEBUG
        cout << "\nFlow through query exits\n" << flush;
        myGLAFlowThroughQueryExits.MoveToStart();
        while (myGLAFlowThroughQueryExits.RightLength()) {
                (myGLAFlowThroughQueryExits.Current()).Print();
                myGLAFlowThroughQueryExits.Advance();
        }
        cout << "\nEnding query exits\n" << flush;
        myGLAEndingQueryExits.MoveToStart();
        while (myGLAEndingQueryExits.RightLength()) {
                (myGLAEndingQueryExits.Current()).Print();
                myGLAEndingQueryExits.Advance();
        }
        cout << endl;
#endif

    QueryToReqStates myReqStates;
    for( GTInfoMap::iterator it = gfInfoMap.begin(); it != gfInfoMap.end(); ++it ) {
        QueryID curID = it->first;
        StateSourceVec & reqStates = it->second.reqStates;

        ReqStateList stateList;

        for( StateSourceVec::iterator iter = reqStates.begin(); iter != reqStates.end(); ++iter  ) {
            WayPointID curSource = *iter;
            stateList.Append(curSource);
        }

        myReqStates.Insert( curID, stateList );
    }


    // here is the waypoint configuration data
    GTConfigureData gfConfigure (gfIDOne, myGLAWorkFuncs,  myGLAEndingQueryExits, myGLAFlowThroughQueryExits,
            myReqStates);

    // and add it
    where.swap (gfConfigure);

    return true;
}

void LT_GT::DeleteQuery(QueryID query)
{
    DeleteQueryCommon(query); // common data
    synthesized.erase(query);
    QueryID qID;
    SlotContainer slotCont;
    gfAttribs.Remove(query, qID, slotCont);
}

void LT_GT::ClearAllDataStructure() {
    ClearAll(); // common data
    gfAttribs.Clear();
    gfInfoMap.clear();
    synthesized.clear();
}

bool LT_GT::AddGT(QueryID query,
        SlotContainer& resultAtts, /*list of attributes produced as the result */
        string glaName, /*name of the GT */
                                        string glaDef,
        string constructorExp, /*expression in GT constructor */
        SlotSet& atts, string expr, string initializer,
        StateSourceVec reqStates)
{
    SlotSet attsSet;
    FOREACH_TWL(iter, resultAtts){
        attsSet.insert(iter);
    }END_FOREACH;
    QueryID qCopy = query;
    gfAttribs.Insert(qCopy, resultAtts);

    GTInfo gfInfo(glaName, glaDef, constructorExp, expr, initializer, reqStates);
    gfInfoMap[query] = gfInfo;
    CheckQueryAndUpdate(query, atts, newQueryToSlotSetMap);
    queriesCovered.Union(query);
    //synthesized
    CheckQueryAndUpdate(query, attsSet, synthesized);

    return true;
}

// atts coming from top is dropped as it will not be propagated down
// that also means, all attributes coming from top are synthesized ones
// 1. used = used + new queries attributes filled after analysis
// 2. result goes down = used
// 3. Correctness : Atts coming from top is subset of synthesized
bool LT_GT::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe)
{
    // atts coming from top should be subset of synthesized.
    if (!IsSubSet(atts, synthesized[query]))
    {
        cerr << "GTWP : Aggregate error: attributes coming from top are not subset of synthesized ones" << endl;
        cerr << "PropgateDown for Waypoint " << GetWPName() << endl;
        cerr << "Query: " << query.ToString() << endl;
        cerr << "atts: " << GetAllAttrAsString(atts) << endl;
        cerr << "synthesized[query]: " << GetAllAttrAsString(synthesized[query]) << endl;
        return false;
    }

    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    result.clear();
    result = used[query];
    queryExit.Insert (qe);
    return true;
}

bool LT_GT::PropagateDownTerminating(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe) {
    // atts coming from top should be subset of synthesized.
    if (!IsSubSet(atts, synthesized[query]))
    {
        cerr << "GTWP : Aggregate error: attributes coming from top are not subset of synthesized ones" << endl;
        cerr << "PropgateDownTerminating for Waypoint " << GetWPName() << endl;
        cerr << "Query: " << query.ToString() << endl;
        cerr << "atts: " << GetAllAttrAsString(atts) << endl;
        cerr << "synthesized[query]: " << GetAllAttrAsString(synthesized[query]) << endl;
        return false;
    }

    //CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    //result.clear();
    //result = used[query];
    queryExitTerminating.Insert(qe);
    return true;
}

// Implementation bottom -> up as follows for all queries together:
// 1. used = used + new queries attributes added since last analysis
// 2. clear the new data
// 3. result = NONE
// 4. Print is last destination hence result is blank
// 5. old used + new = used is good to check correctness if they are subset of down attributes
bool LT_GT::PropagateUp(QueryToSlotSet& result)
{
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();

    result.clear();
    result = synthesized;

    // used should be subset of what is coming from below
    if (!IsSubSet(used, downAttributes))
    {
        cout <<  "GTWP : Attribute mismatch : used is not subset of attributes coming from below\n";
        return false;
    }
    return true;
}

void LT_GT::WriteM4DataStructures(ostream& out) {

    out << "m4_divert(0)" << endl;

    // Note: This anonymous namespace is here so that the same GLAs and
    // functions may be defined in separate files without the linker
    // complaining. The anonymous namespace restricts the linkage visibility
    // of everything in it to this file only.
    //out << "namespace {" << endl;
    FOREACH_EM(key, data, gfAttribs){
        out << " /* Generating datastructures for query "
            << GetQueryName(key) << "*/" << endl;
        GTInfo gfInfo = gfInfoMap[key];
        out << gfInfo.defs << endl;
    } END_FOREACH;
    //out << "}";
}

void LT_GT::WriteM4File(ostream& out) {

    WriteM4DataStructures(out);

    out << "M4_GT_MODULE(" ;
    WriteM4FileAux(out);
}

/**
   need to produce
   M4_GT_MODULE( M4_WPName, M4_GTDesc, M4_Attribute_Queries)
 */

void LT_GT::WriteM4FileAux(ostream& out) {
    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    AttributeManager& am = AttributeManager::GetAttributeManager();

    out << wpname << ", ";
    out << "\t</";

    //GLADesc starts here.
    // go through the query to GT attribute map
    bool first_gfAttribs = true;
    FOREACH_EM(key, data, gfAttribs){
        if( first_gfAttribs )
            first_gfAttribs = false;
        else
            out << ", ";

        out << "( " << GetQueryName(key) << ", ";
        GTInfo gfInfo = gfInfoMap[key];

        out << gfInfo.name << "," << "dummy" << ", </" << gfInfo.constructExp << "/>, </(";
        bool first = true;
        FOREACH_TWL(iter, data){
            if( first )
                first = false;
            else
                out << ", ";
            out << am.GetAttributeName(iter);
        } END_FOREACH;

        out << ")/>, </";

        //write already formatted expr
        out << '(' << gfInfo.expression << ')' << "/>, ";

        //write constantExpr
        out << "</" << gfInfo.initializer << "/>";

        // end of GT description
        out << ")";
    } END_FOREACH;

    out << "/>,\t"; // end of GTDesc

    // format: (att_name, QueryIDSet_serialized), ..
    SlotToQuerySet reverse;
    AttributesToQuerySet(used, reverse);
    PrintAttToQuerySets(reverse, out);

    // macro call end
    out << ")" << endl;

}
