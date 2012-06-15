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
// See struct less<PathNetworkCost> for comparison
class PathNetworkCost
{
public:
    __int64_t _dataFlowCost;
	// int _badDecisionCost; // This is a tweaking parameter for the exceution engine
    // PathNetworkCost operator=(const PathNetworkCost &copyMe);

	vector< pair<__int64_t, __int64_t> > _nonPKFKJoinCard;
    bool operator<(const PathNetworkCost &copyMe);
	PathNetworkCost() {
		_dataFlowCost = 0;
		//_badDecisionCost = 0;
	}
};

class Coster
{
	PathNetworkCost GetDataFlowBasedCost();
    PathNetworkCost GetValueCountFromFlow(string tblName, string attName, __int64_t parentEdgeIndex);
    void AssignWeights();

	bool IsPrimaryKeyForeignKeyReln(string tbl1, string att1, string tbl2, string att2);
    __int64_t GetValueCount(string tableName, string attributeName);
    PathNetworkCost GetCostForSelectionEdge(vector<__int64_t> recIndexes, PathNetworkCost costOfParentEdge);
    PathNetworkCost GetCostForJoinEdge(__int64_t currEdgeIndex, vector<__int64_t> parentEdgeIndexes, vector<__int64_t> recIndexes);

public:

    // Map of index of edge to cost
	map<__int64_t, PathNetworkCost> _costHelper;
    vector<__int64_t> GetParentEdgesIndex(__int64_t currEdgeIndex);
		__int64_t GetCardinality(string tableName);

    PathNetwork* _myNetwork;
    OptimizerConfiguration* _myPtrConfig;
    PathNetworkCost GetCost();
    Coster();
    Coster(const Coster& copyMe);
};
