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
#include "AggregateCG.h"
#include "QueryManager.h"
#include "Errors.h"


AggregateCG::AggregateCG(SymbolicWaypointConfig &config):
	WayPointCG(config) {
	// nothing more to do
}

AggregateCG::~AggregateCG() {
	// nothing to destroy
}

void AggregateCG::GenerateM4Code(ofstream &out) {
	QueryManager& qm=QueryManager::GetQueryManager();

	// write in the aggregate waypoint macro with the name and aggregate
	// (query, function) pairs and the set of dropped attributes
	out << "M4_AGGREGATE_MODULE(" << wpname << ", " << endl;
	out << "\t[";

	// go through all the queries and for each of them extract "aggList"
	QueryIDSet qSet=myConf.GetQueries();
	string aggList("aggList");
	while (!qSet.IsEmpty()){
		QueryID q=qSet.GetFirst();
		string qName;
		if (!qm.GetQueryName(q, qName)){
			WARNING("Query not registered.");
		}
		out << "(" << qName << "," << myConf.GetStringParam(q, aggList) << ")";
		// was this the last element (qSet empty)
		if (!qSet.IsEmpty())
			out << ",\n"; // separator
	}

	out << "], )" << endl;
}

void AggregateCG::ExtractCode(void *handle, WayPointConfigureData &where) {

	// get the function names
	string aggName = "AggChunk_" + wpname;
	string finName = "FinalizeAgg_" + wpname;

	// get the symbols from the module
	void *symAgg = dlsym(handle, aggName.c_str());
	FATALIF(symAgg == NULL, "Unable to obtain function %s from module!", aggName.c_str());

	void *finAgg = dlsym(handle, finName.c_str());
	FATALIF(finAgg == NULL, "Unable to obtain function %s from module!", aggName.c_str());

	// cast in the aggChunk function
	void (*processChunk)(Chunk &, AggStorageMap &aggFuncs, pthread_mutex_t *) =
		(void (*)(Chunk &, AggStorageMap &aggFuncs, pthread_mutex_t *))symAgg;

	// cast in the finalizeAgg function
	void (*getAgg)(Chunk &, AggStorageMap &aggFuncs, QueryExitContainer &,
		TableScanID &) =
			(void (*)(Chunk &, AggStorageMap &aggFuncs, QueryExitContainer &,
				TableScanID &))finAgg;

	// make a waypoint configure object and swap it in
	AggregateWayPointConfigureData agg;
	agg.Load(processChunk, getAgg);
	agg.SetWayPointID(myConf.GetID());

	where.swap(agg);
}
