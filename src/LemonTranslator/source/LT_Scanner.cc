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
#include "LT_Scanner.h"
#include "AttributeManager.h"
#include "WayPointConfigureData.h"
#include "ScannerConfig.h"

bool LT_Scanner::GetConfig(WayPointConfigureData& where){
	// all query exit pairs
	QueryExitContainer queryExits;

	// dropped queries
	QueryExitContainer qExitsDone;
	// Alin: not working properly. Returns current queries					GetDroppedQueries(qExitsDone);

	// columns to slot?
	SlotToSlotMap columnsToSlotsMap;
	AttributeManager& am=AttributeManager::GetAttributeManager();					
	am.GetColumnToSlotMapping(relation, columnsToSlotsMap);

	// query to slot map
	QueryExitToSlotsMap queryColumnsMap;
	GetQuerExitToSlotMap(queryColumnsMap);

	WayPointID tableID = GetId ();
	/* crap from common	inheritance from waypoint*/
	WorkFuncContainer myTableScanWorkFuncs;

	QueryExitContainer myTableScanEndingQueryExits;
	QueryExitContainer myTableScanFlowThroughQueryExits;
	GetQueryExits(queryExits, myTableScanEndingQueryExits);
	PDEBUG("Printing query exits for SCANNER WP ID = %s", tableID.getName().c_str());
#ifdef DEBUG
        cout << "\nFlow through query exits\n" << flush;
        myTableScanFlowThroughQueryExits.MoveToStart();
        while (myTableScanFlowThroughQueryExits.RightLength()) {
                (myTableScanFlowThroughQueryExits.Current()).Print();
                myTableScanFlowThroughQueryExits.Advance();
        }
        cout << "\nEnding query exits\n" << flush;
        myTableScanEndingQueryExits.MoveToStart();
        while (myTableScanEndingQueryExits.RightLength()) {
                (myTableScanEndingQueryExits.Current()).Print();
                myTableScanEndingQueryExits.Advance();
        }
        cout << "\nAll scanning query exits\n" << flush;
        queryExits.MoveToStart();
        while (queryExits.RightLength()) {
                (queryExits.Current()).Print();
                queryExits.Advance();
        }
        cout << endl;
#endif


	/* end crap */
	TableConfigureData scannerConfig( tableID, 
			/* crap arguments */
			myTableScanWorkFuncs, 
			myTableScanEndingQueryExits, 
			myTableScanFlowThroughQueryExits, 
			/* end crap */
			relation, queryExits, qExitsDone, 
			queryColumnsMap, columnsToSlotsMap); 

	where.swap(scannerConfig);

	return true;
}

// This scanner will only get queries from text loader, for text loader this will not be
// called as they are bottom most nodes
void LT_Scanner::ReceiveAttributes(QueryToSlotSet& atts) {

	// allAttr is initialized at the time of constructor
	if (!IsSubSet(allAttr, atts))
	{
		cout << "ERROR Scanner WP : Attributes in scanner should be subset of attributes coming from textLoader\n";
		return;
	}

	// Save all attributes coming from text loader without any check if this scanner has that query or not
	for (QueryToSlotSet::const_iterator iter = atts.begin();
			iter != atts.end();
			++iter) {

		QueryID query = iter->first;
		// Just get all of them from textloader
		//if (DoIHaveQueries(query))  {
		SlotSet atts_s = iter->second;
		CheckQueryAndUpdate(query, atts_s, fromTextLoader);
		//}
	}
}

// Add writer capabilities to this scanner
bool LT_Scanner::AddWriter(QueryID query){
	
	CheckQueryAndUpdate(query, allAttr, newQueryToSlotSetMap);
	queriesCovered.Union(query);
	return true;
}

// This is called just before analysis to add all the queries to the scanner waypoint
bool LT_Scanner::AddScanner(QueryIDSet query)
{
	// In case AddWriter is used to add queries one by one, dont use this method
	// to add all queries, since we don't want to overwrite
	Bitstring64 tmp = query.Clone ();
	while (!tmp.IsEmpty()) {
		Bitstring64 q = tmp.GetFirst ();
		CheckQueryAndUpdate(q, allAttr, newQueryToSlotSetMap);
		queriesCovered.Union(query);
	}
	return true;
}

