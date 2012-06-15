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
#include "OptimizerHelper.h"

vector<__int64_t> PathNetwork :: GetTopologicallySortedWaypointIDs() {
	vector<__int64_t> sortedIDs;
	set<__int64_t> sortedIDSet; // For efficient find - It contains same IDs as sortedIDs

	set<__int64_t> tableScanIDs = GetSpecificWypts(-1, GetAllMappedWaypoints, AlwayTrue,
        	IGNORE_NAME, IGNORE_NAME, -1, TableScan);
	for(set<__int64_t>::iterator it = tableScanIDs.begin(); it != tableScanIDs.end(); it++) {
		if(sortedIDSet.find(*it) == sortedIDSet.end()) {
			// No duplicate IDs in the sortedIDs
			sortedIDs.push_back(*it);
			sortedIDSet.insert(*it);
		}
	}

	set<__int64_t> allWyptIDs = GetSpecificWypts(-1, GetAllMappedWaypoints, AlwayTrue,
        	IGNORE_NAME, IGNORE_NAME, -1, NAPredType);

	while(sortedIDs.size() != allWyptIDs.size()) {
		for(set<__int64_t>::iterator it = allWyptIDs.begin(); it != allWyptIDs.end(); it++) {
			if(sortedIDSet.find(*it) == sortedIDSet.end()) {
				// No duplicate IDs in the sortedIDs
				
				bool insertIntoSortedIDs = true;

				// Are all the input to this waypoint in the sortedID
				for(set<__int64_t>::iterator it1 = allWyptIDs.begin(); it1 != allWyptIDs.end(); it1++) {
					if(*it == *it1) 
						continue;

					// child --> parent edge
					if(!IsEdgePresent(*it1, *it, false))
						continue;

					if(sortedIDSet.find(*it1) == sortedIDSet.end()) {
						// The input waypoint *it1 is not present in the sorted list. Hence donot insert
						insertIntoSortedIDs = false;
						break;
					}

				}

				if(insertIntoSortedIDs) {
					sortedIDs.push_back(*it);
					sortedIDSet.insert(*it);
				}
				
			}
		}
	}

	return sortedIDs;
}

OptimizerRecord :: OptimizerRecord(__int64_t newRecID) {
    _recID = newRecID;

    // All other initialized to default values
    _myType = NAPredType;
	_myOp = NotApplicable;

	_lhsRel = IGNORE_NAME;
	_rhsRel = IGNORE_NAME;
	_lhsAtt = IGNORE_NAME;
	_rhsAtt = IGNORE_NAME;

	_qID = NOT_MAPPED;
	_wyptID = NOT_MAPPED;
	_groupID = NOT_MAPPED;

	_parseString = "";
	_filterExpression = "";
}

OptimizerEdge :: OptimizerEdge(__int64_t newEdgeID) {
    _edgeID = newEdgeID;

    _startWyptID = NOT_MAPPED;
    _endWyptID = NOT_MAPPED;
}

PathNetwork :: PathNetwork() {
    _groupIDCounter = 0;
    _edgeIDCounter = 0;
    _recIDCounter = 0;
    _wyptIDCounter = 0;
}

// Returns -1 if no group found
__int64_t PathNetwork :: GetGroupIndex(__int64_t wyptID) {
    for(__int64_t i = 0; i < _records.size(); i++) {
        if(_records[i]._wyptID == wyptID) {
            for(__int64_t j = 0; j < _groups.size(); j++) {
                if(_groups[j]._groupID == _records[i]._groupID)
                    return j;
            }
            return -1;
        }
    }

    cout << "Error: No waypoint with ID " << wyptID << " present.\n";
    exit(0);
}

__int64_t PathNetwork :: GetNewID(string typeOfID) {
    if(typeOfID.compare("GROUP") == 0) {
        return _groupIDCounter++;
    }
    else if(typeOfID.compare("RECORD") == 0) {
        return _recIDCounter++;
    }
    else if(typeOfID.compare("EDGE") == 0) {
        return _edgeIDCounter++;
    }
    else if(typeOfID.compare("WAYPOINT") == 0) {
        return _wyptIDCounter++;
    }
    else {
        cout << "Error: Expected typeID = GROUP or RECORD or EDGE or WAYPOINT but passed: " << typeOfID << endl;
        exit(0);
    }
}


