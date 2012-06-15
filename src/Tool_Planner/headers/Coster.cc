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
PathNetworkCost Coster :: GetCost()
{
    if((_myPtrConfig -> _costerMode).compare("DataFlow") == 0) {
        return GetDataFlowBasedCost();
    }
    else {
        OPTIMIZER_FAIL_IF(true, "Error: Only DataFlow based cost functions supported by the optimizer\n");
    }
	
	cout << "\nReturning default cost in GetCost() function. Are you sure ?\n";
    PathNetworkCost defaultVal;
    return defaultVal;
}


__int64_t Coster :: GetCardinality(string tableName)
{
	if(tableName.compare(IGNORE_NAME) == 0 ) {
		cout << "Error: In GetCardinality, invalid tblName:" << tableName << endl;
		cout << "Hint: If you see $IGNORE_NAME$ in above error and if this is intended for empty selections, the optimizer expected LT as operator whereas parser passed EQ as operator.\n";
		exit(0);
	}
	Catalog &myCat = Catalog::GetCatalog();
	Schema mySchema(tableName);
	myCat.GetSchema(tableName, mySchema);
	__int64_t numTuples = mySchema.GetNumTuples();
	#ifdef PRINT_COST_PATH_NETWORK
	cout << "Number of tuples for table: " << tableName << " = " << numTuples << endl;
	#endif
	return numTuples;
}

__int64_t Coster :: GetValueCount(string tableName, string attributeName)
{
	if(tableName.compare(IGNORE_NAME) == 0 || attributeName.compare(IGNORE_NAME) == 0) {
		cout << "Error: In GetValueCount, invalid tbl/attName:" << tableName << "." << attributeName << endl;
		cout << "Hint: If you see $IGNORE_NAME$ in above error and if this is intended for empty selections, the optimizer expected LT as operator whereas parser passed EQ as operator.\n";
		exit(0);
	}

	Catalog &myCat = Catalog::GetCatalog();
	Schema mySchema(tableName);
	myCat.GetSchema(tableName, mySchema);
	Attribute myAtt(attributeName);
	mySchema.GetAttribute(myAtt);

	return myAtt.GetUniques();
}

bool Coster :: IsPrimaryKeyForeignKeyReln(string tbl1, string att1, string tbl2, string att2) {

  // Alin: hell with this. Return true always
  return true;

	Catalog &c = Catalog::GetCatalog();

	Schema sch(tbl1);
	c.GetSchema(sch);

	AttributeContainer pc;
	sch.GetPrimaryKey(pc);

	set<string> primaryKeys;

	for (pc.MoveToStart();!pc.AtEnd();pc.Advance()) {
	    primaryKeys.insert(pc.Current().GetName());
	}

	if(primaryKeys.find(att1) == primaryKeys.end()) {
		return false;
	}

	//Schema sch2(tbl2);
	//c.GetSchema(sch2);

	AttributeContainer fc;
	sch.GetForeignKeys(fc);

	set<string> foreignKeys;

	for (fc.MoveToStart();!fc.AtEnd();fc.Advance()) {
	    string sss = fc.Current().GetForeignRelation() + "." +
	    fc.Current().GetForeignAttribute();

	    foreignKeys.insert(sss);
	}
	string fk = tbl2 + "." + att2;
	if(foreignKeys.find(fk) == foreignKeys.end()) {
		// cout << tbl1 << "." << att1 << " is a primary key but " << tbl2 << "." << att2 << " is not a foreign key. Confirm this with catalog. For now, let me be optimistic and assume it to be PK-FK rel\n";
		return true;
	}
	
	return true;
}

