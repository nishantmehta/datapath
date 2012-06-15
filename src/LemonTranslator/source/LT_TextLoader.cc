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
#include "LT_TextLoader.h"
#include "AttributeManager.h"
#include "WayPointConfigureData.h"
#include "ContainerTypes.h"

bool LT_TextLoader :: GetConfig (WayPointConfigureData& where) {

	// Name of waypoint
	string relation = GetWPName();

	// all query exit pairs
	QueryExitContainer queryExits;

	// dropped queries
	QueryExitContainer qExitsDone;
	// Alin: not working properly. Returns current queries          GetDroppedQueries(qExitsDone);

	// query to slot map
	QueryExitToSlotsMap queryColumnsMap;
	GetQuerExitToSlotMap(queryColumnsMap);

	WayPointID tableID = GetId ();
	/* crap from common inheritance from waypoint*/
	WorkFuncContainer myWorkFuncs;
	WorkFunc tempFunc = NULL; // NULL, will be populated in codeloader
	TextLoaderWorkFunc myWorkFunc (tempFunc);
	myWorkFuncs.Insert(myWorkFunc);

	QueryExitContainer myEndingQueryExits;
	QueryExitContainer myFlowThroughQueryExits;
	GetQueryExits(queryExits, myEndingQueryExits);

	PDEBUG("Printing query exits for TEXTLOADER WP ID = %s", GetId().getName().c_str());

	// Use pattern and count here
	StringContainer files;
	for (int i = 1; i <= count; i++) {
		char tmp[100]; // sufficient to contain one name
		sprintf(tmp, pattern.c_str(), i);
		files.push_back (tmp);
	}

	TextLoaderConfigureData loaderConfig( GetId(), myWorkFuncs, myEndingQueryExits, 
			myFlowThroughQueryExits, files, queryExits);

	where.swap(loaderConfig);

	return true;

}

bool LT_TextLoader::PropagateDown(QueryID query, const SlotSet& atts, SlotSet& rez, QueryExit qe)
{
  CheckQueryAndUpdate(newQueryToSlotSetMap, used);
  newQueryToSlotSetMap.clear();

  if (!IsSubSet(atts, used[query]))
  {
    cout << "TextLoader WP : Attributes coming from above should be subset of used\n";
    return false;
  }
	// nothing to be done for rez (result) because we have no-one to propagate down, we are bottommost
  queryExit.Insert(qe);
  return true;
}

void LT_TextLoader :: WriteM4File (ostream& out) {

  IDInfo info;
  GetId().getInfo(info);
  string wpname = info.getName();

	// 1st argument
  out << "M4_TEXTLOADER_MODULE(" << wpname << ", ";

  AttributeManager& am = AttributeManager::GetAttributeManager();

	// 2nd argument
  out << "</";
	attributesInOrder.MoveToStart();
	while (attributesInOrder.RightLength()) {

    SlotID& slot = attributesInOrder.Current();
		if (slot.IsValid())
			out << am.GetAttributeName(slot) ;
		else {
			out << "NONE";
			cout << "\n Got NULL attribute";
		}

    attributesInOrder.Advance ();
    // do we need a comma?
    if (attributesInOrder.RightLength())
      out << ", ";
  }
  out << "/>";

	// 3rd argument
	out << ", </(" << (int)separator << ")/> )";

}

