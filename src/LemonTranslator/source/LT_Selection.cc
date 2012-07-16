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
#include "LT_Selection.h"
#include "AttributeManager.h"

bool LT_Selection::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID selectionID = GetId ();

    // first, get the function we will send to it
    WorkFunc tempFunc = NULL; // NULL, will be populated in codeloader
    SelectionWorkFunc mySelectionWorkFunc (tempFunc);
    WorkFuncContainer mySelectionWorkFuncs;
    mySelectionWorkFuncs.Insert (mySelectionWorkFunc);

    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer mySelectionEndingQueryExits;
    QueryExitContainer mySelectionFlowThroughQueryExits;
    GetQueryExits (mySelectionFlowThroughQueryExits, mySelectionEndingQueryExits);
    PDEBUG("Printing query exits for SELECTION WP ID = %s", selectionID.getName().c_str());
#ifdef DEBUG
        cout << "\nFlow through query exits\n" << flush;
        mySelectionFlowThroughQueryExits.MoveToStart();
        while (mySelectionFlowThroughQueryExits.RightLength()) {
                (mySelectionFlowThroughQueryExits.Current()).Print();
                mySelectionFlowThroughQueryExits.Advance();
        }
        cout << "\nEnding query exits\n" << flush;
        mySelectionEndingQueryExits.MoveToStart();
        while (mySelectionEndingQueryExits.RightLength()) {
                (mySelectionEndingQueryExits.Current()).Print();
                mySelectionEndingQueryExits.Advance();
        }
        cout << endl;
#endif


    // here is the waypoint configuration data
    SelectionConfigureData selectionConfigure (selectionID, mySelectionWorkFuncs, mySelectionEndingQueryExits, mySelectionFlowThroughQueryExits);

    where.swap (selectionConfigure);

    return true;
}

bool LT_Selection::AddBypass(QueryID query) {
    bypassQueries.Union(query);
    queriesCovered.Union(query);
    return true;
}

void LT_Selection::DeleteQuery(QueryID query) {
    DeleteQueryCommon(query);
    filters.erase(query);
}

void LT_Selection::ClearAllDataStructure() {
    ClearAll();
    filters.clear();
    synthesized.clear();
}

bool LT_Selection::AddFilter(QueryID query, SlotSet& atts, string expr /*filter cond*/, string initializer) {
    CheckQueryAndUpdate(query, atts, newQueryToSlotSetMap);

    // we want to deal with the situation in which the predicate is specified as a series
    // of independent predicates

    if (filters.find(query) == filters.end()){
      // new filter
      filters[query]=expr;
    } else { // add to filer
      filters[query]= filters[query] + " && " +  expr;
    }

    if (initializers.find(query) == initializers.end()){
      // new filter
      initializers[query]=initializer;
    } else { // add to filer
      initializers[query]= initializers[query] + "\n" +  initializer;
    }

    queriesCovered.Union(query);
    return true;
}

bool LT_Selection::AddSynthesized(QueryID query, SlotID att,
        SlotSet& atts, string expr, string initializer, string defs) {
    SynthInfo sInfo(att, expr, initializer, defs);
    synthInfoMap[att] = sInfo;
    CheckQueryAndUpdate(query, atts, newQueryToSlotSetMap);
    queriesCovered.Union(query);

    // if a filter for this query does not exist, must add a "true" filter
    if (filters.find(query) == filters.end()){
        filters[query]="true";
    }

    //synthesized
    SlotSet attsSet;
    attsSet.insert(att);
    CheckQueryAndUpdate(query, attsSet, synthesized);

    if (initializers.find(query) == initializers.end()){
      // new filter
      initializers[query]=initializer;
    } else { // add to filer
      initializers[query]= initializers[query] + "\n" +  initializer;
    }

    return true;
}

// Implementation top -> down as follows per query:
// 1. used = used + new queries attributes filled after analysis
// 2. result = used + attributes coming from above - synthesized
//    (synthesized dont need to go down)
bool LT_Selection::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe) {
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();
    result.clear();
    SlotSet uni;
    set_union(atts.begin(), atts.end(), used[query].begin(), used[query].end(), inserter(uni, uni.begin()));
    set_difference(uni.begin(), uni.end(), synthesized[query].begin(), synthesized[query].end(), inserter(result, result.begin()));
    queryExit.Insert (qe);
    return true;
}

// Implementation bottom -> up as follows for all queries together:
// 1. used = used + new queries attributes filled after analysis
// 2. result = attributes coming from below + synthesized
// 3. Dont need to add new attributes received since last analysis to the result
// 4. Correctness : used is subset of what is coming from below
bool LT_Selection::PropagateUp(QueryToSlotSet& result) {
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();
    result.clear();

    // result = attributes coming from below + synthesized
    CheckQueryAndUpdate(downAttributes, result);
    CheckQueryAndUpdate(synthesized, result);

    //PrintAllQueryAndAttributes(used);
    //PrintAllQueryAndAttributes(downAttributes);

    // Correctness
    // used should be subset of what is coming from below
    if (!IsSubSet(used, downAttributes))
    {
        cout << "Selection WP : Attribute mismatch : used is not subset of attributes coming from below\n";
        return false;
    }
    downAttributes.clear();
    return true;
}

void LT_Selection::WriteM4File(ostream& out) {
    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    AttributeManager& am = AttributeManager::GetAttributeManager();

    // Need to first print all of the definitions required by our queries.
    for (QueryFilterToExpr::iterator it = filters.begin();
             it != filters.end(); ++it){
        QueryID query = it->first;
        SlotSet& sAtts = synthesized[query];
        for (SlotSet::iterator its = sAtts.begin(); its != sAtts.end(); ++its ){
            SlotID slot = *its;
            SynthInfo& synthInfo = synthInfoMap[slot];
            out << synthInfo.defs;
        }
    }

    out << "M4_SELECTION_MODULE(" << wpname << ", ";
    out << "\t</";

    // go through all queries and print the predicates
    // format "(Query, filter), ..."
    for (QueryFilterToExpr::iterator it = filters.begin();
             it != filters.end();){
        QueryID query = it->first;
        out << "( " << GetQueryName(query) << ", "
                << it->second << " ,</" << initializers[query] << "/>,  </(";

        // go through the synthesized attributes
        SlotSet& sAtts = synthesized[query];
        for (SlotSet::iterator its = sAtts.begin(); its != sAtts.end(); ){
            // get detailed attribute info and add to argument
            SlotID slot = *its;
            SynthInfo& synthInfo = synthInfoMap[slot];
            out << "(" <<  am.GetAttributeName(synthInfo.att) <<  ","
                    << synthInfo.expression <<  ")";
            ++its;
            // do we need a comma?
            if (its!=sAtts.end())
                out << ", ";
        }
        out << ")/>";

        // close up the list
        out << " )";
        ++it;
        // do we need a comma?
        if (it!=filters.end())
            out << ",";
    }

    out << "/>,\t"; // end of argument

    // print the Attributes with QueryIDSets in which they are used
    // format: (att_name, QueryIDSet_serialized), ..
    SlotToQuerySet reverse;
    AttributesToQuerySet(used, reverse);
    PrintAttToQuerySets(reverse, out);

    // dropped attributes
    // format (att1, att2, ...)

    // macro call end
    out << ")" << endl;

}
