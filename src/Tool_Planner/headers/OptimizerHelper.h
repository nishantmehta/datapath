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
#ifndef OPTIMIZERHELPER_H_INCLUDED
#define OPTIMIZERHELPER_H_INCLUDED

enum PredicateType;
enum Operator;

enum TraversalMethod {
    GetAllImmediateChildren, // Get all children upto 1 level
    GetAllRecursiveChildren,
    GetAllImmediateParents,
    GetAllRecursiveParents,
    GetImmediateLHSChild, //Since only 1 lhs waypoint allowed
    GetImmediateRHSChildren,
    GetRecursiveLHSChildren, // i.e LHS child(i) += (LHS child of i += (LHS child of LHS child of i))
    GetLowerMostChildren,
    GetTopMostParents,
    GetAllRecursiveChildrenAlongWithCurrent,
    GetAllMappedWaypoints, // No reference waypoint required for this traversal method
    GetAllUnMappedWaypoints, // No reference waypoint required for this traversal method
    GetAllWaypoints // No reference waypoint required for this traversal method
};

enum FilterConditions {
    TrueIfQIDsSame,
    TrueIfTablesSame, // Looks inside lhs and rhs
    TrueIfQIDsANDTablesSame,
    TrueIfQIDsORTablesSame,
    TrueIfLHSHashGroupContainsTblAtt,
    AlwayTrue // Always false doesnot make sense here; Note this however rejects the waypoints with no predicates
};

// This class is for internal use only
class GraphTraversalHelper {
public:
    __int64_t _wyptID;
    vector<__int64_t> _recIDs;
    vector<__int64_t> _recIndexes; // For faster traversal
    vector<__int64_t> _inputEdgeIDs;
    vector<__int64_t> _inputEdgeIndexes; // For faster traversal
    vector<__int64_t> _outputEdgeIDs;
    vector<__int64_t> _outputEdgeIndexes; // For faster traversal
};

// This class represent a predicate for the optimizer
class OptimizerRecord {

public:
    PredicateType _myType;
	Operator _myOp;
	string _parseString;
	string _lhsRel;
	string _rhsRel;
	string _lhsAtt;
	string _rhsAtt;
	string _filterExpression;
	__int64_t _recID; // ~ Primary Key for this record
	__int64_t _qID;

	// Initialized to -1
	__int64_t _wyptID;

	// Special case: only applicable for TopPredicate
	vector<TopAggregate> _aggregatePreds;
	vector<TopDisjunction> _disjunctionPreds;
	TopOutput _outputPred;
	TopGroupBy _groupByPred;

	__int64_t _groupID; // This is to group the join predicates that have same left and right hand side tables

    // Initialize to default values
    OptimizerRecord(__int64_t newRecID);
};

class OptimizerEdge {
public:
    __int64_t _startWyptID;
    __int64_t _endWyptID;

    __int64_t _edgeID; // Primary Key of the edge
    OptimizerEdge(__int64_t newEdgeID);
};

class OptimizerGroup {
    // Group is not same as waypoint. Groups assist in optimization process are internal construct of the optimizer
public:
    __int64_t _groupID;
    vector<string> _lhsHashRel;
	vector<string> _lhsHashAtt;
	vector<string> _rhsHashRel;
	vector<string> _rhsHashAtt;
	string _filterExpression;
	string _bypassString;
	OptimizerGroup();
};

// This is a special graph which represent multi-query graph
// but is customised for Datapath system.
class PathNetwork {

    // Network Integrator almost touches all its field
    // The reason it is a separate class is for backward compatibility and to isolate graph traversing logic from mapping/optimization logic
    friend class NetworkIntegrator;
    friend class Coster;

    // Temporary State for internal functions/data-structures
    __int64_t _groupIDCounter;
    __int64_t _recIDCounter;
    __int64_t _edgeIDCounter;
    __int64_t _wyptIDCounter;

    vector<OptimizerRecord> _records;
    vector<OptimizerEdge> _edges;
    vector<OptimizerGroup> _groups;

    #ifdef NIKE_DEBUG
    void PrintEdges();
    #endif
    // typeOfID can be "GROUP" or "RECORD" or "EDGE" or "WAYPOINT"
    __int64_t GetNewID(string typeOfID);

    __int64_t GetGroupIndex(__int64_t wyptID);

    // Donot call this directly; go through the iterator
    bool IsLHSWyptHelper(__int64_t childWypt, __int64_t parentWypt); // child --> parent edge
    // This also works for NOT_MAPPED
    vector<__int64_t> GetRecordIndexesForWaypoint(__int64_t wyptID);
    bool DoesWaypointSatisfyTheFilter(__int64_t wyptID, FilterConditions myFilter, string tblNameForFilter, string attNameForFilter, __int64_t qIDForFilter, PredicateType expectedWyptType);

    bool IsEdgePresent(__int64_t childWyptID, __int64_t parentWyptID, bool checkRecursively);

    __int64_t GetLowestWaypointWithTablesForDisjunction(__int64_t queryID, vector<string> tblNames);
    __int64_t GetLowestCommonAncestor(__int64_t queryID, set<__int64_t> wypts);
    __int64_t GetLowestCommonAncestorHelper(__int64_t queryID, __int64_t wypt1, __int64_t wypt2);

    // Works only for acylic path network
    vector<__int64_t> GetTopologicallySortedWaypointIDs();

    // --------------------- Iterator -------------------------

    // Very powerful iterator
    // Allows u to find children, parents, siblings nodes and many more combinations
    // Make sure you fill in appropriate _tblNamesForFilter or _qIDsForFilter if u intend to use the filter
    // Use NAPredType is u don't care about the waypoint type
    set<__int64_t> GetSpecificWypts(__int64_t referenceWypt, TraversalMethod myTraversalMethod, FilterConditions myFilter,
        string tblNameForFilter, string attNameForFilter, __int64_t qIDForFilter, PredicateType expectedWyptType);

    // This extra method is required since GetAllUnMappedWyptIDs will simply return set of length 1 => NOT_MAPPED
    set<__int64_t> GetUnMappedRecordIndexes(FilterConditions myFilter, string tblNameForFilter, string attNameForFilter, __int64_t qIDForFilter, PredicateType expectedWyptType);

    // Some Eg Usage:
    // 1. DoesWaypointHaveTableRecursive: For A(nation,blah) -> B(region,blah) -> C with C as refWypt => True => (GetAllRecursiveChildrenAlongWithCurrent, TrueIfQIDsANDTablesSame)
    // 2. GetTopWaypointIDsForQuery => (GetTopMostParents, TrueIfQIDsSame)
    // 3. GetTopWaypointIDsForTable => (GetTopMostParents, TrueIfQIDsANDTablesSame)
    // 4. GetAllPossibleChildrenWyptIDs => (GetAllRecursiveChildren, AlwayTrue)
    // -----------------------------------------------------

public:
    PathNetwork();
    PathNetwork(const PathNetwork&);
};


#include "Coster.h"

// -------------------------- Utility classes -------------

#include "OptimizerUtility.h"

// --------------------------------------------------------

#endif // OPTIMIZERHELPER_H_INCLUDED
