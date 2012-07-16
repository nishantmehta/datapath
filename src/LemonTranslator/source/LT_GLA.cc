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
#include "LT_GLA.h"
#include "AttributeManager.h"

bool LT_GLA::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID glaIDOne = GetId ();

    // first, get the function we will send to it
    //    WorkFunc tempFunc = NULL;
    GLAProcessChunkWorkFunc GLAProcessChunkWF (NULL);
    GLAMergeStatesWorkFunc GLAMergeWF (NULL);
    GLAPreFinalizeWorkFunc GLAPreFinalizeWF(NULL);
    GLAFinalizeWorkFunc GLAFinalizeWF (NULL);
    WorkFuncContainer myGLAWorkFuncs;
    myGLAWorkFuncs.Insert (GLAProcessChunkWF);
    myGLAWorkFuncs.Insert (GLAMergeWF);
    myGLAWorkFuncs.Insert (GLAPreFinalizeWF);
    myGLAWorkFuncs.Insert (GLAFinalizeWF);

    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer myGLAEndingQueryExits;
    QueryExitContainer myGLAFlowThroughQueryExits;
    GetQueryExits (myGLAFlowThroughQueryExits, myGLAEndingQueryExits);
    PDEBUG("Printing query exits for AGGREGATE WP ID = %s", glaIDOne.getName().c_str());
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


    // here is the waypoint configuration data
    GLAConfigureData glaConfigure (glaIDOne, myGLAWorkFuncs,  myGLAEndingQueryExits, myGLAFlowThroughQueryExits);

    // and add it
    where.swap (glaConfigure);

    return true;
}

void LT_GLA::DeleteQuery(QueryID query)
{
    DeleteQueryCommon(query); // common data
    synthesized.erase(query);
    QueryID qID;
    SlotContainer slotCont;
    glaAttribs.Remove(query, qID, slotCont);
}

void LT_GLA::ClearAllDataStructure() {
    ClearAll(); // common data
    glaAttribs.Clear();
    glaInfoMap.clear();
    synthesized.clear();
}

bool LT_GLA::AddGLA(QueryID query,
        SlotContainer& resultAtts, /*list of attributes produced as the result */
        string glaName, /*name of the GLA eg. AverageGLA, CountGLA, myGLA etc */
                                        string glaDef,
        string constructorExp, /*expression in GLA constructor */
        SlotSet& atts, string expr, string initializer)
{
    SlotSet attsSet;
    FOREACH_TWL(iter, resultAtts){
        attsSet.insert(iter);
    }END_FOREACH;
    QueryID qCopy = query;
    glaAttribs.Insert(qCopy, resultAtts);

    GLAInfo glaInfo(glaName, glaDef, constructorExp, expr, initializer);
    glaInfoMap[query] = glaInfo;
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
bool LT_GLA::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe)
{
    // atts coming from top should be subset of synthesized.
    if (!IsSubSet(atts, synthesized[query]))
    {
        cout << "AggregateWP : Aggregate error: attributes coming from top and not subset of synthesized ones\n";
        return false;
    }

    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    result.clear();
    result = used[query];
    queryExit.Insert (qe);
    return true;
}

// Implementation bottom -> up as follows for all queries together:
// 1. used = used + new queries attributes added since last analysis
// 2. clear the new data
// 3. result = NONE
// 4. Print is last destination hence result is blank
// 5. old used + new = used is good to check correctness if they are subset of down attributes
bool LT_GLA::PropagateUp(QueryToSlotSet& result)
{
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();

    result.clear();
    result = synthesized;

    // used should be subset of what is coming from below
    if (!IsSubSet(used, downAttributes))
    {
        cout <<  "GLAWP : Attribute mismatch : used is not subset of attributes coming from below\n";
        return false;
    }
    return true;
}

void LT_GLA::WriteM4DataStructures(ostream& out) {

    out << "m4_divert(0)" << endl;

    // Note: This anonymous namespace is here so that the same GLAs and
    // functions may be defined in separate files without the linker
    // complaining. The anonymous namespace restricts the linkage visibility
    // of everything in it to this file only.
    out << "namespace {" << endl;
    FOREACH_EM(key, data, glaAttribs){
        out << " /* Generating datastructures for query "
            << GetQueryName(key) << "*/" << endl;
        GLAInfo glaInfo = glaInfoMap[key];
        out << glaInfo.defs << endl;
    } END_FOREACH;
    out << "}";
}

void LT_GLA::WriteM4File(ostream& out) {

    WriteM4DataStructures(out);

    out << "M4_GLA_MODULE(" ;
    WriteM4FileAux(out);
}

/**
   need to produce
   M4_GLA_MODULE( M4_WPName, M4_GLADesc, M4_Attribute_Queries)

   where M4_GLADesc is a list of the form
   (qname, glaName, </(constructorExp)/>, </(outAtt1, outAtt2, ...)/>, </(exp1, exp2, ...)/>, </ctExpr/>)

   Note: (exp1, exp2, ...) is already formed in expr
 */

void LT_GLA::WriteM4FileAux(ostream& out) {
    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    AttributeManager& am = AttributeManager::GetAttributeManager();


    out << wpname << ", ";
    out << "\t</";

    //GLADesc starts here.
    // go through the query to GLA attribute map
    FOREACH_EM(key, data, glaAttribs){
        out << "( " << GetQueryName(key) << ", ";
        GLAInfo glaInfo = glaInfoMap[key];

        out << glaInfo.name << "," << "dummy" << ", </" << glaInfo.constructExp << "/>, </(";
        FOREACH_TWL(iter, data){
            out << am.GetAttributeName(iter) <<  ", ";
        } END_FOREACH;

        //remove last comma
        out.seekp (-2, ios_base::cur);
        out << ")/>, </";

        //write already formatted expr
        out << glaInfo.expression << "/>, ";

        //write constantExpr
        out << "</" << glaInfo.initializer << "/>)";
        out << ", ";
    } END_FOREACH;

    //remove last comma
    out.seekp (-2, ios_base::cur);
    out << "/>,\t"; // end of GLADesc

    // format: (att_name, QueryIDSet_serialized), ..
    SlotToQuerySet reverse;
    AttributesToQuerySet(used, reverse);
    PrintAttToQuerySets(reverse, out);

    // macro call end
    out << ")" << endl;

}