bool PathNetwork :: IsLHSWyptHelper(__int64_t childWypt, __int64_t parentWypt)
{
    // This method doesnot check whether child for parent relationships
    vector<__int64_t> parentRecIndexes = GetRecordIndexesForWaypoint(parentWypt);
    vector<__int64_t> childRecIndexes = GetRecordIndexesForWaypoint(childWypt);

    assert(parentRecIndexes.size() != 0 && childRecIndexes.size() != 0); // Check for wypts with no predicates :(

    // Logic: For this to be LHSWyptHelper, (child & children of child) should contain lhsHash table of parent
    set<string> lhsHashTablesOfParent;

    // ------------------------------------------------------------------
    // First fill in lhsHashTablesOfParent
    bool groupFound = false;
    for(__int64_t i = 0; i < _groups.size(); i++) {
        if(_groups[i]._groupID == _records[parentRecIndexes[0]]._groupID) {
            groupFound = true;
            for(__int64_t j = 0; j < (_groups[i])._lhsHashRel.size(); j++) {
                lhsHashTablesOfParent.insert((_groups[i])._lhsHashRel[j]);
            }
            break;
        }
    }
    if(!groupFound) {
        cout << "Error: No hash group found for the waypoint with ID " << parentWypt << ". Hint: Are you calling Iterator for non-join waypoints with LHSChilds\n";
        exit(0);
    }
    assert(lhsHashTablesOfParent.size() != 0);
    // ------------------------------------------------------------------

    set<__int64_t> childNItsChildren = GetSpecificWypts(childWypt, GetAllRecursiveChildrenAlongWithCurrent, AlwayTrue, IGNORE_NAME, IGNORE_NAME, -1, NAPredType);

    // Now start removing hash tbls when match if set is empty at end, return true; else return false
    for(set<__int64_t>::iterator it = childNItsChildren.begin(); it != childNItsChildren.end(); it++) {
        vector<__int64_t> childNItsChildrenRecIndexes = GetRecordIndexesForWaypoint(*it);
        for(__int64_t j = 0; j < childNItsChildrenRecIndexes.size(); j++) {
            string lhsRel = _records[childNItsChildrenRecIndexes[j]]._lhsRel;
            lhsHashTablesOfParent.erase(lhsRel); // Returns zero if lhsRel not in the set; we simply ignore it
        }
    }

    if(lhsHashTablesOfParent.size() == 0)
        return true;
    else
        return false;
}

// ---------------- Iterators ------------------

set<__int64_t> PathNetwork :: GetUnMappedRecordIndexes(FilterConditions myFilter, string tblNameForFilter, string attNameForFilter,
    __int64_t qIDForFilter, PredicateType expectedWyptType)
{
    set<__int64_t> retVal;
    for(__int64_t i = 0; i < _records.size(); i++) {
        if( _records[i]._wyptID == NOT_MAPPED &&
            (expectedWyptType == NAPredType || _records[i]._myType == expectedWyptType)
        ) {
            switch(myFilter) {
                case TrueIfQIDsSame:
                    if(_records[i]._qID == qIDForFilter) {
                        retVal.insert(i);
                    }
                    break;
                case TrueIfTablesSame:
                    if((tblNameForFilter.compare(_records[i]._lhsRel) == 0 || tblNameForFilter.compare(_records[i]._rhsRel) == 0)) {
                        retVal.insert(i);
                    }
                    break;
                case TrueIfQIDsANDTablesSame:
                    if( (tblNameForFilter.compare(_records[i]._lhsRel) == 0 || tblNameForFilter.compare(_records[i]._rhsRel) == 0)
                        && _records[i]._qID == qIDForFilter) {
                        retVal.insert(i);
                    }
                    break;
                case TrueIfQIDsORTablesSame:
                    if( (tblNameForFilter.compare(_records[i]._lhsRel) == 0 || tblNameForFilter.compare(_records[i]._rhsRel) == 0)
                        || _records[i]._qID == qIDForFilter) {
                        retVal.insert(i);
                    }
                    break;
                case TrueIfLHSHashGroupContainsTblAtt:
                    cout << "Error: Donot call GetUnMappedRecordIndexes with filter TrueIfLHSHashGroupContainsTblAtt\n";
                    exit(0);
                    break;
                case AlwayTrue:
                    retVal.insert(i);
                    break;
            }
        }

    }

    return retVal;
}

