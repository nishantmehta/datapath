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
#include "LT_Aggregate.h"
#include "AttributeManager.h"

bool LT_Aggregate::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID aggIDOne = GetId ();

    // first, get the function we will send to it
    WorkFunc tempFunc = NULL;
    AggOneChunkWorkFunc myAggOneWorkFunc (tempFunc);
    tempFunc = NULL;
    AggFinishUpWorkFunc myAggOneFinishUpWorkFunc (tempFunc);
    WorkFuncContainer myAggOneWorkFuncs;
    myAggOneWorkFuncs.Insert (myAggOneWorkFunc);
    myAggOneWorkFuncs.Insert (myAggOneFinishUpWorkFunc);

    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer myAggOneEndingQueryExits;
    QueryExitContainer myAggOneFlowThroughQueryExits;
    GetQueryExits (myAggOneFlowThroughQueryExits, myAggOneEndingQueryExits);
    PDEBUG("Printing query exits for AGGREGATE WP ID = %s", aggIDOne.getName().c_str());
#ifdef DEBUG
        cout << "\nFlow through query exits\n" << flush;
        myAggOneFlowThroughQueryExits.MoveToStart();
        while (myAggOneFlowThroughQueryExits.RightLength()) {
                (myAggOneFlowThroughQueryExits.Current()).Print();
                myAggOneFlowThroughQueryExits.Advance();
        }
        cout << "\nEnding query exits\n" << flush;
        myAggOneEndingQueryExits.MoveToStart();
        while (myAggOneEndingQueryExits.RightLength()) {
                (myAggOneEndingQueryExits.Current()).Print();
                myAggOneEndingQueryExits.Advance();
        }
        cout << endl;
#endif


    // here is the waypoint configuration data
    AggregateConfigureData aggOneConfigure (aggIDOne, myAggOneWorkFuncs, myAggOneEndingQueryExits, myAggOneFlowThroughQueryExits);

    // and add it
    where.swap (aggOneConfigure);

    return true;
}

void LT_Aggregate::DeleteQuery(QueryID query)
{
    DeleteQueryCommon(query); // common data
    synthesized.erase(query);
    aggAttribs.erase(query);
}

void LT_Aggregate::ClearAllDataStructure() {
    ClearAll(); // common data
    aggAttribs.clear();
    aggInfoMap.clear();
    synthesized.clear();
}

bool LT_Aggregate::AddAggregate(QueryID query,
        SlotID slot, /* attribute corresponding to aggregate */
        string aggregateType, /* type, just pass along */
        SlotSet& atts, string expr, string initializer,
        string defs)
{
    aggAttribs[query].insert(slot);
    AggInfo aggInfo(slot, aggregateType, expr, initializer, defs);
    aggInfoMap[slot] = aggInfo;
    CheckQueryAndUpdate(query, atts, newQueryToSlotSetMap);
    queriesCovered.Union(query);
    //synthesized
    SlotSet attsSet;
    attsSet.insert(slot);
    CheckQueryAndUpdate(query, attsSet, synthesized);


    if (initializers.find(query) == initializers.end()){
      // new filter
      initializers[query]=initializer;
    } else { // add to filer
      initializers[query]= initializers[query] + "\n" +  initializer;
    }

}

// atts coming from top is dropped as it will not be propagated down
// that also means, all attributes coming from top are synthesized ones
// 1. used = used + new queries attributes filled after analysis
// 2. result goes down = used
// 3. Correctness : Atts coming from top is subset of synthesized
bool LT_Aggregate::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& result, QueryExit qe)
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

// 1. used = new queries filled after analysis
// 2. result goes up = synthesized
// 3. Correctness : used is subset of what is coming from below
bool LT_Aggregate::PropagateUp(QueryToSlotSet& result)
{
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();

    result.clear();
    result = synthesized;

    // used should be subset of what is coming from below
    if (!IsSubSet(used, downAttributes))
    {
        cout <<  "AggregateWP : Attribute mismatch : used is not subset of attributes coming from below\n";
        return false;
    }
    return true;
}

void LT_Aggregate::WriteM4File(ostream& out) {
    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    AttributeManager& am = AttributeManager::GetAttributeManager();

    // Note: This anonymous namespace is here so that the same GLAs and
    // functions may be defined in separate files without the linker
    // complaining. The anonymous namespace restricts the linkage visibility
    // of everything in it to this file only.
    out << "namespace {" << endl;
    for (QueryToSlotSet::iterator it = aggAttribs.begin();
             it != aggAttribs.end(); ++it){

        for (SlotSet::iterator its = (it->second).begin(); its != (it->second).end(); ++its ){
            // get detailed attribute info and add to argument
            SlotID slot = *its;
            AggInfo aggInfo = aggInfoMap[slot];

            out << aggInfo.defs;
        }
    }
    out << "}";

    out << "M4_AGGREGATE_MODULE(" << wpname << ", ";
    out << "\t</";

    // go through the query to attribute map
    for (QueryToSlotSet::iterator it = aggAttribs.begin();
             it != aggAttribs.end();){
        out << "( " << GetQueryName(it->first) << ", </";
        out << initializers[it->first] << "/>, ";
        // go through the attributes
        for (SlotSet::iterator its = (it->second).begin(); its != (it->second).end(); ){
            // get detailed attribute info and add to argument
            SlotID slot = *its;
            AggInfo aggInfo = aggInfoMap[slot];
            out << "(" <<  am.GetAttributeName(aggInfo.att) <<  ","
                    << aggInfo.type <<  "," << aggInfo.expression <<  ")";
            ++its;
            // do we need a comma?
            if (its!=(it->second).end())
                out << ", ";
        }

        // close up the list
        out << ")";
        ++it;
        // do we need a comma?
        if (it != aggAttribs.end())
            out << ",";
    }
    out << "/>,\t"; // end of argument

    // format: (att_name, QueryIDSet_serialized), ..
    SlotToQuerySet reverse;
    AttributesToQuerySet(used, reverse);
    PrintAttToQuerySets(reverse, out);

    // macro call end
    out << ")" << endl;

}
