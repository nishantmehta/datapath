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
#include "JoinCG.h"
#include "QueryManager.h"


JoinCG::JoinCG(SymbolicWaypointConfig &config):
	WayPointCG(config) {
	// nothing more to do
}

JoinCG::~JoinCG() {
	// nothing to destroy
}

void JoinCG::GenerateM4Code(ofstream &out) {
	QueryManager& qm=QueryManager::GetQueryManager();

	// write in the left join waypoint macro with the name, left hash
	// attributes and (query, predicate) pairs
	out << "M4_JOIN_MODULE(" << wpname << ", " << endl;
	out << "\t[" << myConf.GetStringParam("lefthash") << "], [" << endl;

	// go through all the queries and for each of them extract "printList"
	QueryIDSet qSet=myConf.GetQueries();
	while (!qSet.IsEmpty()){
		QueryID q=qSet.GetFirst();
		string qName;
		if (!qm.GetQueryName(q, qName)){
			WARNING("Query not registered");
		}
		out << "(" << qName << "," << myConf.GetStringParam(q, "join") << ")";
		// was this the last element (qSet empty)
		if (!qSet.IsEmpty())
			out << ",\n"; // separator
	}

	out << "])" << endl;
}

void JoinCG::ExtractCode(void *handle, WayPointConfigureData &where) {
	// get the function names
	string rhsName = "JoinRHS_" + wpname;
	string probeName = "Probe_" + wpname;

	// get the symbols from the module
	void *rhsSym = dlsym(handle, rhsName.c_str());
	FATALIF(rhsSym == NULL, "Unable to obtain function %s from module!",
		rhsName.c_str());

	void *probeSym = dlsym(handle, probeName.c_str());
	FATALIF(probeSym == NULL, "Unable to obtain function %s from module!",
		probeName.c_str());

	// cast both functions
	int (*probePtr)(Chunk &, HashTable &, HashTable &, int, Chunk &) =
		(int (*)(Chunk &, HashTable &, HashTable &, int, Chunk &))probeSym;
	int (*rhsPtr)(Chunk &, HashTable &, HashTable &, HT_INDEX_TYPE, HT_INDEX_TYPE,
		HashesToRunList&, ChunkHeaderList&) =
			(int (*)(Chunk &, HashTable &, HashTable &, HT_INDEX_TYPE, HT_INDEX_TYPE,
				HashesToRunList&, ChunkHeaderList&))rhsSym;

	// create a type and swap it in
	JoinWayPointConfigureData join;
	join.Load(probePtr, rhsPtr);
	join.SetWayPointID(myConf.GetID());

	where.swap(join);
}