__int64_t PathNetwork :: GetLowestCommonAncestorHelper(__int64_t queryID, __int64_t wypt1, __int64_t wypt2) {
    set<__int64_t> parents1 = GetSpecificWypts(wypt1, GetAllRecursiveParents, TrueIfQIDsSame,
        IGNORE_NAME, IGNORE_NAME, queryID, Join);
    set<__int64_t> parents2 = GetSpecificWypts(wypt1, GetAllRecursiveParents, TrueIfQIDsSame,
        IGNORE_NAME, IGNORE_NAME, queryID, Join);
    set<__int64_t> commonParents;
    set_intersection(   parents1.begin(), parents1.end(),   // Set 1
                        parents2.begin(), parents2.end(),   // Set 2
                        inserter(commonParents,commonParents.begin()) );        // Intersection Set
    assert(commonParents.size() > 0);
    set<__int64_t>::iterator it = commonParents.begin();
    __int64_t lowest = *it;
    it++;
    for(; it != commonParents.end(); it++) {
        if(IsEdgePresent(*it, lowest, true)) {
            lowest = *it;
        }
        else {
            assert(IsEdgePresent(lowest, *it, true));
        }
    }
    return lowest;
}

__int64_t PathNetwork :: GetLowestCommonAncestor(__int64_t queryID, set<__int64_t> wypts) {
    if(wypts.size() == 1)
        return *(wypts.begin());

    set<__int64_t>::iterator it = wypts.begin();
    __int64_t wypt1 = *it;
    it++;
    __int64_t wypt2 = *it;
    __int64_t lca = GetLowestCommonAncestorHelper(queryID, wypt1, wypt2);
    wypts.erase(wypt1);
    wypts.erase(wypt2);
    wypts.insert(lca);
    return GetLowestCommonAncestor(queryID, wypts);
}

__int64_t PathNetwork :: GetLowestWaypointWithTablesForDisjunction(__int64_t queryID, vector<string> tblNames) {
    set<__int64_t> wypts;
    for(__int64_t i = 0; i < tblNames.size(); i++) {
        set<__int64_t> temp = GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfQIDsANDTablesSame,
            tblNames[i], IGNORE_NAME, queryID, Selection);
        assert(temp.size() == 1);
        wypts.insert(*(temp.begin()));
    }

    return GetLowestCommonAncestor(queryID, wypts);
}