PathNetworkCost Coster :: GetCostForSelectionEdge(vector<__int64_t> recIndexes, PathNetworkCost costOfParentEdge)
{
	vector<float> selectivityFactors;
	for(__int64_t i = 0; i < recIndexes.size(); i++) {
		if((*_myNetwork)._records[recIndexes[i]]._myOp == EQ) {

			__int64_t count= GetValueCount((*_myNetwork)._records[recIndexes[i]]._lhsRel, (*_myNetwork)._records[recIndexes[i]]._lhsAtt);
			float temp = 1.0;
			if (count>0)
			  temp/=count;
			selectivityFactors.push_back(temp);
		}
		else {
			selectivityFactors.push_back(0.3333);
		}
	}

	float finalSelectivityFactor = 1;
	for(__int64_t i = 0; i < selectivityFactors.size(); i++) {
		finalSelectivityFactor *= (1 - selectivityFactors[i]);
	}

	PathNetworkCost retVal;

	#ifdef OPTIMIZE_SELECTIONS
    		retVal._dataFlowCost = (__int64_t)(finalSelectivityFactor * costOfParentEdge._dataFlowCost);
	#endif
	#ifndef OPTIMIZE_SELECTIONS
		retVal._dataFlowCost = costOfParentEdge._dataFlowCost;
	#endif

	return retVal;
}

PathNetworkCost Coster :: GetValueCountFromFlow(string tblName, string attName, __int64_t parentEdgeIndex)
{
    // TODO: No histograms for now :)
    map<__int64_t, PathNetworkCost>::iterator foundIT = _costHelper.find(parentEdgeIndex);
    OPTIMIZER_FAIL_IF(foundIT == _costHelper.end(), "Error: Parent Edge is not assigned")

    return foundIT -> second;
}

