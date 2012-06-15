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
#include "SelectionCG.h"
#include "QueryManager.h"
#include "Errors.h"

SelectionCG::SelectionCG(SymbolicWaypointConfig &config):
	WayPointCG(config){
	// nothing more to do
}

SelectionCG::~SelectionCG() {
	// nothing to destroy
}

void SelectionCG::GenerateM4Code(ofstream &out) {
	QueryManager& qm=QueryManager::GetQueryManager();

	// write in the code with the selections attribute
	out << "M4_SELECTION_MODULE(" << wpname << ", " << endl;
	out << "\t[";

	// go through all the queries and for each of them extract "printList"
	QueryIDSet qSet=myConf.GetQueries();
	string selection("selection");
	while (!qSet.IsEmpty()){
		QueryID q=qSet.GetFirst();
		string qName;
		if (!qm.GetQueryName(q, qName)){
			WARNING("Query not registered");
		}
		out << "(" << qName << "," << myConf.GetStringParam(q, selection) << ")";
		// was this the last element (qSet empty)
		if (!qSet.IsEmpty())
			out << ",\n"; // separator
	}

	out << "], )" << endl;
}

void SelectionCG::ExtractCode(void *handle, WayPointConfigureData &where) {
	// get the exact function name.
	string funName = "ProcessSelection_" + wpname;

	// obtain the symbol from the module.
	void *sym = dlsym(handle, funName.c_str());
	FATALIF(sym == NULL, "Unable to obtain function %s from module!", funName.c_str());

	// cast the symbol (this is temporary!) and create the config structure

	int (*csym)(Chunk &) = (int (*)(Chunk &))sym;
	SelectionWayPointConfigureData sel;
	sel.Load(csym);
	sel.SetWayPointID(myConf.GetID());

	where.swap(sel);
}
