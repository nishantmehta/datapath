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
#ifndef OPTIMIZER1_H_INCLUDED
#define OPTIMIZER1_H_INCLUDED

#define SYSTEM_PIPE_NAME "../../Tool_DataPath/executable/EXECUTE"

#define OPTIMIZER_FAIL_IF(cond, coutStr)    \
    if(cond) {                              \
        cout << coutStr << endl;            \
        exit(0);                            \
    }

// Defines that are used as Interface
#define GREEDY_SEARCH 0
#define LOOKAHEAD_SEARCH 1
#define NOT_MAPPED -1
#define END_OF_GRAPH -2
#define IGNORE_NAME "$IGNORE$"
#define NO_GROUP_CARTESIAN -2
#define NO_GROUP_OTHER -3

// Internal Defines used only by the optimizer
#define ALL_TABLESCAN_SELECTION_MAPPED 1
#define ALL_JOIN_MAPPED 2

// ^^^^^^^^^^^^^^^^^^^^^^^^^ Configuration for the Optimizer ^^^^^^^^^^^^^^^^^
#define NUMBER_LOOKAHEADS 1
#define NIKE_DEBUG
#define CONSTANT_WEIGHT 10
#define OPTIMIZE_FOR_EXECUTION_ENGINE
// This means if there are 3 different join predicate from same two tables say (lineitem(L), partsupp(P))
// say predicates are L.A=P.B, L.C=P.D, L.E=P.F
// we create wypts with following hash {{L.A=P.B}, {L.C=P.D}, {L.E=P.F}, {P.B=L.A}, {P.D=L.C}, {P.F=L.E}, {L.A=P.B AND L.C=P.D}, {L.A=P.B AND L.E=P.F}, ....}
// If you think optimization is going slow, simply comment the following line. This will only create 2 wypt with hash {{L.A=P.B AND L.C=P.D L.E=P.F}, {P.B=L.A AND P.D=L.C AND P.F=L.E} }
// #define ALLOW_SUBSET_JOIN_GROUPING_WHILE_CREATION
// #define ALLOW_SUBSET_JOIN_GROUPING_WHILE_MAPPING
#define BYPASS_LIMIT 10
// #define ALLOW_JOIN_BYPASSING

// Subi: Comment or uncomment this
#define OPTIMIZE_SELECTIONS
// #define JOIN_COSTING_SCENARIO1

// This is a debugging flag. Please comment this if you are not debugging the Planner
#define PRINT_COST_PATH_NETWORK

// Standard Include files
#include <vector>
#include <set>
#include <algorithm>
#include <sstream>
#include <fstream>

// Catalog Include Files
#include "Catalog.h"
#include "TopPredicates.h"

// This optimizer is only LookAhead based and uses DataFlow Selectors
// and doesnot use minisearch and is Increemental rather than Batch Query Optimizer

// ############## Interface to the Parser  ##############
enum PredicateType {
	// Inner joins
	Join, Selection, TableScan, Top, Aggregate, Output, NAPredType
};
enum Operator {
  EQ, NEQ, LT, GT, LEQ, GEQ, LIKE, NLIKE, FUNCTION, CARTESIAN, NotApplicable
};

class OptimizerConfiguration
{
public:
    string _costerMode; // DataFlow
    string _searchType; // LookAhead Greedy Exhaustive
};

#include "OptimizerHelper.h"

// This is an interface class to Parser
// PathNetwork handles only graph traversing logic
// while NetworkIntegrator handles other optimization operations (eg: Mapping, Costing, Printing )
class NetworkIntegrator
{
    set<__int64_t> _distinctWypts;

    PathNetwork _myNetwork;
    set<__int64_t> _removeRecID;

    // Helper functions
    void MapTableScans(__int64_t queryID);
    void MapSelections(__int64_t queryID);
    pair< string, string > GetTablesInvolved(__int64_t recIndex); // Call it on join only
    vector<NetworkIntegrator> CreateNewJoinWypt(__int64_t queryID, pair<string, string> tblsInvolved, set<__int64_t> unMappedJoinIndexes, __int64_t topWypt1, __int64_t topWypt2);
    vector<NetworkIntegrator> MapJoin(__int64_t queryID, pair<string, string> tblsInvolved, set<__int64_t> unMappedJoinIndexes);
    // If all possible is set as false only returns union set rather than power set
    set<OptimizerGroup> GetGroups(set<__int64_t> unMappedJoinIndexes, bool allPossible);

public:
    Coster _myCoster;
    // __int64_t GetCost(); // global cost for all the queries

    OptimizerConfiguration* _myPtrConfig;

    NetworkIntegrator();
    // Deep copy functions
    NetworkIntegrator(const NetworkIntegrator& copyMe);
    NetworkIntegrator& operator=(const NetworkIntegrator& copyMe);

    // -------------- Functions called by the optimizer -------------
    __int64_t _stateOfNI;

    void InsertPredicate(PredicateType myType, Operator myOp, string parseString, string lhsRel, string rhsRel, string lhsAtt, string rhsAtt, __int64_t qID);
    void InsertTopPredicate(vector<TopAggregate> aggregatePreds, vector<TopDisjunction> disjunctionPreds, TopOutput outputPred, TopGroupBy groupByPred, __int64_t qID);

    void ConvertPathNetworkToDotFile(string outputFileName);
    void ConvertPathNetworkToDPFile(string outputFileName, set<__int64_t> oldQueries, map<__int64_t, string>& oldWaypoints);

    set<__int64_t> GetRemainingQueryIDs();
    bool IsIntegrationDone(__int64_t queryID);

    // Super important functions
    vector<NetworkIntegrator> Enumerate(__int64_t queryID); // Since only increemental optimization supported

    // Post-Processing step: Called only by the Optimizer
    void MapTopAndCartesianProducts(__int64_t queryID);

    set<__int64_t> GetAllQueries();
};

class Optimizer
{
    set<__int64_t> oldQueries;
    map<__int64_t, string> oldWaypoints;

    NetworkIntegrator _nextNI; // We support only Greedy Optimizers (or LookAhead) :)
	string _outputFileName;
	//__int64_t _searchType;
	OptimizerConfiguration _myConfig;

	FILE* systemPipe;


public:

	void LoadConfiguration();
    // ############## Interface to the Parser  ##############


    // Step 1: Initialize the optimizer
    Optimizer(NetworkIntegrator& initialNI, string outputFileName, __int64_t searchType);

    // Step 2: ReInitialize the optimizer
    void ReInitialize();

    // Step 3: Insert the predicates of the query
    void InsertPredicate(PredicateType myType, Operator myOp, string parseString,
        string lhsRel, string rhsRel, string lhsAtt, string rhsAtt, __int64_t qID);
    void InsertTopPredicate(vector<TopAggregate> aggregates, vector<TopDisjunction> disjunctions,
        TopOutput output, TopGroupBy groupBy, __int64_t qID);

    // Step 4: Run the optimizer. Then restart from step 2.
    void Optimize();

    // method to produce the output for the system
    void ProduceOutput();


    // ########################################################

    // Super important functions
	void LookAheadSearch(__int64_t queryID); // Uses nextNI enumerates it and replaces it with next NI
	// void GreedySearch(__int64_t queryID);

};

// ########################################################

#endif // OPTIMIZER1_H_INCLUDED