PathNetworkCost Coster :: GetCostForJoinEdge(__int64_t currEdgeIndex, vector<__int64_t> parentEdgeIndexes, vector<__int64_t> recIndexes)
{
    PathNetworkCost retVal;
    retVal._dataFlowCost = 1;
    assert(recIndexes.size() > 0);
    assert(parentEdgeIndexes.size() >= 2);

    // First check whether it is cartesian product
    if((*_myNetwork)._records[recIndexes[0]]._myOp == CARTESIAN) {
        for(__int64_t i = 0; i < parentEdgeIndexes.size(); i++) {
            map<__int64_t, PathNetworkCost>::iterator foundIT = _costHelper.find(parentEdgeIndexes[i]);
            OPTIMIZER_FAIL_IF(foundIT == _costHelper.end(), "Error: Parent Edge is not assigned")

            // Cartesian is product of flow of input edges
            retVal._dataFlowCost *= (foundIT -> second)._dataFlowCost;
        }
        return retVal;
    }


    // Cost of join is Maximum flow for all queries
    // Why ? --> The goal is to reward sharing and also hash lookups (over filters)
    // Assumptions: Primary key foreign key relationship
    __int64_t grpIndex = (*_myNetwork).GetGroupIndex((*_myNetwork)._edges[currEdgeIndex]._startWyptID);

    OPTIMIZER_FAIL_IF(grpIndex == -1, "Error: No group found for the join wypt: " << (*_myNetwork)._edges[currEdgeIndex]._startWyptID)

    set<__int64_t> temp = (*_myNetwork).GetSpecificWypts((*_myNetwork)._edges[currEdgeIndex]._startWyptID, GetImmediateLHSChild, AlwayTrue,
        IGNORE_NAME, IGNORE_NAME, -1, NAPredType);
    assert(temp.size() == 1);
    set<__int64_t>::iterator it = temp.begin();
    __int64_t lhsChild = *it;
    __int64_t lhsParentIndex = -1;
    for(__int64_t i = 0; i < parentEdgeIndexes.size(); i++) {
        if((*_myNetwork)._edges[parentEdgeIndexes[i]]._startWyptID ==  lhsChild) {
            OPTIMIZER_FAIL_IF(lhsParentIndex != -1, "Error: Only 1 Left edge allowed")
            lhsParentIndex = parentEdgeIndexes[i];
        }
    }
    assert(lhsParentIndex != -1);

    for(__int64_t i = 0; i < parentEdgeIndexes.size(); i++) {
        if(parentEdgeIndexes[i] != lhsParentIndex) {
            PathNetworkCost minAmongHashes;
            minAmongHashes._dataFlowCost = -1;
            for(__int64_t j = 0; j < (*_myNetwork)._groups[grpIndex]._lhsHashRel.size(); j++) {


                //  primary key foriegn key relation
                PathNetworkCost outputFlowForThisHash;

		if(!(IsPrimaryKeyForeignKeyReln((*_myNetwork)._groups[grpIndex]._lhsHashRel[j], (*_myNetwork)._groups[grpIndex]._lhsHashAtt[j], (*_myNetwork)._groups[grpIndex]._rhsHashRel[j], (*_myNetwork)._groups[grpIndex]._rhsHashAtt[j])
		|| IsPrimaryKeyForeignKeyReln((*_myNetwork)._groups[grpIndex]._rhsHashRel[j], (*_myNetwork)._groups[grpIndex]._rhsHashAtt[j],(*_myNetwork)._groups[grpIndex]._lhsHashRel[j], (*_myNetwork)._groups[grpIndex]._lhsHashAtt[j]) )){
			__int64_t lhsVals = GetCardinality((*_myNetwork)._groups[grpIndex]._lhsHashRel[j]);
			__int64_t rhsVals = GetCardinality((*_myNetwork)._groups[grpIndex]._rhsHashRel[j]);
			#ifdef PRINT_COST_PATH_NETWORK
			cout << "Not a PK-FK join" << (*_myNetwork)._groups[grpIndex]._lhsHashRel[j] << "." << (*_myNetwork)._groups[grpIndex]._lhsHashAtt[j] << ", " << (*_myNetwork)._groups[grpIndex]._rhsHashRel[j] << "." << (*_myNetwork)._groups[grpIndex]._rhsHashAtt[j] << endl;
			#endif
			// Nike Change: To punish non-PK-FK joins
			outputFlowForThisHash._dataFlowCost = (lhsVals * rhsVals) / 3;
			// Also, especially punish if LHS < RHS
			if(lhsVals < rhsVals) {
				outputFlowForThisHash._dataFlowCost = outputFlowForThisHash._dataFlowCost + rhsVals;
				#ifdef PRINT_COST_PATH_NETWORK
				cout << "Since no PK-FK & LHS < RHS, therefore increase the flow\n"; 
				#endif
			}
			outputFlowForThisHash._nonPKFKJoinCard.push_back(pair<__int64_t, __int64_t>(lhsVals, rhsVals));
		}
		else {
			PathNetworkCost lhsValueCount = GetValueCountFromFlow((*_myNetwork)._groups[grpIndex]._lhsHashRel[j], (*_myNetwork)._groups[grpIndex]._lhsHashAtt[j], lhsParentIndex);
			PathNetworkCost rhsValueCount = GetValueCountFromFlow((*_myNetwork)._groups[grpIndex]._rhsHashRel[j], (*_myNetwork)._groups[grpIndex]._rhsHashAtt[j], parentEdgeIndexes[i]);
			// Nike_Change Think about it !!!
			#ifdef JOIN_COSTING_SCENARIO1
				outputFlowForThisHash = lhsValueCount < rhsValueCount ? lhsValueCount : rhsValueCount;
			#endif
			#ifndef JOIN_COSTING_SCENARIO1
				outputFlowForThisHash = rhsValueCount < lhsValueCount ? lhsValueCount : rhsValueCount;
			#endif
			
			#ifdef PRINT_COST_PATH_NETWORK
			cout << "PK-FK join: " << lhsValueCount._dataFlowCost << "|" << rhsValueCount._dataFlowCost << " -> " << outputFlowForThisHash._dataFlowCost << endl;
			#endif
		}

                if(minAmongHashes._dataFlowCost == -1) {
                    minAmongHashes = outputFlowForThisHash;
                }
                else {
                    minAmongHashes = outputFlowForThisHash < minAmongHashes ? outputFlowForThisHash : minAmongHashes;
                }
            }
            assert(minAmongHashes._dataFlowCost != -1);
            // Maximum of flow for all the queries
            retVal = minAmongHashes < retVal ? retVal : minAmongHashes;
        }
    }

    return retVal;
}