set<__int64_t> PathNetwork :: GetSpecificWypts(__int64_t referenceWypt, TraversalMethod myTraversalMethod,
    FilterConditions myFilter, string tblNameForFilter, string attNameForFilter, __int64_t qIDForFilter, PredicateType expectedWyptType) {
    if(myTraversalMethod == GetAllUnMappedWaypoints) {
        cout << "Error: Do not use GetSpecificWypts for getting unmapped waypoints. Instead use GetUnMappedRecordIndexes\n";
        exit(0);
    }

    bool noMoreWyptsAdded = true;
    // Use set to avoid multiple recursion calls
    set<__int64_t> retVal;
    set<__int64_t> relativeWypts; // Either parents or children
    set<__int64_t> :: iterator it; // For Iterating, since cannot define in switch case
    __int64_t i, j; //For Iterating

    __int64_t prevSize;
    bool firstTime = true;
    bool innerConditionFlag;
    set<__int64_t> allRecursiveRelative;
    set<__int64_t> :: iterator it1, it2;
    set<__int64_t> diffRelativeWypts; // To make the optimizer work faster in case of recursive traversal (also required to avoid infinite recursion)

    do {
        noMoreWyptsAdded = true;
        switch(myTraversalMethod) {
            // ==================================================================
            // Logic: Get relatives, put in retVal if filter satisfies, then exit do while loop
            case GetAllImmediateChildren: // Get all children upto 1 level
                for(i = 0; i < _edges.size(); i++){
                    if(_edges[i]._endWyptID == referenceWypt) {
                        // Children only upto 1 level
                        relativeWypts.insert(_edges[i]._startWyptID);
                    }
                }
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        noMoreWyptsAdded = true; // End do while loop
                        retVal.insert(*it);
                    }
                }
                break;
            case GetAllImmediateParents:
                for(i = 0; i < _edges.size(); i++){
                    if(_edges[i]._startWyptID == referenceWypt) {
                        // Parents only upto 1 level
                        relativeWypts.insert(_edges[i]._endWyptID);
                    }
                }
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        noMoreWyptsAdded = true; // End do while loop
                        retVal.insert(*it);
                    }
                }
                break;
            case GetImmediateLHSChild: //Since only 1 lhs waypoint allowed
                for(i = 0; i < _edges.size(); i++){
                    if(_edges[i]._endWyptID == referenceWypt) {
                        // Children only upto 1 level
                        // ==================================================
                        // Additional Logic to test whether LHS children or not
                        if(IsLHSWyptHelper(_edges[i]._startWyptID, _edges[i]._endWyptID))
                            relativeWypts.insert(_edges[i]._startWyptID);
                        // ==================================================
                    }
                }
                assert(relativeWypts.size() == 1); // Exactly 1 LHS allowed: Even applicable for non-joins
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        noMoreWyptsAdded = true; // End do while loop
                        retVal.insert(*it);
                    }
                }
                break;
            case GetImmediateRHSChildren:
                for(i = 0; i < _edges.size(); i++){
                    if(_edges[i]._endWyptID == referenceWypt) {
                        // Children only upto 1 level
                        // ==================================================
                        // Additional Logic to test whether RHS children or not
                        if(!IsLHSWyptHelper(_edges[i]._startWyptID, _edges[i]._endWyptID))
                            relativeWypts.insert(_edges[i]._startWyptID);
                        // ==================================================
                    }
                }
                // Additional Logic: Filter wypts that are on child hash
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        noMoreWyptsAdded = true; // End do while loop
                        retVal.insert(*it);
                    }
                }
                break;
            // ==================================================================
            // Logic: Get relatives, put in retVal if filter satisfies, exit do while loop if no more children/parents added in retVal
            case GetAllRecursiveChildren:
                if(firstTime) {
                    for(i = 0; i < _edges.size(); i++){
                        if(_edges[i]._endWyptID == referenceWypt) {
                            // Children only upto 1 level
                            relativeWypts.insert(_edges[i]._startWyptID);
                        }
                    }
                    for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                    firstTime = false;
                }
                else {
                    diffRelativeWypts.clear();
                    for(i = 0; i < _edges.size(); i++){
                        // If _edges[i]._endWyptID is any of the children, then insert the _edges[i]._startWyptID into the relatives' set
                        if(relativeWypts.find(_edges[i]._endWyptID) != relativeWypts.end()) {
                            relativeWypts.insert(_edges[i]._startWyptID);
                            diffRelativeWypts.insert(_edges[i]._startWyptID);
                        }
                    }
                    for(it = diffRelativeWypts.begin(); it != diffRelativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                }
                break;
            case GetAllRecursiveParents:
                if(firstTime) {
                    for(i = 0; i < _edges.size(); i++){
                        if(_edges[i]._startWyptID == referenceWypt) {
                            // Parents only upto 1 level
                            relativeWypts.insert(_edges[i]._endWyptID);
                        }
                    }
                    for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                    firstTime = false;
                }
                else {
                    diffRelativeWypts.clear();
                    for(i = 0; i < _edges.size(); i++){
                        // If _edges[i]._startWyptID is any of the parents, then insert the _edges[i]._endWyptID into the relatives' set
                        if(relativeWypts.find(_edges[i]._startWyptID) != relativeWypts.end()) {
                            relativeWypts.insert(_edges[i]._endWyptID);
                            diffRelativeWypts.insert(_edges[i]._endWyptID);
                        }
                    }
                    for(it = diffRelativeWypts.begin(); it != diffRelativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                }
                break;
            case GetRecursiveLHSChildren: // i.e LHS child(i) += (LHS child of i += (LHS child of LHS child of i))
                if(firstTime) {
                    for(i = 0; i < _edges.size(); i++){
                        if(_edges[i]._endWyptID == referenceWypt) {
                            // Children only upto 1 level
                            // ==================================================
                            // Additional Logic to test whether LHS children or not
                            if(IsLHSWyptHelper(_edges[i]._startWyptID, _edges[i]._endWyptID))
                                relativeWypts.insert(_edges[i]._startWyptID);
                            // ==================================================
                        }
                    }
                    assert(relativeWypts.size() == 1); // Exactly 1 LHS allowed: Even applicable for non-joins
                    for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                    firstTime = false;
                }
                else {
                    diffRelativeWypts.clear();
                    for(i = 0; i < _edges.size(); i++){
                        if(_edges[i]._endWyptID == referenceWypt) {
                            // Children only upto 1 level
                            // ==================================================
                            // Additional Logic to test whether LHS children or not
                            if(IsLHSWyptHelper(_edges[i]._startWyptID, _edges[i]._endWyptID)) {
                                relativeWypts.insert(_edges[i]._startWyptID);
                                diffRelativeWypts.insert(_edges[i]._startWyptID);
                            }
                            // ==================================================
                        }
                    }
                    assert(diffRelativeWypts.size() == 1); // Exactly 1 LHS allowed: Even applicable for non-joins
                    for(it = diffRelativeWypts.begin(); it != diffRelativeWypts.end(); it++) {
                        if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                            prevSize = retVal.size();
                            retVal.insert(*it);
                            if(prevSize < retVal.size()) {
                                noMoreWyptsAdded = false; // Continue do while loop
                            }
                        }
                    }
                }
                break;
            // ==================================================================
            case GetLowerMostChildren:
                allRecursiveRelative = GetSpecificWypts(referenceWypt, GetAllRecursiveChildren,
                    myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType);
                // Now only put those children that don't have parent among the vector
                for(it1 = allRecursiveRelative.begin(); it1 != allRecursiveRelative.end(); it1++) {
                    // Inner Condition  => Is one of other children its children
                    innerConditionFlag = false;
                    for(it2 = allRecursiveRelative.begin(); it2 != allRecursiveRelative.end(); it2++) {
                        // Since all the children are already present, no need to check recursively
                        if(*it1 != *it2 && IsEdgePresent(*it2, *it1, false)) {
                            // Not the lowest
                            innerConditionFlag = true;
                            break;
                        }
                    }
                    if(!innerConditionFlag) {
                        retVal.insert(*it1);
                    }
                }
                break;
            case GetTopMostParents:
                allRecursiveRelative = GetSpecificWypts(referenceWypt, GetAllRecursiveParents,
                    myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType);
                // Now only put those children that don't have parent among the vector
                for(it1 = allRecursiveRelative.begin(); it1 != allRecursiveRelative.end(); it1++) {
                    // Inner Condition  => Is one of other parent its parent
                    innerConditionFlag = false;
                    for(it2 = allRecursiveRelative.begin(); it2 != allRecursiveRelative.end(); it2++) {
                        // Since all the children are already present, no need to check recursively
                        if(*it1 != *it2 && IsEdgePresent(*it1, *it2, false)) {
                            // Not the topmost
                            innerConditionFlag = true;
                            break;
                        }
                    }
                    if(!innerConditionFlag) {
                        retVal.insert(*it1);
                    }
                }
                break;
            case GetAllRecursiveChildrenAlongWithCurrent:
                if(DoesWaypointSatisfyTheFilter(referenceWypt, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                    myTraversalMethod = GetAllRecursiveChildren; // Nice trick
                    noMoreWyptsAdded = false; // Continue the Do while loop
                    retVal.insert(referenceWypt);
                }
                break;
            case GetAllMappedWaypoints:
                // Use relativeWypts
                for(i = 0; i < _records.size(); i++) {
                    if(_records[i]._wyptID != NOT_MAPPED) {
                        relativeWypts.insert(_records[i]._wyptID);
                    }
                }
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        retVal.insert(*it);
                    }
                }
                noMoreWyptsAdded = true; // Exit Do while loop
                break;
            case GetAllUnMappedWaypoints:
                // Use relativeWypts
                /*
                for(i = 0; i < _records.size(); i++) {
                    if(_records[i]._wyptID == NOT_MAPPED) {
                        relativeWypts.insert(_records[i]._wyptID);
                    }
                }
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        retVal.insert(*it);
                    }
                }
                noMoreWyptsAdded = true; // Exit Do while loop
                */
                cout << "11Error: Do not use GetSpecificWypts for getting unmapped waypoints. Instead use GetUnMappedRecordIndexes\n";
                exit(0);
                break;
            case GetAllWaypoints:
                // Use relativeWypts
                for(i = 0; i < _records.size(); i++) {
                    relativeWypts.insert(_records[i]._wyptID);
                }
                for(it = relativeWypts.begin(); it != relativeWypts.end(); it++) {
                    if(DoesWaypointSatisfyTheFilter(*it, myFilter, tblNameForFilter, attNameForFilter, qIDForFilter, expectedWyptType)) {
                        retVal.insert(*it);
                    }
                }
                noMoreWyptsAdded = true; // Exit Do while loop
                break;
        }
    }while(!noMoreWyptsAdded);


    return retVal;
}

