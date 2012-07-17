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
#include "LT_Print.h"
#include "WayPointConfigureData.h"
#include <ctime>
#include "Stl.h"


bool LT_Print::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID printIDOne = GetId ();

    // first, get the function we will send to it
    WorkFunc tempFunc = NULL;
    PrintWorkFunc myPrintOneWorkFunc (tempFunc);
    WorkFuncContainer myPrintOneWorkFuncs;
    myPrintOneWorkFuncs.Insert (myPrintOneWorkFunc);


    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer myPrintOneEndingQueryExits;
    QueryExitContainer myPrintOneFlowThroughQueryExits;
    GetQueryExits (myPrintOneFlowThroughQueryExits, myPrintOneEndingQueryExits);

    PDEBUG("Printing query exits for PRINT WP ID = %s", printIDOne.getName().c_str());
#ifdef DEBUG
        cout << "\nFlow through query exits\n" << flush;
        myPrintOneFlowThroughQueryExits.MoveToStart();
        while (myPrintOneFlowThroughQueryExits.RightLength()) {
                (myPrintOneFlowThroughQueryExits.Current()).Print();
                myPrintOneFlowThroughQueryExits.Advance();
        }
        cout << "\nEnding query exits\n" << flush;
        myPrintOneEndingQueryExits.MoveToStart();
        while (myPrintOneEndingQueryExits.RightLength()) {
                (myPrintOneEndingQueryExits.Current()).Print();
                myPrintOneEndingQueryExits.Advance();
        }
        cout << endl;
#endif

        QueryToFileInfoMap info;
        FOREACH_STL(el, colNames){
            QueryID query = el.first;
            string header = el.second;
            header += "\n"+colTypes[query]+"\n";
            string fileName = "RESULTS/";
            if( fileOut[query] == "" )
            {
                fileName+=GetQueryName(query)+".csv";
                time_t date = time(NULL);
                struct tm* ptm = localtime(&date);
                char buff[64];
                strftime(buff, 64, "%m.%d.%Y %H:%M:%S", ptm);
                fileName+=" ";
                fileName+=buff;
            }
            else {
                fileName+=fileOut[query];
            }
            pair<string,string> inf(fileName, header);
            FileInfoObj fInfo(inf);
            info.Insert(query, fInfo);
        }END_FOREACH;

    // here is the waypoint configuration data
        PrintConfigureData printOneConfigure (printIDOne, myPrintOneWorkFuncs, myPrintOneEndingQueryExits, myPrintOneFlowThroughQueryExits, info);

    where.swap (printOneConfigure);

    return true;
}

void LT_Print::ReceiveAttributesTerminating(QueryToSlotSet& atts)
{
  for (QueryToSlotSet::const_iterator iter = atts.begin();
                                      iter != atts.end();
                                      ++iter) {
    QueryID query = iter->first;
    if (DoIHaveQueries(query))  {
      SlotSet atts_s = iter->second;
      CheckQueryAndUpdate(query, atts_s, downAttributes);
    }
  }
}

void LT_Print::DeleteQuery(QueryID query)
{
    DeleteQueryCommon(query);
    print.erase(query);
}

void LT_Print::ClearAllDataStructure() {
    ClearAll();
    print.clear();
}
bool LT_Print::AddPrint(QueryID query, SlotSet& atts, string expr, string initializer, string name, string type, string file, string defs)
{

    cout << "Adding query " << GetQueryName(query) << endl;
    bool isNew = CheckQueryAndUpdate(query, atts, newQueryToSlotSetMap);

    print[query] += ","+expr;
    initializers[query] += initializer;
    definitions[query] += defs;

    if (isNew){
        colNames[query] = name;
        colTypes[query] = type;
        fileOut[query] = file;
    } else {
        colNames[query] += ","+name;
        colTypes[query] += ","+type;
    }

    queriesCovered.Union(query);
    return true;
}

// Implementation top -> down as follows per query:
// 1. used = used + new attributes added since last analysis
// 2. clear the new attributes
// 3. result = used attributes
bool LT_Print::PropagateDownTerminating(QueryID query, const SlotSet& atts/*blank*/, SlotSet& result, QueryExit qe)
{

    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();
    result.clear();
    result = used[query];
    queryExitTerminating.Insert(qe);
    return true;
}

// Implementation bottom -> up as follows for all queries together:
// 1. used = used + new queries attributes added since last analysis
// 2. clear the new data
// 3. result = NONE
// 4. Print is last destination hence result is blank
// 5. old used + new = used is good to check correctness if they are subset of down attributes
bool LT_Print::PropagateUp(QueryToSlotSet& result)
{
    CheckQueryAndUpdate(newQueryToSlotSetMap, used);
    newQueryToSlotSetMap.clear();
    result.clear();

    // Correctness
    // used should be subset of what is coming from below
    if (!IsSubSet(used, downAttributes))
    {
        cout << "Print WP : Attribute mismatch : used is not subset of attributes coming from below\n";
        return false;
    }
    downAttributes.clear();
    return true;
}

void LT_Print::WriteM4File(ostream& out) {
    IDInfo info;
    GetId().getInfo(info);
    string wpname = info.getName();

    // Note: This anonymous namespace is here so that the same GLAs and
    // functions may be defined in separate files without the linker
    // complaining. The anonymous namespace restricts the linkage visibility
    // of everything in it to this file only.
    //out << "namespace {" << endl;
    for (QueryToPrintString::iterator it = definitions.begin();
             it != definitions.end(); ++it){
        out << it->second;
    }
    //out << "}";

    // print module call
    out << "M4_PRINT_MODULE(" << wpname << ", ";
        out << "\t</";

    // go through all queries and print the predicates
    // format "(Query, print), ..."

    for (QueryToPrintString::iterator it = print.begin();
             it != print.end();){
        out << "( " << GetQueryName(it->first)
                 << it->second << " )";
        ++it;
        // do we need a comma?
        if (it!=print.end())
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