void LT_Scanner::DeleteQuery(QueryID query) {
	DeleteQueryCommon(query);
	dropped.erase(query);
}

void LT_Scanner::ClearAllDataStructure() {
	ClearAll();
	allAttr.clear();
	dropped.clear();
}

void LT_Scanner::GetDroppedQueries(QueryExitContainer& qe) {
	QueryExitContainer tmp;
	queryExit.MoveToStart();
	// iterate over queryExit and see it some if it matches with dropped queries
	while (queryExit.RightLength()) {
		QueryToSlotSet::const_iterator it = dropped.find((queryExit.Current()).query);
		if (it != dropped.end()){
			tmp.Insert(queryExit.Current());
		}
		queryExit.Advance();
	}
	tmp.swap(qe);
}

void LT_Scanner::GetQuerExitToSlotMap(QueryExitToSlotsMap& qe){
	AttributeManager& am = AttributeManager::GetAttributeManager();
	QueryExitToSlotsMap tmp;
	queryExit.MoveToStart();
	// iterate over queryExit and see it some if it matches with used queries
	// actually all of it should match
	while (queryExit.RightLength()) {
		QueryToSlotSet::const_iterator it = used.find((queryExit.Current()).query);
		if (it != used.end()){
			SlotContainer slotIds;
			for (SlotSet::const_iterator iter = (it->second).begin(); iter != (it->second).end(); ++iter){
				SlotID s = *iter;
				slotIds.Insert(s);
			}
			QueryExit qeTemp = queryExit.Current();
			tmp.Insert(qeTemp, slotIds);
		}
		queryExit.Advance();
	}
	tmp.swap(qe);
}

// Implementation top -> down as follows per query:
// 1. used = used + new queries attributes added since last analysis
// 2. result = used attributes - attributes coming from above 
// 3. result can be considered as dropped attributes which can be dropped
//    from further scanning. result can be stored in this class.
// 5. Save new query exit pair per query
bool LT_Scanner::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& rez, QueryExit qe)
{
	CheckQueryAndUpdate(newQueryToSlotSetMap, used);
	newQueryToSlotSetMap.clear();

	if (!IsSubSet(atts, used[query]))
	{
		cout << "Scanner WP : Attributes coming from above should be subset of used\n";
		return false;
	}
	SlotSet result;	
	set_difference((used[query]).begin(), (used[query]).end(), atts.begin(), atts.end(), inserter(result, result.begin()));
	dropped[query] = result;
	used[query]=atts;
	// this is flow through query exit, but should never be the case
	queryExit.Insert(qe);
	return true;
}

bool LT_Scanner::PropagateDownTerminating(QueryID query, const SlotSet& atts/*blank*/, SlotSet& result, QueryExit qe)
{
	CheckQueryAndUpdate(newQueryToSlotSetMap, used);
	newQueryToSlotSetMap.clear();
	// populate everything we got from text loader to send them down
	result = fromTextLoader[query];
	queryExitTerminating.Insert(qe);
	return true;
}

// Implementation bottom -> up as follows for all queries together:
// 1. used = used + new queries attributes added since last analysis
// 2. result = used (= attributes going up)
bool LT_Scanner::PropagateUp(QueryToSlotSet& result)
{
	// newQueryToSlotSetMap is populated from allAttr while AddScanner
	CheckQueryAndUpdate(newQueryToSlotSetMap, used);
	newQueryToSlotSetMap.clear();
	result.clear();
	result = used;
	if (!downAttributes.empty() && downAttributes == used) {
		cout << "ScannerWP : Attribute mismatch : used is not equal to attribute coming from below textLoader\n";
		return false;
	}
	downAttributes.clear();
	return true;
}

void LT_Scanner::WriteM4File(string dir) {
	IDInfo info;
	GetId().getInfo(info);
	cout << "SCANNER Waypoint: " << info.getName() << endl;
	cout << "--- Predicates ---" << endl;

	//cout << "USED set ===========";
	//PrintAllQueryAndAttributes(used);

	cout << "--- Attributes QuerySets --" << endl;
	SlotToQuerySet reverse;
	AttributesToQuerySet(used, reverse);

	PrintAttToQuerySets(reverse, cout);

	cout << "--- Dropped attributes ---" << endl;
	PrintAllQueryAndAttributes(dropped);
}