void Coster :: AssignWeights()
{

	_costHelper.clear();
	bool done = true;

	#ifdef PRINT_COST_PATH_NETWORK
	cout << "\n---------------------------------------------------\n";
	#endif

	do {
		done = true;
		for(__int64_t i = 0; i < (*_myNetwork)._edges.size(); i++) {
			map<__int64_t, PathNetworkCost>::iterator foundIT = _costHelper.find(i);
			if(foundIT != _costHelper.end()) {
				// Alreasy assigned, continue
				continue;
			}

			// Atleast 1 edge not done
			done = false;
			vector<__int64_t> pEdgeIndex = GetParentEdgesIndex(i); // TODO: Change later to use iterator

			if(pEdgeIndex.size() == 0) {
				// TableScan edge
				vector<__int64_t> recIndexes = (*_myNetwork).GetRecordIndexesForWaypoint((*_myNetwork)._edges[i]._startWyptID);
				if(recIndexes.size() == 0) {
					cout << "Error: In AssignWeights, no predicates for waypoint:" << (*_myNetwork)._edges[i]._startWyptID << endl;
					exit(0);
				}

				__int64_t predIndex = recIndexes[0];
				if((*_myNetwork)._records[predIndex]._myType != TableScan) {
					cout << "Error: In AssignWeights, no input edge but predicate is not of type TableScan" << endl;
					exit(0);
				}

				string relName = (*_myNetwork)._records[predIndex]._lhsRel;
				PathNetworkCost tblScanCost;
				tblScanCost._dataFlowCost = GetCardinality(relName);
				#ifdef PRINT_COST_PATH_NETWORK
				cout << "TblScan Edge: " << relName << " " << tblScanCost._dataFlowCost << endl;
				#endif
				_costHelper.insert(pair<__int64_t, PathNetworkCost>(i, tblScanCost));
			}
			else if(pEdgeIndex.size() == 1) {

				map<__int64_t, PathNetworkCost>::iterator parentFoundIT = _costHelper.find(pEdgeIndex[0]);
				if(parentFoundIT == _costHelper.end()) {
					// Parent edge not assigned
					continue;
				}

				PathNetworkCost costOfParentEdge = parentFoundIT -> second;

				// ParentEdge ---> start ---i---> end

				vector<__int64_t> startRecIndexes = (*_myNetwork).GetRecordIndexesForWaypoint((*_myNetwork)._edges[i]._startWyptID);

				if(startRecIndexes.size() == 0) {
					cout << "Error: In AssignWeights, no predicates for waypoint" << (*_myNetwork)._edges[i]._startWyptID << endl;
					exit(0);
				}

				PredicateType startWyptType = (*_myNetwork)._records[startRecIndexes[0]]._myType;

				if(startWyptType == Selection) {
					#ifndef PRINT_COST_PATH_NETWORK
					_costHelper.insert(pair<__int64_t, PathNetworkCost>(i, GetCostForSelectionEdge(startRecIndexes, costOfParentEdge)));
					#else
					PathNetworkCost selCost =  GetCostForSelectionEdge(startRecIndexes, costOfParentEdge);
					cout << "Selection Edge: (with parent edge cost:" << costOfParentEdge._dataFlowCost  << ")  -> " << selCost._dataFlowCost << endl;
					_costHelper.insert(pair<__int64_t, PathNetworkCost>(i, selCost));
					#endif
				}
				else if(startWyptType == Aggregate || startWyptType == Output) {
					// cout << "Agg or Output Edge\n";
				    PathNetworkCost outputCost;
				    outputCost._dataFlowCost = CONSTANT_WEIGHT;
					_costHelper.insert(pair<__int64_t, PathNetworkCost>(i, outputCost));
				}
				else {
					cout << "Error: In AssignWeights, only 1 parent edge, but not a Selection waypoint\n.";
					exit(0);
				}
			}
			else {
				// cout << "Join Edge\n";
				// Join edge
				vector<__int64_t> parentCost;
				bool isAnyParentEdgeNotAssigned = false;
				for(__int64_t j = 0; j < pEdgeIndex.size(); j++) {
					map<__int64_t, PathNetworkCost>::iterator parentFoundIT = _costHelper.find(pEdgeIndex[j]);
					if(parentFoundIT == _costHelper.end()) {
						// Parent edge not assigned
						isAnyParentEdgeNotAssigned = true;
					}
					else {
						parentCost.push_back((parentFoundIT -> second)._dataFlowCost);
					}
				}
				if(isAnyParentEdgeNotAssigned) {
					//Since one of Parent edge not assigned, continue
					continue;
				}

				// All the input are assigned
				// ParentEdges ---> start ---i---> end

				vector<__int64_t> startRecIndexes = (*_myNetwork).GetRecordIndexesForWaypoint((*_myNetwork)._edges[i]._startWyptID);

				if(startRecIndexes.size() == 0) {
					cout << "Error: In AssignWeights:> no predicates for waypoint" << (*_myNetwork)._edges[i]._startWyptID << endl;
					exit(0);
				}

				PredicateType startWyptType = (*_myNetwork)._records[startRecIndexes[0]]._myType;

				if(startWyptType == Join) {
					 #ifndef PRINT_COST_PATH_NETWORK
				    	_costHelper.insert(pair<__int64_t, PathNetworkCost>(i, GetCostForJoinEdge(i, pEdgeIndex, startRecIndexes)));
                                        #else
                                        PathNetworkCost joinCost =  GetCostForJoinEdge(i, pEdgeIndex, startRecIndexes);
                                        cout << "Selection Edge: (with parent edge cost:";
					for(__int64_t costIter = 0; costIter < parentCost.size(); costIter++) {
						cout << parentCost[costIter] << ", ";
					}
 					cout << ")  -> " << joinCost._dataFlowCost << endl;
                                        _costHelper.insert(pair<__int64_t, PathNetworkCost>(i, joinCost));
                                        #endif

				}
				else {
					cout << "Error: In AssignWeights, more than 1 parent edge, but not a Join waypoint\n.";
					exit(0);
				}
			}
		} // for
	} while(!done);

	#ifdef PRINT_COST_PATH_NETWORK
	cout << "\n---------------------------------------------------\n";
	#endif
}