#ifdef NIKE_DEBUG
void PathNetwork :: PrintEdges() {
    cout << "Edges:\n";
    for(__int64_t i = 0; i < _edges.size(); i++) {
        cout << _edges[i]._startWyptID << " -> " << _edges[i]._endWyptID << endl;
    }
}
#endif

bool PathNetwork :: IsEdgePresent(__int64_t childWyptID, __int64_t parentWyptID, bool checkRecursively)
{
    // Go top down from parent to children
    set<__int64_t> nextLevelChildren;
    for(__int64_t i = 0; i < _edges.size(); i++) {
        if(_edges[i]._startWyptID == childWyptID && _edges[i]._endWyptID == parentWyptID) {
            return true;
        }
        else if(_edges[i]._endWyptID == parentWyptID) {
            nextLevelChildren.insert(_edges[i]._startWyptID);
        }
    }

    if(checkRecursively) {
        for(set<__int64_t> :: iterator it = nextLevelChildren.begin(); it != nextLevelChildren.end(); it++) {
            if(IsEdgePresent(childWyptID, *it, true)) {
                return true;
            }
        }
    }

    // If none of the parent's children are child's parent; return false
    return false;
}

bool PathNetwork :: DoesWaypointSatisfyTheFilter(__int64_t wyptID, FilterConditions myFilter, string tblNameForFilter, string attNameForFilter, __int64_t qIDForFilter, PredicateType expectedWyptType)
{
    vector<__int64_t> recIndexes = GetRecordIndexesForWaypoint(wyptID);
    vector<string> lhsTblHashGroup;
    vector<string> lhsAttHashGroup;

    __int64_t j, k;

    assert(wyptID != NOT_MAPPED);

    // Return true if atleast one satisfy the condition
    for(__int64_t i = 0; i < recIndexes.size(); i++) {
        if(wyptID != _records[recIndexes[i]]._wyptID) {
            // i.e given record doesnot belong to given waypoint
            continue;
        }

        switch(myFilter) {
            case TrueIfQIDsSame:
                if(_records[recIndexes[i]]._qID == qIDForFilter && (expectedWyptType == NAPredType || expectedWyptType == _records[recIndexes[i]]._myType)) {
                    return true;
                }
                break;
            case TrueIfTablesSame:
                if((tblNameForFilter.compare(_records[recIndexes[i]]._lhsRel) == 0 || tblNameForFilter.compare(_records[recIndexes[i]]._rhsRel) == 0)
                    && (expectedWyptType == NAPredType || expectedWyptType == _records[recIndexes[i]]._myType)) {
                    return true;
                }
                break;
            case TrueIfQIDsANDTablesSame:
                if( (tblNameForFilter.compare(_records[recIndexes[i]]._lhsRel) == 0 || tblNameForFilter.compare(_records[recIndexes[i]]._rhsRel) == 0)
                    && _records[recIndexes[i]]._qID == qIDForFilter
                    && (expectedWyptType == NAPredType || expectedWyptType == _records[recIndexes[i]]._myType)) {
                    return true;
                }
                break;
            case TrueIfQIDsORTablesSame:
                if( ((tblNameForFilter.compare(_records[recIndexes[i]]._lhsRel) == 0 || tblNameForFilter.compare(_records[recIndexes[i]]._rhsRel) == 0)
                    || _records[recIndexes[i]]._qID == qIDForFilter)
                    && (expectedWyptType == NAPredType || expectedWyptType == _records[recIndexes[i]]._myType)) {
                    return true;
                }
                break;
            case TrueIfLHSHashGroupContainsTblAtt:
                // Since all the predicates point to same hash group, we need not check for all the predicates
                // Hence this case either returns true or false and doesnot iterate for another predicate of the same waypoint
                assert(wyptID != -1 && _records[recIndexes[i]]._groupID != -1); // Cannot find a hash group for waypoint that is not mapped
                for(j = 0; j < _groups.size(); j++) {
                    if(_groups[j]._groupID == _records[recIndexes[i]]._groupID) {
                        lhsTblHashGroup = _groups[j]._lhsHashRel;
                        lhsAttHashGroup = _groups[j]._lhsHashAtt;
                        assert(lhsTblHashGroup.size() == lhsAttHashGroup.size());
                        for(k = 0; k < lhsTblHashGroup.size(); k++) {
                            if(lhsTblHashGroup[k].compare(tblNameForFilter) == 0 && lhsAttHashGroup[k].compare(attNameForFilter) == 0) {
                                return true;
                            }
                        }
                        return false; // LHS doesnot contain tblNameForFilter
                    }
                }
                cout << "Error: No group found with groupID " <<  _records[recIndexes[i]]._groupID << " for waypoint " << wyptID << endl;
                exit(0);
                break;
            case AlwayTrue:
                if(expectedWyptType == NAPredType || expectedWyptType == _records[recIndexes[i]]._myType)
                    return true;
        }
    }

    return false;
}