PathNetworkCost Coster :: GetDataFlowBasedCost()
{
    	// Statistics
	AssignWeights();

	// No MiniSearch !!!

	__int64_t finalCost = 0;
	for(map<__int64_t, PathNetworkCost>::iterator it = _costHelper.begin(); it != _costHelper.end(); it++) {
		finalCost += (it -> second)._dataFlowCost;
	}

	#ifdef PRINT_COST_PATH_NETWORK
	
	#endif

    	PathNetworkCost retVal;
    	retVal._dataFlowCost = finalCost;
	return retVal;
}


bool PathNetworkCost :: operator<(const PathNetworkCost &copyMe) {
    // TODO: Multi-objective optimization
    /*
	if(_dataFlowCost <= copyMe._dataFlowCost && _badDecisionCost < copyMe._badDecisionCost) {
		return true;
	}
	else if(_dataFlowCost <= copyMe._dataFlowCost && _badDecisionCost >= copyMe._badDecisionCost) {
		// This is strange case: dataflow is less but it has taken some bad decision while optimizations. Eg: lhs is smaller.
		if((copyMe._dataFlowCost - _dataFlowCost) / _dataFlowCost < 0.1) {
			// i.e copyMe's cost is greater than this by 10%
		}
	}
	*/
	if(_dataFlowCost == copyMe._dataFlowCost) {
		cout << "\n Are primary keys set properly ?\n";

	}
    return _dataFlowCost < copyMe._dataFlowCost;
}

Coster :: Coster()
{
    _myNetwork = NULL;
    _myPtrConfig = NULL;
}

Coster :: Coster(const Coster& copyMe)
{
    _costHelper = copyMe._costHelper;
}


vector<__int64_t> Coster :: GetParentEdgesIndex(__int64_t currEdgeIndex)
{
	vector<__int64_t> retVal;

	__int64_t fromID = (*_myNetwork)._edges[currEdgeIndex]._startWyptID;

	for(__int64_t i = 0; i < (*_myNetwork)._edges.size(); i++) {
		if((*_myNetwork)._edges[i]._endWyptID == fromID) {
			retVal.push_back(i);
		}
	}

	return retVal;
}