OptimizerGroup :: OptimizerGroup() {
    _groupID = -1;
    _lhsHashRel.clear();
    _lhsHashAtt.clear();
    _rhsHashRel.clear();
    _rhsHashAtt.clear();
    _filterExpression = "";
    _bypassString = "";
}

vector<__int64_t> PathNetwork :: GetRecordIndexesForWaypoint(__int64_t wyptID)
{
    vector<__int64_t> retVal;

    for(__int64_t i = 0; i < _records.size(); i++) {
        if(_records[i]._wyptID == wyptID) {
            retVal.push_back(i);
        }
    }

    return retVal;
}

PathNetwork :: PathNetwork(const PathNetwork& copyMe) {
    _groupIDCounter = copyMe._groupIDCounter;
    _recIDCounter = copyMe._recIDCounter;
    _edgeIDCounter = copyMe._edgeIDCounter;
    _wyptIDCounter = copyMe._wyptIDCounter;

    for(__int64_t i = 0; i < copyMe._records.size(); i++) {
        _records.push_back(copyMe._records[i]);
    }
    for(__int64_t i = 0; i < copyMe._edges.size(); i++) {
        _edges.push_back(copyMe._edges[i]);
    }
    for(__int64_t i = 0; i < copyMe._groups.size(); i++) {
        _groups.push_back(copyMe._groups[i]);
    }
}
// ----------------------------------------------

#include "Coster.cc"
