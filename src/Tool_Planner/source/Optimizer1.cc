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

// avoid the large relation on the right
#define OPTIMIZE_FOR_EXECUTION_ENGINE

#include "Optimizer1.h"

#include "OptimizerHelper.cc" // I know this is bad practice, but its better than creating one large file for Optimizer


#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <map>

void Optimizer :: ProduceOutput(){
	/** Alin's Note: do not mess with this. I need this for the
			interaction with the system and debugging */
	
	// first, get the precise time
  timeval t;
  gettimeofday(&t, NULL);

  // make a directory name with the timestamp
  char dirName[128];
  //sprintf(dirName, "../../Tool_DataPath/executable/generated_%d_%d", (unsigned int)t.tv_sec, (unsigned int)t.tv_usec);
  sprintf(dirName, "../../Tool_DataPath/executable/");

  // now, create the directory
  //int mkret = mkdir(dirName, S_IRWXU | S_IRWXG);
  //FATALIF(mkret == -1, "Unable to create directory %s!", dirName);

	printf("The generated files are in directory %s\n", dirName);
	string file = string(dirName)+"/"+_outputFileName + ".dp";
	_nextNI.ConvertPathNetworkToDPFile(file, oldQueries, oldWaypoints);
	_nextNI.ConvertPathNetworkToDotFile(string(dirName)+"/"+_outputFileName + ".dot");
//	_nextNI._bigMap.PrintBigMap();
	
	// tell the rest of the system about the  file
	fprintf(systemPipe, "%s\n", file.c_str());
	fflush(systemPipe);
}


void Optimizer :: ReInitialize() {
    oldQueries = _nextNI.GetAllQueries();
}

void NetworkIntegrator :: ConvertPathNetworkToDotFile(string outputFileName)
{
    ofstream myfile;
	myfile.open(outputFileName.c_str());

	myfile << "digraph graphname {\n";

    // Notations
    myfile << "notation1[label=\"TableScan\",shape=box,style=filled,color=\".7 .3 1.0\"]\n";
    myfile << "notation2[label=\"Join\",shape=ellipse,style=filled,color=\".7 .3 1.0\"]\n";
    myfile << "notation3[label=\"Selection\",shape=parallelogram,style=filled,color=\".7 .3 1.0\"]\n";
    myfile << "notation4[label=\"Aggregate\",shape=diamond,style=filled,color=\".7 .3 1.0\"]\n";
    myfile << "notation5[label=\"Print\",shape=hexagon,style=filled,color=\".7 .3 1.0\"]\n";

	// First create wypts
    set<__int64_t> wyptIDs = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, AlwayTrue,
        IGNORE_NAME, IGNORE_NAME, -1, NAPredType);
    for(set<__int64_t>::iterator it = wyptIDs.begin(); it != wyptIDs.end(); it++) {
        myfile << "W" << *it << "[label=\"";
        vector<__int64_t> recIndexes = _myNetwork.GetRecordIndexesForWaypoint(*it);
        assert(recIndexes.size() > 0);
	myfile << "W" << *it;
        for(__int64_t i = 0; i < recIndexes.size(); i++) {
            myfile << "{" << _myNetwork._records[recIndexes[i]]._qID;
            if(_myNetwork._records[recIndexes[i]]._lhsRel.compare(IGNORE_NAME) != 0) {
                myfile << ", " << _myNetwork._records[recIndexes[i]]._lhsRel;
            }
            if(_myNetwork._records[recIndexes[i]]._lhsAtt.compare(IGNORE_NAME) != 0) {
                myfile << "." << _myNetwork._records[recIndexes[i]]._lhsAtt;
            }
            if(_myNetwork._records[recIndexes[i]]._rhsRel.compare(IGNORE_NAME) != 0) {
                myfile << ", " << _myNetwork._records[recIndexes[i]]._rhsRel << "." << _myNetwork._records[recIndexes[i]]._rhsAtt;
            }
		if(_myNetwork._records[recIndexes[0]]._myType == Join) {
            		__int64_t grpIndex = _myNetwork.GetGroupIndex(_myNetwork._records[recIndexes[0]]._wyptID);
			myfile << "(";
			for(int j = 0; j < _myNetwork._groups[grpIndex]._lhsHashRel.size(); j++) {
				myfile << _myNetwork._groups[grpIndex]._lhsHashRel[j] << "." << _myNetwork._groups[grpIndex]._lhsHashAtt[j]  << ", "; 
			}
			myfile << ")";
		}	
	
            myfile << "} ";
        }

        myfile << "\", shape=";
        switch(_myNetwork._records[recIndexes[0]]._myType){
            case Join:
                myfile << "ellipse];\n";
                break;
            case Selection:
                myfile << "parallelogram];\n";
                break;
            case TableScan:
                myfile << "box];\n";
                break;
            case Aggregate:
                myfile << "diamond];\n";
                break;
            case Output:
                myfile << "hexagon];\n";
                break;
            case Top:
            case NAPredType:
                OPTIMIZER_FAIL_IF(true, "Error: Top and NAPredType not allowed in as final wypt type while creating dot file.");
                break;
        }

    }

    // Now edges
    for(__int64_t i = 0; i < _myNetwork._edges.size(); i++) {
	int flow = -1;
	_myCoster.GetCost();
	 map<__int64_t, PathNetworkCost>::iterator flowIter = _myCoster._costHelper.find(i);
	if(flowIter != _myCoster._costHelper.end())
		flow = (flowIter -> second)._dataFlowCost;
        myfile << "W" << _myNetwork._edges[i]._startWyptID << " -> W" << _myNetwork._edges[i]._endWyptID << "[label=\"" << flow <<"\" color=\"red\"];" << endl;
    }

    // To keep tablescans at same level
    set<__int64_t> tblScanIDs = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, AlwayTrue,
        IGNORE_NAME, IGNORE_NAME, -1, TableScan);
    myfile << "{ rank=same; ";
    for(set<__int64_t>::iterator it = tblScanIDs.begin(); it != tblScanIDs.end(); it++) {
        myfile << " W" << *it;
    }
    myfile << "}\n";

  	myfile << "{ rank=same; notation1 notation2 notation3 notation4 notation5 }\n";

	myfile << "}";

	myfile.close();
}

/** Produce a nice .dp file. */

void NetworkIntegrator :: ConvertPathNetworkToDPFile(string outputFileName, set<__int64_t> oldQueries, map<__int64_t, string>& oldWaypoints)
{
	ofstream myfile;
	myfile.open(outputFileName.c_str());

	// In the following code, we need to keep a map from wyptIDs to names of scanners
	// Whenever we encounter the name of an id, we look into the map

	typedef map<__int64_t, string> MyMapT;
	MyMapT wpNameMap;

	// First output scanners: new scanner = part;
	set<__int64_t> wyptIDs = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, AlwayTrue,
        IGNORE_NAME, IGNORE_NAME, -1, TableScan);
	for(set<__int64_t>::iterator it = wyptIDs.begin(); it != wyptIDs.end(); it++) {
		//myfile << "new scanner ";
		vector<__int64_t> recIndexes = _myNetwork.GetRecordIndexesForWaypoint(*it);
		assert(recIndexes.size() > 0);

		 wpNameMap[*it] = _myNetwork._records[recIndexes[0]]._lhsRel;
		  
		if (oldWaypoints.find(*it)==oldWaypoints.end()){
		  myfile << "new scanner " << wpNameMap[*it] << ";\n";
		}
		oldWaypoints[*it] = _myNetwork._records[recIndexes[0]]._lhsRel;
	}

	// Then find all the queries - All the queries need to have tablescans
	vector<__int64_t> sortedWaypointIDsScan = _myNetwork.GetTopologicallySortedWaypointIDs();
	set<__int64_t> queryIDs;
	for(vector<__int64_t>::iterator it = sortedWaypointIDsScan.begin(); it != sortedWaypointIDsScan.end(); it++) {
		vector<__int64_t> recIndexes = _myNetwork.GetRecordIndexesForWaypoint(*it);
		for(int i = 0; i < recIndexes.size(); i++) {
			queryIDs.insert(_myNetwork._records[recIndexes[i]]._qID);
		}
	}

	myfile << "\nnew query";
	bool firstTime = true;
	for(set<__int64_t>::iterator it1 = queryIDs.begin(); it1 != queryIDs.end(); it1++) {
		if (oldQueries.find(*it1) == oldQueries.end()) {
		if(firstTime) {
			myfile << " Q";
			firstTime = false;
		}
		else {
			myfile << ", Q";				
		}
		myfile << *it1;
		}
	}
	myfile << ";\n\n";

	// Get the topological sort on all the waypoints
	vector<__int64_t> sortedWaypointIDs = _myNetwork.GetTopologicallySortedWaypointIDs();
	for(int i = 0; i < sortedWaypointIDs.size(); i++) {
		if(wyptIDs.find(sortedWaypointIDs[i]) == wyptIDs.end()) {
		  
			vector<__int64_t> recIndexes = _myNetwork.GetRecordIndexesForWaypoint(sortedWaypointIDs[i]);
			assert(recIndexes.size() > 0);
			PredicateType typeOfWaypoint = _myNetwork._records[recIndexes[0]]._myType;
			vector<string> lhsHashRel;
			vector<string> lhsHashAtt;
		    char buffer[100];
		    sprintf(buffer, "W%ld", sortedWaypointIDs[i]);
		    wpNameMap[sortedWaypointIDs[i]] = buffer;
			bool print = false;
		  if (oldWaypoints.find(sortedWaypointIDs[i])==oldWaypoints.end()){ // SS
			oldWaypoints[sortedWaypointIDs[i]] = buffer; // SS
			print = true;
		    
		    
		    myfile << "new waypoint " << wpNameMap[sortedWaypointIDs[i]] << " = ";
		}
		    // new waypoint
		    // Only output non-table scans waypoints
		    

			// Find the info about the waypoint
			// Output the type of waypoint
			if(typeOfWaypoint == Join) {
				if (print)
				myfile << "Join";
				string lhsString = "";
				int grpIndex = _myNetwork.GetGroupIndex(_myNetwork._records[recIndexes[0]]._wyptID);
				if(grpIndex != -1) {
					// Normal join cases
					bool isFirstTime = true;
					for(int iter = 0; iter < _myNetwork._groups[grpIndex]._lhsHashRel.size(); iter++) {
						if(!isFirstTime)
							lhsString.append(", ");
						lhsString.append(_myNetwork._groups[grpIndex]._lhsHashRel[iter]);
						lhsString.append("."); 
						lhsString.append(_myNetwork._groups[grpIndex]._lhsHashAtt[iter]);
						lhsHashRel.push_back(_myNetwork._groups[grpIndex]._lhsHashRel[iter]);
						lhsHashAtt.push_back(_myNetwork._groups[grpIndex]._lhsHashAtt[iter]);
						isFirstTime = false;
					}
				if (print)
					myfile << "(" << lhsString << ")";

					if(_myNetwork._groups[grpIndex]._filterExpression.compare("") != 0 && print)
						myfile << " filterExpression=\"" << _myNetwork._groups[grpIndex]._filterExpression << "\"";
					if(_myNetwork._groups[grpIndex]._bypassString.compare("") != 0 && print)
						myfile << " bypassString=\"" << _myNetwork._groups[grpIndex]._bypassString << "\"";

				if (print)
					myfile << " :- ";
				}
				else {
					// Cartesian joins
					assert(_myNetwork._records[recIndexes[0]]._groupID == -2); // i.e cartesian product
					cout << "Cartesian Products not supported. Please check your query to make sure the predicates are correct\n";
					cout << "Optimizer Hint: Try to add join predicates for tables: " << _myNetwork._records[recIndexes[0]]._lhsRel << " and " << _myNetwork._records[recIndexes[0]]._rhsRel << endl;
					assert(false);
				}
				
			}
			else if(typeOfWaypoint == Selection) {
				if (print)
				myfile << "Select :-";				
			}
			else if(typeOfWaypoint == Aggregate) {
				if (print)
				myfile << "Aggregate :-";
			}
			else if(typeOfWaypoint == Top) {
				// myfile << "Top :-";
				cout << "\nError: While printing the DP file: Top waypoint not supported in this format\n";
				exit(0);
			}
			else if(typeOfWaypoint == Output) {
				if (print)
				myfile << "Print :-";
			}
			else {
				cout << "\nError: While printing the DP file: Incorrect type of waypoint\n";
				exit(0);
			}
			

			
			// Now print the input edges
			if(typeOfWaypoint == Join) {
				// Sinnce Seperate logic for RHS
				set<__int64_t> allLHSChildWyptID = _myNetwork.GetSpecificWypts(sortedWaypointIDs[i], GetImmediateLHSChild, AlwayTrue,
		        		IGNORE_NAME, IGNORE_NAME, -1, NAPredType);
				set<__int64_t> allRHSChildWyptIDs = _myNetwork.GetSpecificWypts(sortedWaypointIDs[i], GetImmediateRHSChildren, AlwayTrue,
		        		IGNORE_NAME, IGNORE_NAME, -1, NAPredType);

				// Only 1 LHS child allowed
				assert(allLHSChildWyptID.size() == 1);				
				//myfile << "?" << wpNameMap[*(allLHSChildWyptID.begin())];
				if (print)
				myfile << wpNameMap[*(allLHSChildWyptID.begin())];

				// Now RHS children
				for(set<__int64_t>::iterator rhsIT = allRHSChildWyptIDs.begin(); rhsIT != allRHSChildWyptIDs.end(); rhsIT++) {
				if (print)
					myfile << ", ?" << wpNameMap[*rhsIT];
				}
				if (print)
				myfile << ";\n";
			}
			else {
				set<__int64_t> allImmediateChildrenWyptIDs = _myNetwork.GetSpecificWypts(sortedWaypointIDs[i], GetAllImmediateChildren, 					AlwayTrue, IGNORE_NAME, IGNORE_NAME, -1, NAPredType);

				firstTime = true;
				for(set<__int64_t>::iterator childIT = allImmediateChildrenWyptIDs.begin(); childIT != allImmediateChildrenWyptIDs.end(); childIT++) {
					if (typeOfWaypoint == Output) {
						if(firstTime) {
				if (print)
							myfile << " ?" << wpNameMap[*childIT];
							firstTime = false;
						}
						else
				if (print)
							myfile << ", ?" << wpNameMap[*childIT];
					} else {
						if(firstTime) {

				if (print)
							myfile << " " << wpNameMap[*childIT];
							firstTime = false;
						}
						else
				if (print)
							myfile << ", " << wpNameMap[*childIT];
					}
				}
				if (print)
				myfile << ";\n";
			}	


			// Once you have declared the waypoint now create a structure for each query
			
			myfile << "waypoint " << wpNameMap[sortedWaypointIDs[i]] << " {\n";
			for(int j = 0; j < recIndexes.size(); j++) {
				if (oldQueries.find(_myNetwork._records[recIndexes[j]]._qID) == oldQueries.end()) { // Ss
				myfile << "\tquery Q" << _myNetwork._records[recIndexes[j]]._qID << " { ";
				
				if(_myNetwork._records[recIndexes[j]]._myType == Selection) {
					myfile << "filter " << _myNetwork._records[recIndexes[j]]._parseString << ";";
				}				
				else if(_myNetwork._records[recIndexes[j]]._myType == Join) {					
					myfile << "join ";					
					bool matched = false;
					for(int hashIter = 0; hashIter < lhsHashRel.size(); hashIter++) {
						if(lhsHashRel[hashIter].compare(_myNetwork._records[recIndexes[j]]._lhsRel) == 0 
						&& lhsHashAtt[hashIter].compare(_myNetwork._records[recIndexes[j]]._lhsAtt) == 0) {
							myfile << _myNetwork._records[recIndexes[j]]._rhsRel << "."
								<< _myNetwork._records[recIndexes[j]]._rhsAtt;
							matched = true;
							break;
						}
						else if(lhsHashRel[hashIter].compare(_myNetwork._records[recIndexes[j]]._rhsRel) == 0 
						&& lhsHashAtt[hashIter].compare(_myNetwork._records[recIndexes[j]]._rhsAtt) == 0) {
							myfile << _myNetwork._records[recIndexes[j]]._lhsRel << "."
								<< _myNetwork._records[recIndexes[j]]._lhsAtt;
							matched = true;
							break;
						}
					}
					if(!matched) {
						cout << "\nError: While printing the DP file: Neither LHS or RHS match the hashing attribute\n";
						exit(0);
					}
					myfile << ";";
				}
				else if(_myNetwork._records[recIndexes[j]]._myType == Aggregate) {
					for(__int64_t innerIT = 0; innerIT < _myNetwork._records[recIndexes[j]]._aggregatePreds.size(); innerIT++) {
					  myfile << "aggregate " <<  _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].attName << "="
						 << _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].attType
						 << "(" << _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].expression << "); ";
			
					    // myfile << "expr=\"" <<  _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].expression << "\" ";
					    // myfile << "name=\"" << _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].attName << "\" ";
					    // myfile << "func=\"" << _myNetwork._records[recIndexes[j]]._aggregatePreds[innerIT].attType << "\" ";

					}
				}
				else if(_myNetwork._records[recIndexes[j]]._myType == Top) {
					// myfile << "top " << _myNetwork._records[recIndexes[j]]._parseString << ";";
					cout << "\nError: While printing the DP file: Top waypoint not supported in this format\n";
					exit(0);
				}
				else if(_myNetwork._records[recIndexes[j]]._myType == Output) {
					for(map<string, string>::iterator innerIT = _myNetwork._records[recIndexes[j]]._outputPred.derivedColumns.begin();
						innerIT != _myNetwork._records[recIndexes[j]]._outputPred.derivedColumns.end(); innerIT++) {
						myfile << "print ";
						myfile << innerIT -> second << "; ";
						//myfile << "file=\"" << _myNetwork._records[recIndexes[j]]._outputPred.outputFile << "\" ";
						//myfile << "name=\"" << innerIT -> first << "\" ";
						//myfile << "expr=\"" << innerIT -> second << "\" ";
					}
					
				}
				else {
					cout << "\nError: While printing the DP file: Incorrect type of waypoint: 1\n";
					exit(0);
				}
				myfile << " }\n";
			}
			}
			myfile << "}\n";
		} // Output only non tablescan of loop
	}

	myfile << "\nrun query";
	firstTime = true;
	for(set<__int64_t>::iterator it1 = queryIDs.begin(); it1 != queryIDs.end(); it1++) {
		if (oldQueries.find(*it1) == oldQueries.end()) {
		if(firstTime) {
			myfile << " Q";
			firstTime = false;
		}
		else {
			myfile << ", Q";				
		}
		myfile << *it1;
		}
	}
	myfile << ";\n\n";
	// then define waypoints
	myfile.close();
}

// ########################################################

// ############## Interface to the Parser  ##############
Optimizer :: Optimizer(NetworkIntegrator& initialNI, string outputFileName, __int64_t searchType)
{
    _nextNI = initialNI;
    _outputFileName = outputFileName;
    // Ignore the search type passed by the Parser for now
    // _searchType = searchType;

	// opening the pipe to talk to the rest of the system
    cout << SYSTEM_PIPE_NAME << "\n";
    systemPipe = fopen(SYSTEM_PIPE_NAME, "w");
	
    assert(systemPipe!=NULL);

}

void Optimizer :: InsertPredicate(PredicateType myType, Operator myOp, string parseString, string lhsRel, string rhsRel, string lhsAtt, string rhsAtt, __int64_t qID)
{
    // This function is only for backward compatibility and checking for valid input
	if(myType == TableScan) {
		assert(myOp == NotApplicable);
		assert(rhsRel.compare(IGNORE_NAME) == 0);
		assert(lhsRel.compare(IGNORE_NAME) != 0);
		assert(lhsAtt.compare(IGNORE_NAME) == 0);
		assert(rhsAtt.compare(IGNORE_NAME) == 0);
		assert(qID >= 0);
	}
	else if(myType == Selection) {
		assert(myOp != NotApplicable);
		assert(rhsRel.compare(IGNORE_NAME) == 0);
		assert(lhsRel.compare(IGNORE_NAME) != 0);
		// Bypass attribute
		// assert(lhsAtt.compare(IGNORE_NAME) != 0);
		assert(rhsAtt.compare(IGNORE_NAME) == 0);
		assert(qID >= 0);
	}
	else if(myType == Join) {
		assert(myOp != NotApplicable);
		assert(rhsRel.compare(IGNORE_NAME) != 0);
		assert(lhsRel.compare(IGNORE_NAME) != 0);
		assert(lhsAtt.compare(IGNORE_NAME) != 0);
		assert(rhsAtt.compare(IGNORE_NAME) != 0);
		assert(qID >= 0);
	}
	else {
		cout << "Use InsertPredicate for TableScan, Selection and Join. For Top predicate use InsertTopPredicate method\n";
		assert(false);
	}

    _nextNI.InsertPredicate(myType, myOp, parseString, lhsRel, rhsRel, lhsAtt, rhsAtt, qID);
}

void Optimizer :: InsertTopPredicate(vector<TopAggregate> aggregatePreds, vector<TopDisjunction> disjunctionPreds,
        TopOutput outputPred, TopGroupBy groupByPred, __int64_t qID)
{
    // This function is only for backward compatibility
    _nextNI.InsertTopPredicate(aggregatePreds, disjunctionPreds, outputPred, groupByPred, qID);
}

void Optimizer :: Optimize()
{
    LoadConfiguration(); // Load Default configuration
    _nextNI._myPtrConfig = &_myConfig;

	if(_myConfig._searchType.compare("LookAhead") == 0){
		// First find which queries are still remaining
		set<__int64_t> remainingQueryIDs = _nextNI.GetRemainingQueryIDs();

		if(remainingQueryIDs.size() == 0) {
			cout << "Error: Why did you call optimize when you have no queries. (If its a mistake, may be you forgot to insert the query)\n";
			assert(false);
		}
		else if(remainingQueryIDs.size() > 1) {
			cout << "Optimize Warning: You are trying batch optimization. Contact Niketan first\n";
		}

		// Do Search
		for(set<__int64_t> :: iterator it = remainingQueryIDs.begin();
				it != remainingQueryIDs.end(); it++) {
            		LookAheadSearch(*it);
			(this -> _nextNI)._stateOfNI = NOT_MAPPED;
		}

	}
	else if (_myConfig._searchType.compare("Greedy") == 0) {
		// First find which queries are still remaining
		set<__int64_t> remainingQueryIDs = _nextNI.GetRemainingQueryIDs();

		if(remainingQueryIDs.size() == 0) {
			cout << "Error: Why did you call optimize when you have no queries. (If its a mistake, may be you forgot to insert the query)\n";
			assert(false);
		}
		else if(remainingQueryIDs.size() > 1) {
			cout << "Optimize Warning: You are trying batch optimization. Contact Niketan first\n";
		}

		// Do Search
		for(set<__int64_t> :: iterator it = remainingQueryIDs.begin();
				it != remainingQueryIDs.end(); it++) {
			// GreedySearch(*it);
			cout << "Error: Greedy Search not supported by the optimizer.\n";
			assert(false);
			(this -> _nextNI)._stateOfNI = NOT_MAPPED;
		}
	}
	else {
		cout << "Error: In Optimizer's Constructor Invalid Search type.\n";
		assert(false);
	}

}

set<__int64_t> NetworkIntegrator :: GetAllQueries()
{
    set<__int64_t> retVal;
    for(__int64_t i = 0; i < _myNetwork._records.size(); i++) {
        retVal.insert(_myNetwork._records[i]._qID);
    }
    return retVal;
}

NetworkIntegrator :: NetworkIntegrator()
{
    _stateOfNI = NOT_MAPPED;
    _myCoster._myNetwork = &_myNetwork; // My cost should always point to myNetwork
    _myCoster._myPtrConfig = _myPtrConfig;
}

// ########################################################

NetworkIntegrator :: NetworkIntegrator(const NetworkIntegrator& copyMe)
{
    if(this != &copyMe) {
        *this = copyMe; // Calls operator=
        _myCoster._myNetwork = &_myNetwork; // My cost should always point to myNetwork
        _myCoster._myPtrConfig = _myPtrConfig;
    }
}

NetworkIntegrator& NetworkIntegrator :: operator=(const NetworkIntegrator& copyMe)
{
    _myNetwork = copyMe._myNetwork;
    _removeRecID = copyMe._removeRecID;
    _myCoster = copyMe._myCoster;
    _stateOfNI = copyMe._stateOfNI;
    _myPtrConfig = copyMe._myPtrConfig;
    _myCoster._myNetwork = &_myNetwork; // My cost should always point to myNetwork
    _myCoster._myPtrConfig = _myPtrConfig;
    return *this;
}

set<__int64_t> NetworkIntegrator :: GetRemainingQueryIDs()
{
    set<__int64_t> remQIDs;
    for(__int64_t i = 0; i < _myNetwork._records.size(); i++) {
        if(_myNetwork._records[i]._wyptID == NOT_MAPPED) {
            remQIDs.insert(_myNetwork._records[i]._qID);
        }
    }
    return remQIDs;
}

bool NetworkIntegrator :: IsIntegrationDone(__int64_t queryID)
{
    if(GetRemainingQueryIDs().size() == 0)
        return true;
    else
        return false;
}

void NetworkIntegrator :: MapTopAndCartesianProducts(__int64_t queryID)
{
    set<__int64_t> tblScansForTheQuery = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfQIDsSame,
            IGNORE_NAME, IGNORE_NAME, queryID, TableScan);

    set<__int64_t> topIDsForTheQuery;
    for(set<__int64_t>::iterator it = tblScansForTheQuery.begin(); it != tblScansForTheQuery.end(); it++) {
        set<__int64_t> temp = _myNetwork.GetSpecificWypts(*it, GetTopMostParents, TrueIfQIDsSame,
            IGNORE_NAME, IGNORE_NAME, queryID, NAPredType);
        for(set<__int64_t>::iterator it1 = temp.begin(); it1 != temp.end(); it1++) {
            topIDsForTheQuery.insert(*it1);
        }
    }

    assert(topIDsForTheQuery.size() > 0);

    while(topIDsForTheQuery.size() > 1) {
	cout << "Error: Cartesian Products not supported\n";
	ConvertPathNetworkToDotFile("hello.dot");
	assert(false);
        // Cartesian products necessary
        __int64_t wypt1, wypt2;

		set<__int64_t>::iterator it = topIDsForTheQuery.begin();
		wypt1 = *it;
		it++;
		wypt2 = *it;

		vector<__int64_t> predIndexes1 = _myNetwork.GetRecordIndexesForWaypoint(wypt1);
		vector<__int64_t> predIndexes2 = _myNetwork.GetRecordIndexesForWaypoint(wypt2);

        string lhsRel, rhsRel;
        for(__int64_t i = 0; i < predIndexes1.size(); i++) {
			if(_myNetwork._records[predIndexes1[i]]._qID == queryID) {
				lhsRel = _myNetwork._records[predIndexes1[i]]._lhsRel;
				break;
			}
		}
		for(__int64_t i = 0; i < predIndexes2.size(); i++) {
			if(_myNetwork._records[predIndexes2[i]]._qID == queryID) {
				rhsRel = _myNetwork._records[predIndexes2[i]]._lhsRel;
				break;
			}
		}

		OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
		newRec._myType = Join;
		newRec._myOp = CARTESIAN;
		newRec._parseString = lhsRel + " CARTESIAN PRODUCT " + rhsRel;
        newRec._lhsRel = lhsRel;
        newRec._rhsRel = rhsRel;
        newRec._lhsAtt = IGNORE_NAME;
        newRec._rhsAtt = IGNORE_NAME;
        newRec._qID = queryID;
        newRec._wyptID = _myNetwork.GetNewID("WAYPOINT");
        newRec._groupID = NO_GROUP_CARTESIAN;
        OptimizerEdge newEdge1(_myNetwork.GetNewID("EDGE"));
        newEdge1._startWyptID = wypt1;
        newEdge1._endWyptID = newRec._wyptID;
        OptimizerEdge newEdge2(_myNetwork.GetNewID("EDGE"));
        newEdge2._startWyptID = wypt2;
        newEdge2._endWyptID = newRec._wyptID;

        // Now insert them
        _myNetwork._records.push_back(newRec);
        _myNetwork._edges.push_back(newEdge1);
        _myNetwork._edges.push_back(newEdge2);

        topIDsForTheQuery.erase(wypt1);
        topIDsForTheQuery.erase(wypt2);
    }

    // Now we have only 1 topWypt !!!
	__int64_t myTopWyptID = *(topIDsForTheQuery.begin());

    bool topDone = false;
	__int64_t topIndex = -1;
	for(__int64_t i = 0; i < _myNetwork._records.size(); i++) {
		if(_myNetwork._records[i]._wyptID == NOT_MAPPED && _myNetwork._records[i]._qID == queryID) {

			if(_myNetwork._records[i]._myType != Top) {
				cout << "Error: In MapTopAndCartesianProducts, expected Top predicate\n";
				assert(false);
			}

			if(topDone) {
				cout << "Optimizer expects only 1 top predicate for query:" << queryID << endl;
				assert(false);
			}

			topIndex = i; // This is to avoid any conflict while updating vector during iterating

			topDone = true;
		}
	}

    if(topIndex != -1) {


		// ----------------------------------------------------------
		// Logic for Aggregate(Create Pred), Disjunction(Map), GroupBy(Ignore) and Output(Create Pred)

		// First add disjunction to the joins
		for(__int64_t i = 0; i < _myNetwork._records[topIndex]._disjunctionPreds.size(); i++) {
			if(_myNetwork._records[topIndex]._disjunctionPreds[i].baseTables.size() < 1) {
				cout << "Error: The Disjunction " << _myNetwork._records[topIndex]._disjunctionPreds[i].expression << " cannot have "
                     << _myNetwork._records[topIndex]._disjunctionPreds[i].baseTables.size() << " tables.\n";
				assert(false);
			}

			if(_myNetwork._records[topIndex]._disjunctionPreds[i].baseTables.size() == 1) {
				string tblName = _myNetwork._records[topIndex]._disjunctionPreds[i].baseTables[0];
				__int64_t selectionWyptID = -1;
				for(__int64_t selIter = 0; selIter < _myNetwork._records.size(); selIter++) {
					if(_myNetwork._records[selIter]._myType == Selection && _myNetwork._records[selIter]._lhsRel.compare(tblName) == 0) {
						selectionWyptID = _myNetwork._records[selIter]._wyptID;
					}
				}

				assert(selectionWyptID != -1);

				// Special case for disjunction, just create a new Selection Predicate
                OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
                newRec._myType = Selection;
                newRec._myOp = LT;
                newRec._parseString = _myNetwork._records[topIndex]._disjunctionPreds[i].expression;
                newRec._lhsRel = tblName;
                newRec._rhsRel = IGNORE_NAME;
                newRec._lhsAtt = IGNORE_NAME;
                newRec._rhsAtt = IGNORE_NAME;
                newRec._qID = queryID;
                newRec._wyptID = selectionWyptID;
                newRec._groupID = NO_GROUP_OTHER;
                _myNetwork._records.push_back(newRec);
				continue;
			}

            assert(_myNetwork._records[topIndex]._disjunctionPreds[i].baseTables.size() > 0);
            // Find lowest wypt with the tbls
			__int64_t joinWypt = _myNetwork.GetLowestWaypointWithTablesForDisjunction(queryID,
                _myNetwork._records[topIndex]._disjunctionPreds[i].baseTables);
            assert(joinWypt != NOT_MAPPED);

		//-----------------------------------------------------------
		// Nike Change for supporting filter expression in predicate rather than waypoint
		assert(false); // We donot support disjunction: Why? What will happen if we give query with only disjunctions and no equi-join predicates 
            __int64_t grpIndex = _myNetwork.GetGroupIndex(joinWypt);
            if(_myNetwork._groups[grpIndex]._filterExpression.empty()) {
                _myNetwork._groups[grpIndex]._filterExpression = _myNetwork._records[topIndex]._disjunctionPreds[i].expression;
            }
            else {
                _myNetwork._groups[grpIndex]._filterExpression += " && " +  _myNetwork._records[topIndex]._disjunctionPreds[i].expression;
            }
		//-----------------------------------------------------------


		}

		__int64_t aggWyptID = -1;
		if(_myNetwork._records[topIndex]._aggregatePreds.size() != 0) {
			// Then Aggregates
			// If there exist an aggregate waypoint which is direct parent of myTopWyptID, then map all aggregates onto it
			// Else create a new aggregate waypoint

			set<__int64_t> tempAgg = _myNetwork.GetSpecificWypts(myTopWyptID, GetAllImmediateParents, AlwayTrue,
                IGNORE_NAME, IGNORE_NAME, -1, Aggregate);
            assert(tempAgg.size() <= 1); // Only 1 aggregate allowed on top of any given waypoint
            if(tempAgg.size() == 0)
                aggWyptID = -1;
            else
                aggWyptID = *(tempAgg.begin());

			if(aggWyptID == -1) {
				// Mapping not possible, Create new waypoint and connect to myTopWyptID

				// First create a predicate and a record for aggregate
				OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
                newRec._myType = Aggregate;
                newRec._myOp = NotApplicable;
                newRec._parseString = "";
                newRec._lhsRel = IGNORE_NAME;
                newRec._rhsRel = IGNORE_NAME;
                newRec._lhsAtt = IGNORE_NAME;
                newRec._rhsAtt = IGNORE_NAME;
                newRec._qID = queryID;
                newRec._wyptID = _myNetwork.GetNewID("WAYPOINT");
                newRec._groupID = NO_GROUP_OTHER;

                for(__int64_t innerIter = 0; innerIter < _myNetwork._records[topIndex]._aggregatePreds.size(); innerIter++) {
					newRec._aggregatePreds.push_back(_myNetwork._records[topIndex]._aggregatePreds[innerIter]);
				}

                OptimizerEdge newEdge(_myNetwork.GetNewID("EDGE"));
                newEdge._startWyptID = myTopWyptID;
                newEdge._endWyptID = newRec._wyptID;

                // Now insert them
                _myNetwork._records.push_back(newRec);
                _myNetwork._edges.push_back(newEdge);

				// This is for output waypoint
				aggWyptID = newRec._wyptID;
			}
			else {
				// Map onto aggWyptID

				// First create a record
				OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
                newRec._myType = Aggregate;
                newRec._myOp = NotApplicable;
                newRec._parseString = "";
                newRec._lhsRel = IGNORE_NAME;
                newRec._rhsRel = IGNORE_NAME;
                newRec._lhsAtt = IGNORE_NAME;
                newRec._rhsAtt = IGNORE_NAME;
                newRec._qID = queryID;
                newRec._groupID = NO_GROUP_OTHER;
                // Mapping implies same waypointID
                newRec._wyptID = aggWyptID;

                for(__int64_t innerIter = 0; innerIter < _myNetwork._records[topIndex]._aggregatePreds.size(); innerIter++) {
					newRec._aggregatePreds.push_back(_myNetwork._records[topIndex]._aggregatePreds[innerIter]);
				}
				_myNetwork._records.push_back(newRec);
			}
		}

		if(_myNetwork._records[topIndex]._groupByPred.isPresent) {
			cout << "Sorry, Group By Predicates are not supported by the Optimizer.\n";
			assert(false);
		}

		// Finally output
		if(_myNetwork._records[topIndex]._outputPred.isPresent) {
			__int64_t outputWyptID = -1;
			bool isOutputMapped = false;

			if(aggWyptID == -1) {
			    set<__int64_t> tempOutput = _myNetwork.GetSpecificWypts(myTopWyptID, GetAllImmediateParents, AlwayTrue,
                    IGNORE_NAME, IGNORE_NAME, -1, Output);
                assert(tempOutput.size() <= 1); // Only 1 output allowed on top of any given waypoint
                if(tempOutput.size() == 0)
                    outputWyptID = -1;
                else
                    outputWyptID = *(tempOutput.begin());
			}
			else {
				set<__int64_t> tempOutput = _myNetwork.GetSpecificWypts(aggWyptID, GetAllImmediateParents, AlwayTrue,
                    IGNORE_NAME, IGNORE_NAME, -1, Output);
                assert(tempOutput.size() <= 1); // Only 1 output allowed on top of any given waypoint
                if(tempOutput.size() == 0)
                    outputWyptID = -1;
                else
                    outputWyptID = *(tempOutput.begin());
			}

			if(outputWyptID == -1) {
				// Mapping output not possible. Create a new waypoint

				// First a record for output
				OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
                newRec._myType = Output;
                newRec._myOp = NotApplicable;
                newRec._parseString = "";
                newRec._lhsRel = IGNORE_NAME;
                newRec._rhsRel = IGNORE_NAME;
                newRec._lhsAtt = IGNORE_NAME;
                newRec._rhsAtt = IGNORE_NAME;
                newRec._qID = queryID;
                newRec._groupID = NO_GROUP_OTHER;
                newRec._wyptID = _myNetwork.GetNewID("WAYPOINT");
                newRec._outputPred = _myNetwork._records[topIndex]._outputPred;
				_myNetwork._records.push_back(newRec);

				outputWyptID = newRec._wyptID;
				isOutputMapped = false;

			}
			else {
				// Map onto outputWyptID

				OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
                newRec._myType = Output;
                newRec._myOp = NotApplicable;
                newRec._parseString = "";
                newRec._lhsRel = IGNORE_NAME;
                newRec._rhsRel = IGNORE_NAME;
                newRec._lhsAtt = IGNORE_NAME;
                newRec._rhsAtt = IGNORE_NAME;
                newRec._qID = queryID;
                newRec._groupID = NO_GROUP_OTHER;
                newRec._outputPred = _myNetwork._records[topIndex]._outputPred;

				// Mapping implies same waypointID
				newRec._wyptID = outputWyptID;
				_myNetwork._records.push_back(newRec);

				isOutputMapped = true;
			}

			// Add edge only if output is not mapped, i.e new waypoint is created
			if(aggWyptID == -1 && !isOutputMapped) {
				// Connect myTopWyptID to output
				OptimizerEdge newEdge(_myNetwork.GetNewID("EDGE"));
                newEdge._startWyptID = myTopWyptID;
                newEdge._endWyptID = outputWyptID;
                _myNetwork._edges.push_back(newEdge);
			}
			else if(aggWyptID != -1 && !isOutputMapped) {
				// Connect agg to output
				OptimizerEdge newEdge(_myNetwork.GetNewID("EDGE"));
                newEdge._startWyptID = aggWyptID;
                newEdge._endWyptID = outputWyptID;
                _myNetwork._edges.push_back(newEdge);
			}

		}
		else {
			if(aggWyptID != -1) {
				cout << "Is this an error, aggregate present but not output waypoint for the query " << queryID << ". Inform Niketan if you don't think this is an error\n";
				assert(false);
			}
		}

		// At end remove the old entry _records[topIndex]
		_myNetwork._records.erase(_myNetwork._records.begin() + topIndex);
		// ----------------------------------------------------------
	}
}

void NetworkIntegrator :: MapTableScans(__int64_t queryID)
{
    set<__int64_t> unMappedTblScanIndexes = _myNetwork.GetUnMappedRecordIndexes(TrueIfQIDsSame, IGNORE_NAME, IGNORE_NAME, queryID, TableScan);

    for(set<__int64_t> :: iterator it = unMappedTblScanIndexes.begin(); it != unMappedTblScanIndexes.end(); it++) {
        string tblName = _myNetwork._records[*it]._lhsRel;
        set<__int64_t> alreadyMappedTblScanIDs = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfTablesSame,
            tblName, IGNORE_NAME, -1, TableScan);
        assert(alreadyMappedTblScanIDs.size() <= 1); // More than 1 tablescans wypt with same table name not allowed
        if(alreadyMappedTblScanIDs.size() == 1) {
            // If Mapping possible try to map
            assert(_myNetwork._records[*it]._wyptID == NOT_MAPPED); // Trying to map already mapped waypoint: Check GetUnMappedRecordIndexes
            _myNetwork._records[*it]._wyptID = *(alreadyMappedTblScanIDs.begin());
        }
        else {
            // If mapping not possible, create a new waypoint => No edge required
            _myNetwork._records[*it]._wyptID = _myNetwork.GetNewID("WAYPOINT");
        }
    }
}

void NetworkIntegrator :: MapSelections(__int64_t queryID)
{
    set<__int64_t> unMappedSelectionIndexes = _myNetwork.GetUnMappedRecordIndexes(TrueIfQIDsSame, IGNORE_NAME, IGNORE_NAME, queryID, Selection);

    for(set<__int64_t> :: iterator it = unMappedSelectionIndexes.begin(); it != unMappedSelectionIndexes.end(); it++) {
        string tblName = _myNetwork._records[*it]._lhsRel;
        set<__int64_t> alreadyMappedSelectionIDs = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfTablesSame,
            tblName, IGNORE_NAME, -1, Selection);
        assert(alreadyMappedSelectionIDs.size() <= 1); // More than 1 Selection wypt with same table name not allowed
        if(alreadyMappedSelectionIDs.size() == 1) {
            // If Mapping possible try to map
            assert(_myNetwork._records[*it]._wyptID == NOT_MAPPED); // Trying to map already mapped waypoint: Check GetUnMappedRecordIndexes
            _myNetwork._records[*it]._wyptID = *(alreadyMappedSelectionIDs.begin());
        }
        else {
            // If mapping not possible, create a new waypoint => Need to Add an edge
            _myNetwork._records[*it]._wyptID = _myNetwork.GetNewID("WAYPOINT");

            // Get TableScan corresponding to the Selection
            set<__int64_t> tblScansCorrespondingToSelection = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfTablesSame,
                tblName, IGNORE_NAME, -1, TableScan);
            assert(tblScansCorrespondingToSelection.size() == 1); // Exactly 1 tablescan corresponding to the selection

            OptimizerEdge newEdge(_myNetwork.GetNewID("EDGE"));
            newEdge._startWyptID = *(tblScansCorrespondingToSelection.begin());
            newEdge._endWyptID = _myNetwork._records[*it]._wyptID;
            _myNetwork._edges.push_back(newEdge);
        }
    }
}

pair< string, string > NetworkIntegrator :: GetTablesInvolved(__int64_t recIndex)
{
    string tbl1 = _myNetwork._records[recIndex]._lhsRel;
    string tbl2 = _myNetwork._records[recIndex]._rhsRel;

    assert(tbl1.compare(IGNORE_NAME) != 0 && tbl2.compare(IGNORE_NAME) != 0); // Since this method is always called on joins

    // Order pair lexicographically
    if(tbl1.compare(tbl2) < 0) {
        return pair<string, string>(tbl1, tbl2);
    }
    else
        return pair<string, string>(tbl2, tbl1);
}

vector<NetworkIntegrator> NetworkIntegrator :: CreateNewJoinWypt(__int64_t queryID, pair<string, string> tblsInvolved, set<__int64_t> unMappedJoinIndexes, __int64_t topWypt1, __int64_t topWypt2)
{
	cout << "Creating wypt for tbls " << tblsInvolved.first << " and " << tblsInvolved.second << " where top wypts are " << topWypt1 << " and " << topWypt2 << endl;

	bool handleScenario1 = true;
	bool handleScenario2 = true;
	#ifdef OPTIMIZE_FOR_EXECUTION_ENGINE
	// Only handle this for simple case
	{
	  __int64_t card1 = _myCoster.GetCardinality(tblsInvolved.first);
	  __int64_t	 card2 = _myCoster.GetCardinality(tblsInvolved.second);
		// Nike_India Code: Allow handling both cases if size of tables is approximately same
		if(card1 > (card2 * 2)) {
			handleScenario1 = true;
			handleScenario2 = false;
			cout << "Handle Scenario 1 (" << tblsInvolved.first << ", " << card1 << ") (" << tblsInvolved.second << ", " << card2 << ")\n";
		}
		else if((card1 * 2)  < card2) {
			handleScenario1 = false;
			handleScenario2 = true;
			cout << "Handle Scenario 2 (" << tblsInvolved.first << ", " << card1 << ") (" << tblsInvolved.second << ", " << card2 << ")\n";
		}
		else {
			// Handle both cases
			handleScenario1 = true;
			handleScenario2 = true;
		}

	}
	#endif

    vector<NetworkIntegrator> retVal;
    //*****************************************************************************
    // Scenario1: Creations of new waypoint with lhs table: tblsInvolved.first (Maximal Grouping)
    // topWypt1 ---> New wypt
    // topWypt2 -------^
    //*****************************************************************************
    if(handleScenario1)
    {
    cout << "LHSTbl:" << tblsInvolved.first << endl;
    // Create copy of NI
    NetworkIntegrator newNI1(*this);
    // Create New group
    OptimizerGroup newGrp1;
    __int64_t newWyptID1 = newNI1._myNetwork.GetNewID("WAYPOINT");
    newGrp1._groupID = newNI1._myNetwork.GetNewID("GROUP");
    for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
        newGrp1._lhsHashRel.push_back(tblsInvolved.first);
        newGrp1._rhsHashRel.push_back(tblsInvolved.second);
        if(newNI1._myNetwork._records[*it]._lhsRel.compare(tblsInvolved.first) == 0) {
            newGrp1._lhsHashAtt.push_back(newNI1._myNetwork._records[*it]._lhsAtt);
            newGrp1._rhsHashAtt.push_back(newNI1._myNetwork._records[*it]._rhsAtt);
        }
        else {
            newGrp1._rhsHashAtt.push_back(newNI1._myNetwork._records[*it]._lhsAtt);
            newGrp1._lhsHashAtt.push_back(newNI1._myNetwork._records[*it]._rhsAtt);
        }
        newNI1._myNetwork._records[*it]._wyptID = newWyptID1;
        newNI1._myNetwork._records[*it]._groupID = newGrp1._groupID;
    }
    newGrp1._filterExpression = "";
    newGrp1._bypassString = "";
    // Add an edge
    OptimizerEdge newEdge1a(newNI1._myNetwork.GetNewID("EDGE"));
    newEdge1a._startWyptID = topWypt1;
    newEdge1a._endWyptID = newWyptID1;
    OptimizerEdge newEdge1b(newNI1._myNetwork.GetNewID("EDGE"));
    newEdge1b._startWyptID = topWypt2;
    newEdge1b._endWyptID = newWyptID1;
    newNI1._myNetwork._edges.push_back(newEdge1a);
    newNI1._myNetwork._edges.push_back(newEdge1b);
    newNI1._myNetwork._groups.push_back(newGrp1);
    retVal.push_back(newNI1);
    }

    //*****************************************************************************
    // Scenario2: Creations of new waypoint with lhs table: tblsInvolved.second (Maximal Grouping)
    // topWypt1 ---> New wypt
    // topWypt2 -------^
    //*****************************************************************************

    if(handleScenario2)
    {
    cout << "LHSTbl:" << tblsInvolved.second << endl;
    // Create copy of NI
    NetworkIntegrator newNI2(*this);
    // Create New group
    OptimizerGroup newGrp2;
    __int64_t newWyptID2 = newNI2._myNetwork.GetNewID("WAYPOINT");
    newGrp2._groupID = newNI2._myNetwork.GetNewID("GROUP");
    for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
        newGrp2._lhsHashRel.push_back(tblsInvolved.second);
        newGrp2._rhsHashRel.push_back(tblsInvolved.first);
        if(newNI2._myNetwork._records[*it]._lhsRel.compare(tblsInvolved.second) == 0) {
            newGrp2._lhsHashAtt.push_back(newNI2._myNetwork._records[*it]._lhsAtt);
            newGrp2._rhsHashAtt.push_back(newNI2._myNetwork._records[*it]._rhsAtt);
        }
        else {
            newGrp2._rhsHashAtt.push_back(newNI2._myNetwork._records[*it]._lhsAtt);
            newGrp2._lhsHashAtt.push_back(newNI2._myNetwork._records[*it]._rhsAtt);
        }
        newNI2._myNetwork._records[*it]._wyptID = newWyptID2;
        newNI2._myNetwork._records[*it]._groupID = newGrp2._groupID;
    }
    newGrp2._filterExpression = "";
    newGrp2._bypassString = "";
    // Add an edge
    OptimizerEdge newEdge2a(newNI2._myNetwork.GetNewID("EDGE"));
    newEdge2a._startWyptID = topWypt1;
    newEdge2a._endWyptID = newWyptID2;
    OptimizerEdge newEdge2b(newNI2._myNetwork.GetNewID("EDGE"));
    newEdge2b._startWyptID = topWypt2;
    newEdge2b._endWyptID = newWyptID2;
    newNI2._myNetwork._edges.push_back(newEdge2a);
    newNI2._myNetwork._edges.push_back(newEdge2b);
    newNI2._myNetwork._groups.push_back(newGrp2);
    retVal.push_back(newNI2);
    }

#ifdef ALLOW_SUBSET_JOIN_GROUPING_WHILE_CREATION
    //*****************************************************************************
    // Scenario3: Creations of new waypoint with lhs table: tblsInvolved.first (Subset Grouping)
    // topWypt1 ---> New wypt
    // topWypt2 -------^
    //*****************************************************************************
    cout << "TODO: Subset grouping not supported as of now :( But is expected to give same result because cost function doesnot look inside disjunctions/filter strings\n";
    assert(false);

    //*****************************************************************************
    // Scenario4: Creations of new waypoint with lhs table: tblsInvolved.second (Subset Grouping)
    // topWypt1 ---> New wypt
    // topWypt2 -------^
    //*****************************************************************************
#endif

    return retVal;
}

bool IsPredicatePresentInGroup(OptimizerGroup* grp, string lhsRel, string lhsAtt, string rhsRel, string rhsAtt) {
    for(__int64_t i = 0; i < (grp -> _lhsHashRel).size(); i++) {
        if( lhsRel.compare((grp -> _lhsHashRel)[i]) == 0 &&
            lhsAtt.compare((grp -> _lhsHashAtt)[i]) == 0 &&
            rhsRel.compare((grp -> _rhsHashRel)[i]) == 0 &&
            rhsAtt.compare((grp -> _rhsHashAtt)[i]) == 0) {
            return true;
        }
    }
    return false;
}

set<OptimizerGroup> NetworkIntegrator :: GetGroups(set<__int64_t> unMappedJoinIndexes, bool allPossible)
{
	cout << "\nCalled GetGroups!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    assert(unMappedJoinIndexes.size() > 0);
    set<__int64_t>::iterator it = unMappedJoinIndexes.begin();
    string tbl1 = _myNetwork._records[*it]._lhsRel;
    string tbl2 = _myNetwork._records[*it]._rhsRel;

    set<OptimizerGroup> retVal;

    if(!allPossible) {
        OptimizerGroup grp1; // LHS hash is tbl1

        for(it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
            if(_myNetwork._records[*it]._lhsRel.compare(tbl1) == 0) {
		 #ifdef PRINT_COST_PATH_NETWORK
					cout << "1: Grp1: " << *it <<  ": Tbl1: " << tbl1 << ": LHS Hash: " << _myNetwork._records[*it]._lhsRel << endl;
		#endif
                // LHS Hash is tbl1
                if(!IsPredicatePresentInGroup(&grp1, _myNetwork._records[*it]._lhsRel, _myNetwork._records[*it]._lhsAtt,
                    _myNetwork._records[*it]._rhsRel, _myNetwork._records[*it]._rhsAtt)) {
                grp1._lhsHashRel.push_back(_myNetwork._records[*it]._lhsRel);
                grp1._lhsHashAtt.push_back(_myNetwork._records[*it]._lhsAtt);

                grp1._rhsHashRel.push_back(_myNetwork._records[*it]._rhsRel);
                grp1._rhsHashAtt.push_back(_myNetwork._records[*it]._rhsAtt);
                }
            }
            else if(_myNetwork._records[*it]._rhsRel.compare(tbl1) == 0) {
		 #ifdef PRINT_COST_PATH_NETWORK
                  cout << "2: Grp1: " << *it <<  ": Tbl1: " << tbl1 << ": LHS Hash: " << _myNetwork._records[*it]._rhsRel << endl;
		#endif
                // LHS Hash is tbl1
                if(!IsPredicatePresentInGroup(&grp1, _myNetwork._records[*it]._rhsRel, _myNetwork._records[*it]._rhsAtt,
                    _myNetwork._records[*it]._lhsRel, _myNetwork._records[*it]._lhsAtt)) {
                    grp1._lhsHashRel.push_back(_myNetwork._records[*it]._rhsRel);
                    grp1._lhsHashAtt.push_back(_myNetwork._records[*it]._rhsAtt);

                    grp1._rhsHashRel.push_back(_myNetwork._records[*it]._lhsRel);
                    grp1._rhsHashAtt.push_back(_myNetwork._records[*it]._lhsAtt);
                }
            }
            else {
                cout << "Cannot form groups since expected tables " << tbl1 << " and " << tbl2 << " but got "
                    << _myNetwork._records[*it]._lhsRel << " and " << _myNetwork._records[*it]._rhsRel << endl;
                assert(false);
            }
        }
        OptimizerGroup grp2; // LHS hash is tbl2
        for(it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
            if(_myNetwork._records[*it]._lhsRel.compare(tbl2) == 0) {
		 #ifdef PRINT_COST_PATH_NETWORK
                 cout << "3: Grp2: " << *it <<  ": Tbl2: " << tbl2 << ": LHS Hash: " << _myNetwork._records[*it]._lhsRel << endl;
		#endif
                if(!IsPredicatePresentInGroup(&grp2, _myNetwork._records[*it]._lhsRel, _myNetwork._records[*it]._lhsAtt,
                _myNetwork._records[*it]._rhsRel, _myNetwork._records[*it]._rhsAtt)) {
                    grp2._lhsHashRel.push_back(_myNetwork._records[*it]._lhsRel);
                    grp2._lhsHashAtt.push_back(_myNetwork._records[*it]._lhsAtt);
                    grp2._rhsHashRel.push_back(_myNetwork._records[*it]._rhsRel);
                    grp2._rhsHashAtt.push_back(_myNetwork._records[*it]._rhsAtt);
                }
            }
            else if(_myNetwork._records[*it]._rhsRel.compare(tbl2) == 0) {
		 #ifdef PRINT_COST_PATH_NETWORK
                 cout << "4: Grp2: " << *it <<  ": Tbl2: " << tbl2 << ": LHS Hash: " << _myNetwork._records[*it]._rhsRel << endl;
		#endif
                if(!IsPredicatePresentInGroup(&grp2, _myNetwork._records[*it]._rhsRel, _myNetwork._records[*it]._rhsAtt,
                    _myNetwork._records[*it]._lhsRel, _myNetwork._records[*it]._lhsAtt)){
                    grp2._lhsHashRel.push_back(_myNetwork._records[*it]._rhsRel);
                    grp2._lhsHashAtt.push_back(_myNetwork._records[*it]._rhsAtt);
                    grp2._rhsHashRel.push_back(_myNetwork._records[*it]._lhsRel);
                    grp2._rhsHashAtt.push_back(_myNetwork._records[*it]._lhsAtt);

                }
            }
            else {
                cout << "Cannot form groups since expected tables " << tbl1 << " and " << tbl2 << " but got "
                    << _myNetwork._records[*it]._lhsRel << " and " << _myNetwork._records[*it]._rhsRel << endl;
                assert(false);
            }
        }

	// Nike_India Code
	retVal.insert(grp1);
	retVal.insert(grp2);

        // Just to make sure that we have formed the right hashes
        #ifdef NIKE_DEBUG
        for(__int64_t i = 0; i < grp1._lhsHashRel.size(); i++) {
            assert(tbl1.compare(grp1._lhsHashRel[i]) == 0);
            // assert(tbl2.compare(grp1._rhsHashRel[i]) == 0);
        }
       //  cout << "<----\n";
        for(__int64_t i = 0; i < grp2._lhsHashRel.size(); i++) {
            // cout << "3:" << tbl2 << "-" << grp2._lhsHashRel[i] << endl;
            cout << "4:" << tbl1 << "-" << grp2._rhsHashRel[i] << endl;
            assert(tbl2.compare(grp2._lhsHashRel[i]) == 0);
            // assert(tbl1.compare(grp1._rhsHashRel[i]) == 0);
        }
        #endif

    }
    else {
        cout << "All possible groups not supported as of now !!!";
        assert(false);
    }

    return retVal;
}


vector<NetworkIntegrator> NetworkIntegrator :: MapJoin(__int64_t queryID, pair<string, string> tblsInvolved, set<__int64_t> unMappedJoinIndexes)
{
    vector<NetworkIntegrator> retVal;

    // Logic:
    // First check whether this group of predicates can be added as filter expressions or not
    // (This protects us from stupid user that adds more join predicates than necessary)
    // Eg: L JOIN P AND L JOIN S AND S JOIN P
    // If yes, add them else continue with MapJoin logic

    set<__int64_t> tblScan1 = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfQIDsANDTablesSame,
            tblsInvolved.first, IGNORE_NAME, queryID, TableScan);
    set<__int64_t> tblScan2 = _myNetwork.GetSpecificWypts(-1, GetAllMappedWaypoints, TrueIfQIDsANDTablesSame,
            tblsInvolved.second, IGNORE_NAME, queryID, TableScan);

    assert(tblScan1.size() == 1 && tblScan2.size() == 1);

    set<__int64_t> filterWypts;
    set<__int64_t> potentialFilter1a = _myNetwork.GetSpecificWypts(*(tblScan1.begin()), GetTopMostParents, TrueIfQIDsANDTablesSame,
            tblsInvolved.first, IGNORE_NAME, queryID, NAPredType);
    set<__int64_t> potentialFilter2a = _myNetwork.GetSpecificWypts(*(tblScan2.begin()), GetTopMostParents, TrueIfQIDsANDTablesSame,
            tblsInvolved.second, IGNORE_NAME, queryID, NAPredType);

    assert(potentialFilter1a.size() == 1 && potentialFilter2a.size() == 1);
	
    set<__int64_t> potentialFilter1b = _myNetwork.GetSpecificWypts(*(tblScan1.begin()), GetAllRecursiveParents, TrueIfQIDsSame,
            IGNORE_NAME, IGNORE_NAME, queryID, NAPredType);
    set<__int64_t> potentialFilter2b = _myNetwork.GetSpecificWypts(*(tblScan2.begin()), GetAllRecursiveParents, TrueIfQIDsSame,
            IGNORE_NAME, IGNORE_NAME, queryID, NAPredType);
	cout << "potential b" << potentialFilter1b.size() << ", " << potentialFilter2b.size() << endl;
	__int64_t tmpFilter1 = *(potentialFilter1a.begin());	
	//if( potentialFilter1b.size() > 1) {
		for(set<__int64_t>::iterator it = potentialFilter1b.begin(); it != potentialFilter1b.end(); it++) {
			if(_myNetwork.IsEdgePresent(tmpFilter1, *it, true)) {
				tmpFilter1 = *it;
				cout << "$$$$\n";
			}
			//cout << ":11:" << tmpFilter1 << ", " << *(potentialFilter1a.begin()) << endl;
			// assert(false); // Feature check
		}
	//} 
	__int64_t tmpFilter2 = *(potentialFilter2a.begin());	
	//if( potentialFilter2b.size() > 1) {
		for(set<__int64_t>::iterator it = potentialFilter2b.begin(); it != potentialFilter2b.end(); it++) {
			if(_myNetwork.IsEdgePresent(tmpFilter2, *it, true)) {
				tmpFilter2 = *it;
				cout << "$$$$\n";
			}
			//cout << ":22:" << tmpFilter2 << ", " << *(potentialFilter2a.begin()) << endl;
			// assert(false); // Feature check
		}
	//} 


    __int64_t topWypt1 = tmpFilter1;
    __int64_t topWypt2 = tmpFilter2;
	
    if(topWypt1 == topWypt2) {
	// This means add all unmapped join to the filter expression  topWypt2
        string myExpression = "";
        for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
            _removeRecID.insert(_myNetwork._records[*it]._recID);

            if(it == unMappedJoinIndexes.begin()) {
                string newEx = "(" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression = newEx;
            }
            else {
                string newEx = "&amp;&amp; (" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression += newEx;
            }
        }

        // --------------------------------------------------------------------------
        // Nike Change to support filter expression as part of predicate and not waypoint
        // First get index of mapped topWypt2
        vector<__int64_t> tmp = _myNetwork.GetRecordIndexesForWaypoint(topWypt2);
        int topIndex = -1;
        for(int iter = 0; iter < tmp.size(); iter++) {
                if(_myNetwork._records[tmp[iter]]._wyptID != NOT_MAPPED && _myNetwork._records[tmp[iter]]._qID == queryID) {
                        topIndex = tmp[iter];
                        break;
                }
        }
        if(_myNetwork._records[topIndex]._filterExpression.empty()) {
            _myNetwork._records[topIndex]._filterExpression = myExpression;
        }
        else {
            _myNetwork._records[topIndex]._filterExpression += " &amp;&amp; " + myExpression;
        }
        cout << "1: " << myExpression << endl;
        
        // --------------------------------------------------------------------------

        // Exception: U r modifying the current network, instead of copy since u r not mapping or creating wypts
        retVal.push_back(*this);
        return retVal;

    }
    else if(_myNetwork.IsEdgePresent(topWypt1, topWypt2, true)) {
        // This means add all unmapped join to the filter expression  topWypt2
        string myExpression = "";
        for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
            _removeRecID.insert(_myNetwork._records[*it]._recID);

            if(it == unMappedJoinIndexes.begin()) {
                string newEx = "(" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression = newEx;
            }
            else {
                string newEx = "&amp;&amp; (" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression += newEx;
            }
        }

	// --------------------------------------------------------------------------
	// Nike Change to support filter expression as part of predicate and not waypoint
	// First get index of mapped topWypt2
	vector<__int64_t> tmp = _myNetwork.GetRecordIndexesForWaypoint(topWypt2);
	int topIndex = -1;
	for(int iter = 0; iter < tmp.size(); iter++) {
		if(_myNetwork._records[tmp[iter]]._wyptID != NOT_MAPPED && _myNetwork._records[tmp[iter]]._qID == queryID) {
			topIndex = tmp[iter];
			break;
		}
	}
        if(_myNetwork._records[topIndex]._filterExpression.empty()) {
            _myNetwork._records[topIndex]._filterExpression = myExpression;
        }
        else {
            _myNetwork._records[topIndex]._filterExpression += " &amp;&amp; " + myExpression;
        }
	cout << "1: " << myExpression << endl;
	/*
        __int64_t grpIndex = _myNetwork.GetGroupIndex(topWypt2);
        if(_myNetwork._groups[grpIndex]._filterExpression.empty()) {
            _myNetwork._groups[grpIndex]._filterExpression = myExpression;
        }
        else {
            _myNetwork._groups[grpIndex]._filterExpression += " &amp;&amp; " + myExpression;
        }*/
	// --------------------------------------------------------------------------

        // Exception: U r modifying the current network, instead of copy since u r not mapping or creating wypts
        retVal.push_back(*this);
        return retVal;
    }
    else if(_myNetwork.IsEdgePresent(topWypt2, topWypt1, true)) {
        // This means add all unmapped join to the filter expression  topWypt1
        string myExpression = "";
        for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
            _removeRecID.insert(_myNetwork._records[*it]._recID);
            if(it == unMappedJoinIndexes.begin()) {
                string newEx = "(" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression = newEx;
            }
            else {
                string newEx = "&amp;&amp; (" + _myNetwork._records[*it]._lhsRel + "." + _myNetwork._records[*it]._lhsAtt + " = " +
                    _myNetwork._records[*it]._rhsRel + "." + _myNetwork._records[*it]._rhsAtt + ") ";
                myExpression += newEx;
            }
        }

	// --------------------------------------------------------------------------
	// Nike Change to support filter expression as part of predicate and not waypoint
	// First get index of mapped topWypt1
	vector<__int64_t> tmp = _myNetwork.GetRecordIndexesForWaypoint(topWypt1);
	int topIndex = -1;
	for(int iter = 0; iter < tmp.size(); iter++) {
		if(_myNetwork._records[tmp[iter]]._wyptID != NOT_MAPPED && _myNetwork._records[tmp[iter]]._qID == queryID) {
			topIndex = tmp[iter];
			break;
		}
	}
        if(_myNetwork._records[topIndex]._filterExpression.empty()) {
            _myNetwork._records[topIndex]._filterExpression = myExpression;
        }
        else {
            _myNetwork._records[topIndex]._filterExpression += " &amp;&amp; " + myExpression;
        }
	// cout << "22: " << myExpression << endl;
	/*
        __int64_t grpIndex = _myNetwork.GetGroupIndex(topWypt1);
        if(_myNetwork._groups[grpIndex]._filterExpression.empty()) {
            _myNetwork._groups[grpIndex]._filterExpression = myExpression;
        }
        else {
            _myNetwork._groups[grpIndex]._filterExpression += " &amp;&amp; " + myExpression;
        }
	*/
	// --------------------------------------------------------------------------
	
        // Exception: U r modifying the current network, instead of copy since u r not mapping or creating wypts
        retVal.push_back(*this);
        return retVal;
    }

    // MapJoin Logic:
    // This method doesnot modify the current NI => *this
    // It always works with copy of *this
    // topWypt1 and topWypt2 are the top waypoints. Following are possible scenarios

    // For Scenarios 1-4, see CreateNewJoinWypt
    vector<NetworkIntegrator> scenario1_4NIs = CreateNewJoinWypt(queryID, tblsInvolved, unMappedJoinIndexes, topWypt1, topWypt2);
		bool noMergeScenario=true;

    //*****************************************************************************
    // Necessary Optimization Step
    // Scenario 5: Map onto W1 if group of W1 = group formed by unMappedJoinIndexes
    // topWypt1 ---> Existing Parent wypt(W1)
    // topWypt2 -------^
    //*****************************************************************************
    {
    set<__int64_t> top1JoinParents = _myNetwork.GetSpecificWypts(topWypt1, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> top2JoinParents = _myNetwork.GetSpecificWypts(topWypt2, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> potentialW1s;
    set_intersection(   top1JoinParents.begin(), top1JoinParents.end(),   // Set 1
                        top2JoinParents.begin(), top2JoinParents.end(),   // Set 2
                        inserter(potentialW1s,potentialW1s.begin()) );        // Intersection Set
    for(set<__int64_t>::iterator it = potentialW1s.begin(); it != potentialW1s.end(); it++) {
        // Either top1 or top2 should be LHS of *it
        set<__int64_t> lhsOfPotentialW1 = _myNetwork.GetSpecificWypts(*it, GetImmediateLHSChild, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, NAPredType); // Since top1 and top2 can be Selections
        assert(lhsOfPotentialW1.size() == 1); // Only 1 LHS
        __int64_t W1WyptID = *it;
        __int64_t W1GroupIndex = _myNetwork.GetGroupIndex(W1WyptID);
        __int64_t W1GroupID = _myNetwork._groups[W1GroupIndex]._groupID;

	// Nike_India Code
	assert(W1WyptID >= 0);
	assert(W1GroupIndex  >= 0);
	assert(W1GroupID != -1);

        if(topWypt1 == *(lhsOfPotentialW1.begin()) || topWypt2 == *(lhsOfPotentialW1.begin())) {
            // Top1 or Top2 is the LHS
            cout << "\nReached Here!!!!\n";

					  // Alin: this group stuff is not necessary unless subset
					  // joins are used
					// Deactivated since it does not work properly
            set<OptimizerGroup> unionGroups = GetGroups(unMappedJoinIndexes, false);
            if(unionGroups.find(_myNetwork._groups[W1GroupIndex]) != unionGroups.end()) {
                // Mapping possible if any union of unMappedJoinIndexes groups = group of *it
                NetworkIntegrator scenario5NI = *this;
                // Since union group, no filter string required just connect the group
                for(set<__int64_t>::iterator unMappedIT = unMappedJoinIndexes.begin(); unMappedIT != unMappedJoinIndexes.end(); unMappedIT++) {
                    scenario5NI._myNetwork._records[*unMappedIT]._wyptID = W1WyptID;
                    scenario5NI._myNetwork._records[*unMappedIT]._groupID = W1GroupID;
                }
                retVal.push_back(scenario5NI);
								noMergeScenario=false;
	    }
        }
        // Note, Mapping not possible if both top1 and top2 are on RHS
    }
    }

#ifdef ALLOW_SUBSET_JOIN_GROUPING_WHILE_MAPPING
    //*****************************************************************************
    // Optional Optimization suggested by Chris
    // Scenario 6: Map onto W1 if group of W1 is subset group formed by unMappedJoinIndexes (In this case, some are put as filter conditions)
    // topWypt1 ---> Existing Parent wypt(W1)
    // topWypt2 -------^
    // Instead of union group, use powerset and add other predicates to filter string
    //*****************************************************************************
    {
    set<__int64_t> top1JoinParents = _myNetwork.GetSpecificWypts(topWypt1, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> top2JoinParents = _myNetwork.GetSpecificWypts(topWypt2, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> potentialW1s;
    set_intersection(   top1JoinParents.begin(), top1JoinParents.end(),   // Set 1
                        top2JoinParents.begin(), top2JoinParents.end(),   // Set 2
                        inserter(potentialW1s,potentialW1s.begin()) );        // Intersection Set
    for(set<__int64_t>::iterator it = potentialW1s.begin(); it != potentialW1s.end(); it++) {
        // Either top1 or top2 should be LHS of *it
        set<__int64_t> lhsOfPotentialW1 = _myNetwork.GetSpecificWypts(*it, GetImmediateLHSChild, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, NAPredType); // Since top1 and top2 can be Selections
        assert(lhsOfPotentialW1.size() == 1); // Only 1 LHS
        __int64_t W1WyptID = *it;
        __int64_t W1GroupIndex = _myNetwork.GetGroupIndex(W1WyptID);
        __int64_t W1GroupID = _myNetwork._groups[W1GroupIndex]._groupID;
        if(topWypt1 == *(lhsOfPotentialW1.begin()) || topWypt2 == *(lhsOfPotentialW1.begin())) {
            // Top1 or Top2 is the LHS
            set<OptimizerGroup> unionGroups = GetGroups(unMappedJoinIndexes, false);
            set<OptimizerGroup> powerSetGroups = GetGroups(unMappedJoinIndexes, true);
            if(unionGroups.find(_myNetwork._groups[W1GroupIndex]) == unionGroups.end() &&
                powerSetGroups.find(_myNetwork._groups[W1GroupIndex]) != powerSetGroups.end()) {
                // Donot repeat scenario 5
                // Mapping possible if any union of unMappedJoinIndexes groups = group of *it
                NetworkIntegrator scenario6NI = *this;
                // ---------------------------------------------------------------------------------
                // Filter conditions !!!
                string tempFilterString = "";
                bool tempFilterFirstTime = true;
                for(set<__int64_t>::iterator it1 = unMappedJoinIndexes.begin(); it1 != unMappedJoinIndexes.end(); it1++) {
                    assert(scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashRel.size() == scenario6NI._myNetwork._groups[W1GroupIndex]._rhsHashRel.size());
                    assert(scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashRel.size() == scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashAtt.size());
                    assert(scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashRel.size() == scenario6NI._myNetwork._groups[W1GroupIndex]._rhsHashAtt.size());
                    bool isHashPred = false;
                    string leftSide = scenario6NI._myNetwork._records[*it1]._lhsRel + "." + scenario6NI._myNetwork._records[*it1]._lhsAtt;
                    string rightSide = scenario6NI._myNetwork._records[*it1]._rhsRel + "." + scenario6NI._myNetwork._records[*it1]._rhsAtt;
                    for(__int64_t j = 0; j < scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashRel; j++) {
                        string lhsHash = scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashRel[j] + "." + scenario6NI._myNetwork._groups[W1GroupIndex]._lhsHashAtt[j];
                        string rhsHash = scenario6NI._myNetwork._groups[W1GroupIndex]._rhsHashRel[j] + "." + scenario6NI._myNetwork._groups[W1GroupIndex]._rhsHashAtt[j];

                        if( (lhsHash.compare(leftSide) == 0 &&  rhsHash.compare(rightSide) == 0) ||
                            (lhsHash.compare(rightSide) == 0 &&  rhsHash.compare(leftSide) == 0) ) {
                            isHashPred = true;
                            break;
                        }
                    }
                    if(!isHashPred && tempFilterFirstTime) {
                        tempFilterString = "(" + leftSide + " = " + rightSide + ")";
                        tempFilterFirstTime = false;
                    }
                    else if(!isHashPred && !tempFilterFirstTime) {
                        tempFilterString += "&& (" + leftSide + " = " + rightSide + ")";
                    }
                }

                // ---------------------------------------------------------------------------------
                for(set<__int64_t>::iterator unMappedIT = unMappedJoinIndexes.begin(); unMappedIT != unMappedJoinIndexes.end(); unMappedIT++) {
                    scenario6NI._myNetwork._records[*unMappedIT]._wyptID = W1WyptID;
                    scenario6NI._myNetwork._records[*unMappedIT]._groupID = W1GroupID;
                }
		assert(false); // Put filter expression in predicate rather than group
                if(scenario6NI._myNetwork._groups[W1GroupIndex]._filterExpression.empty())
                    scenario6NI._myNetwork._groups[W1GroupIndex]._filterExpression = tempFilterString;
                else
                    scenario6NI._myNetwork._groups[W1GroupIndex]._filterExpression += " && " + tempFilterString;

                retVal.push_back(scenario6NI);
								noMergeScenario=false;
            }
        }

        // Note, Mapping not possible if both top1 and top2 are on RHS
    }
    }
#endif

#ifdef ALLOW_JOIN_BYPASSING
    //*****************************************************************************
    // Scenario 7: Map onto W1 with Scenario3,4 if bypassing enabled
    // topWypt1 ---> Bypass wypts ----> Existing Parent wypt(W1)
    // topWypt2 -------^
    //*****************************************************************************

// Support Bypass
/*
{
    set<__int64_t> top1JoinParents = _myNetwork.GetSpecificWypts(topWypt1, GetAllRecursiveParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> top2JoinParents = _myNetwork.GetSpecificWypts(topWypt2, GetAllRecursiveParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> potentialW1s;
    set_intersection(   top1JoinParents.begin(), top1JoinParents.end(),   
                        top2JoinParents.begin(), top2JoinParents.end(),   
                        inserter(potentialW1s,potentialW1s.begin()) );    
    for(set<__int64_t>::iterator it = potentialW1s.begin(); it != potentialW1s.end(); it++) {
        
        set<__int64_t> lhsOfPotentialW1 = _myNetwork.GetSpecificWypts(*it, GetRecursiveLHSChildren, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join); 
        set<__int64_t> rhsOfPotentialW1 = _myNetwork.GetSpecificWypts(*it, GetImmediateRHSChildren, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join); 
	set<__int64_t> lhsOfRhs;
	for(set<__int64_t>::iterator rhsIter = rhsOfPotentialW1.begin(); rhsIter != rhsOfPotentialW1.begin(); rhsIter++) {
		set<__int64_t> tmpLHS = _myNetwork.GetSpecificWypts(*rhsIter, GetRecursiveLHSChildren, AlwayTrue,
		    IGNORE_NAME, IGNORE_NAME, -1, Join); 
		lhsOfRhs.insert(tmpLHS.begin(), tmpLHS.end());
	}

	set<__int64_t> bypassLHS;
	set<__int64_t> bypassRHS;


	bool bypassScenario1 = false;
	bool bypassScenario2 = false;
	if(lhsOfPotentialW1.find(topWypt1) != lhsOfPotentialW1.end() && lhsOfPotentialW1.find(topWypt2) != lhsOfPotentialW1.end()){
		cout << "Both top1 and top2 cannot be on LHS stem\n";
		assert(false);
	}
	else if(lhsOfPotentialW1.find(topWypt1) != lhsOfPotentialW1.end() && lhsOfRhs.find(topWypt2) != lhsOfRhs.end()) {
		bypassScenario1 = true;
		set_intersection( lhsOfPotentialW1.begin(), lhsOfPotentialW1.end(),
                        top1JoinParents.begin(), top1JoinParents.end(),
                        inserter(bypassLHS,bypassLHS.begin()) );
		set_intersection( lhsOfRhs.begin(), lhsOfRhs.end(),
                        top2JoinParents.begin(), top2JoinParents.end(),
                        inserter(bypassRHS,bypassRHS.begin()) );

	}
	else if(lhsOfPotentialW1.find(topWypt2) != lhsOfPotentialW1.end() && lhsOfRhs.find(topWypt2) != lhsOfRhs.end()) {
		bypassScenario2 = true;
		set_intersection( lhsOfPotentialW1.begin(), lhsOfPotentialW1.end(),
                        top2JoinParents.begin(), top2JoinParents.end(),
                        inserter(bypassLHS,bypassLHS.begin()) );
		set_intersection( lhsOfRhs.begin(), lhsOfRhs.end(),
                        top1JoinParents.begin(), top1JoinParents.end(),
                        inserter(bypassRHS,bypassRHS.begin()) );
	}
	else {
		cout << "LHSOfPotential size: "  << lhsOfPotentialW1.size() << "lhsOfRhs size: " << lhsOfRhs.size() << endl;
		continue;
	}

	cout << "Num of possible bypass LHS:" << bypassLHS.size() << " and " << bypassRHS.size() << endl;
	assert(false);

	if(bypassLHS.size() > BYPASS_LIMIT || bypassRHS.size() > BYPASS_LIMIT)
		continue;

	// cout << "TODO: Also only bypass if none of the waypoints in either bypassLHS or bypassRHS are eligible for any of the remaining tbls\n";

        __int64_t W1WyptID = *it;
        __int64_t W1GroupIndex = _myNetwork.GetGroupIndex(W1WyptID);
        __int64_t W1GroupID = _myNetwork._groups[W1GroupIndex]._groupID;

        
        assert(W1WyptID >= 0);
        assert(W1GroupIndex  >= 0);
        assert(W1GroupID != -1);

        if(topWypt1 == *(lhsOfPotentialW1.begin()) || topWypt2 == *(lhsOfPotentialW1.begin())) {

            set<OptimizerGroup> unionGroups = GetGroups(unMappedJoinIndexes, false);
            if(unionGroups.find(_myNetwork._groups[W1GroupIndex]) != unionGroups.end()) {
        
                NetworkIntegrator scenario7NI = *this;
        
                for(set<__int64_t>::iterator unMappedIT = unMappedJoinIndexes.begin(); unMappedIT != unMappedJoinIndexes.end(); unMappedIT++) {
                    scenario7NI._myNetwork._records[*unMappedIT]._wyptID = W1WyptID;
                    scenario7NI._myNetwork._records[*unMappedIT]._groupID = W1GroupID;
                }
		// Also create bypass predicates for bypassLHS and bypassRHS
                for(set<__int64_t>::iterator bypassIT = bypassLHS.begin(); bypassIT != bypassLHS.end(); bypassIT++) {
			OptimizerRecord newRec(scenario7NI._myNetwork.GetNewID("RECORD"));
			newRec._myType = Join;
			newRec._myOp = LT;
			newRec._parseString = "(bypass)";
			newRec._lhsRel = IGNORE_NAME;
			newRec._rhsRel = IGNORE_NAME;
			newRec._lhsAtt = IGNORE_NAME;
			newRec._rhsAtt = IGNORE_NAME;
			newRec._qID = queryID;
			newRec._wyptID = *bypassIT;
			newRec._groupID = NO_GROUP_OTHER;
			scenario7NI._myNetwork._records.push_back(newRec);
		}
                for(set<__int64_t>::iterator bypassIT = bypassRHS.begin(); bypassIT != bypassRHS.end(); bypassIT++) {
			OptimizerRecord newRec(scenario7NI._myNetwork.GetNewID("RECORD"));
			newRec._myType = Join;
			newRec._myOp = LT;
			newRec._parseString = "(bypass)";
			newRec._lhsRel = IGNORE_NAME;
			newRec._rhsRel = IGNORE_NAME;
			newRec._lhsAtt = IGNORE_NAME;
			newRec._rhsAtt = IGNORE_NAME;
			newRec._qID = queryID;
			newRec._wyptID = *bypassIT;
			newRec._groupID = NO_GROUP_OTHER;
			scenario7NI._myNetwork._records.push_back(newRec);
		}
	 	cout << "TODO: Also only bypass if none of the waypoints in either bypassLHS or bypassRHS are eligible for any of the remaining tbls\n";
		assert(false); // Feature check
                retVal.push_back(scenario7NI);
                noMergeScenario=false;
            }
        }        
    }
}
*/
// Support multiple RHS
{
    set<__int64_t> top1JoinParents = _myNetwork.GetSpecificWypts(topWypt1, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> top2JoinParents = _myNetwork.GetSpecificWypts(topWypt2, GetAllImmediateParents, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, Join);
    set<__int64_t> potentialW1s;
    set_union(   top1JoinParents.begin(), top1JoinParents.end(),
                 top2JoinParents.begin(), top2JoinParents.end(),
                 inserter(potentialW1s,potentialW1s.begin()) );
    set<__int64_t> potentialIntersection;
    set_intersection(   top1JoinParents.begin(), top1JoinParents.end(),
			 top2JoinParents.begin(), top2JoinParents.end(),
			 inserter(potentialIntersection,potentialIntersection.begin()) );
    for(set<__int64_t>::iterator it = potentialW1s.begin(); it != potentialW1s.end() && potentialIntersection.size() == 0; it++) {
	bool applicable = true;

        set<__int64_t> lhsOfPotentialW1 = _myNetwork.GetSpecificWypts(*it, GetImmediateLHSChild, AlwayTrue,
            IGNORE_NAME, IGNORE_NAME, -1, NAPredType); // Since top1 and top2 can be Selections
        assert(lhsOfPotentialW1.size() == 1); // Only 1 LHS
        __int64_t W1WyptID = *it;
        __int64_t W1GroupIndex = _myNetwork.GetGroupIndex(W1WyptID);
        __int64_t W1GroupID = _myNetwork._groups[W1GroupIndex]._groupID;


        assert(W1WyptID >= 0);
        assert(W1GroupIndex  >= 0);
        assert(W1GroupID != -1);

        if(topWypt1 == *(lhsOfPotentialW1.begin()) || topWypt2 == *(lhsOfPotentialW1.begin())) {


            set<OptimizerGroup> unionGroups = GetGroups(unMappedJoinIndexes, false);
            if(unionGroups.find(_myNetwork._groups[W1GroupIndex]) != unionGroups.end()) {

                NetworkIntegrator scenario8NI = *this;

                for(set<__int64_t>::iterator unMappedIT = unMappedJoinIndexes.begin(); unMappedIT != unMappedJoinIndexes.end(); unMappedIT++) {
                    scenario8NI._myNetwork._records[*unMappedIT]._wyptID = W1WyptID;
                    scenario8NI._myNetwork._records[*unMappedIT]._groupID = W1GroupID;
                }
		// Additional thing: Add additional RHS edge
        	if(topWypt1 == *(lhsOfPotentialW1.begin()) ) {
			// Also the rhs has to be topmost parent
    			set<__int64_t> myTopJoinParents = scenario8NI._myNetwork.GetSpecificWypts(topWypt2, GetAllImmediateParents, TrueIfQIDsSame,
			    IGNORE_NAME, IGNORE_NAME, queryID, Join);
			if(myTopJoinParents.size() != 0)
				applicable = false;

			// Add edge from topWypt2 to W1WyptID
			if(scenario8NI._myNetwork.IsEdgePresent(topWypt2, W1WyptID, false)) {
				OptimizerEdge rhsEdge(scenario8NI._myNetwork.GetNewID("EDGE")); 
				rhsEdge._startWyptID = topWypt2;
				rhsEdge._endWyptID = W1WyptID;
				scenario8NI._myNetwork._edges.push_back(rhsEdge);
				// cout << "Adding edge for scenario 8: " << topWypt2 << " -> " << W1WyptID << endl; 
			}
			else
				applicable = false;
		}
        	else if(topWypt2 == *(lhsOfPotentialW1.begin()) ) {
			// Add edge from topWypt1 to W1WyptID
    			set<__int64_t> myTopJoinParents = scenario8NI._myNetwork.GetSpecificWypts(topWypt1, GetAllImmediateParents, TrueIfQIDsSame,
			    IGNORE_NAME, IGNORE_NAME, queryID, Join);
			// assert(myTopJoinParents.size() == 0);
			if(myTopJoinParents.size() != 0)
				applicable = false;
			if(scenario8NI._myNetwork.IsEdgePresent(topWypt1, W1WyptID, false)) {
				OptimizerEdge rhsEdge(_myNetwork.GetNewID("EDGE")); 
				rhsEdge._startWyptID = topWypt1;
				rhsEdge._endWyptID = W1WyptID;
				scenario8NI._myNetwork._edges.push_back(rhsEdge);
				// cout << "Adding edge for scenario 8: " << topWypt1 << " -> " << W1WyptID << endl; 
			}
			else
				applicable = false;
		}
		// assert(false);
		if(applicable) {
			cout << "!!!! scenario 8 applicable - This is not a bug but just a check to test we encounter any such cases. Inform Niketan \n";
                	retVal.push_back(scenario8NI);
		}
                noMergeScenario=false;
            }
        }
    }
}

    cout << "TODO: Bypassing in join waypoints is not allowed in current optimizer\n";
    assert(false);
#endif

		// Alin: We allow a new join waypoint to be created only if a
		// better scenario could not be found
		
//		if (noMergeScenario){
			for(__int64_t i = 0; i < scenario1_4NIs.size(); i++) {
        retVal.push_back(scenario1_4NIs[i]);
			}
//		}

    return retVal;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ------------------------- Main Optimization Logic --------------------------------------------

vector<NetworkIntegrator> NetworkIntegrator :: Enumerate(__int64_t queryID)
{
    vector<NetworkIntegrator> retVal;

    // First Map all tablescans
    if(_stateOfNI == NOT_MAPPED) {
        MapTableScans(queryID);
        MapSelections(queryID);
        _stateOfNI = ALL_TABLESCAN_SELECTION_MAPPED;
        retVal.push_back(*this);
    }
    else if(_stateOfNI == ALL_TABLESCAN_SELECTION_MAPPED) {
        // Check whether all the joins are mapped
        set<__int64_t> unMappedJoinIndexes = _myNetwork.GetUnMappedRecordIndexes(TrueIfQIDsSame, IGNORE_NAME, IGNORE_NAME, queryID, Join);
        if(unMappedJoinIndexes.size() == 0) {
            _stateOfNI = ALL_JOIN_MAPPED;
            retVal.push_back(*this);
        }
        else {
            // Map Join
            // ------------------------------------------------------------------------------
            // Now create map of <Tbls, recIndexes>
					map< pair< string, string >, set<__int64_t> > joinMap; 
																														
            for(set<__int64_t>::iterator it = unMappedJoinIndexes.begin(); it != unMappedJoinIndexes.end(); it++) {
                pair<string, string> tbls = GetTablesInvolved(*it);
                map< pair< string, string >, set<__int64_t> >::iterator foundIT = joinMap.find(tbls);
                if(foundIT == joinMap.end()) {
                    // No entry present create 1
                    set<__int64_t> temp;
                    temp.insert(*it);
                    joinMap.insert(pair< pair< string, string >, set<__int64_t> >(tbls, temp));
                }
                else {
                    // Replace the entry after appending current recIndex i.e *it
                    set<__int64_t> temp = foundIT -> second;
                    joinMap.erase(foundIT);
                    temp.insert(*it);
                    joinMap.insert(pair< pair< string, string >, set<__int64_t> >(tbls, temp));
                }
            }
            // ------------------------------------------------------------------------------
            for(map< pair< string, string >, set<__int64_t> >::iterator it = joinMap.begin();
                it != joinMap.end(); it++) {
                // Since MapJoin doesnot modify the current NI,
                // this logic maps tries to map every group independently => Optimizes for all joins irrespective of the order :)
                vector<NetworkIntegrator> newNIs = MapJoin(queryID, it-> first, it -> second);
                for(vector<NetworkIntegrator>::iterator newIT = newNIs.begin(); newIT != newNIs.end(); newIT++) {
                    retVal.push_back(*newIT);
                }
            }

        }
    }
    else if(_stateOfNI == ALL_JOIN_MAPPED) {
        cout << "Error: Do not call Enumerate after mapping all the joins\n";
        assert(false);
    }
    else {
        cout << "Error: Invalid Optimizer State in Enumerate\n";
        assert(false);
    }

    // Remove all the records from all the NIs in retVal that are flagged for removal
    for(__int64_t i = 0; i < retVal.size(); i++) {
        for(vector<OptimizerRecord>::iterator it = retVal[i]._myNetwork._records.begin();
            it != retVal[i]._myNetwork._records.end(); it++) {
            if(_removeRecID.find(it -> _recID) != _removeRecID.end()) {
                retVal[i]._myNetwork._records.erase(it);
            }
        }
    }

    return retVal;
}

void Optimizer :: LookAheadSearch(__int64_t queryID)
{
	while(!_nextNI.IsIntegrationDone(queryID)) {

		//Is Integration not done because of Top and Cartesian products ??
		if(_nextNI._stateOfNI == ALL_JOIN_MAPPED) {
			_nextNI.MapTopAndCartesianProducts(queryID);
			continue;
		}

		// First enumerate
		vector<NetworkIntegrator> lni = _nextNI.Enumerate(queryID);

		if(lni.size() == 0) {
			if(!_nextNI.IsIntegrationDone(queryID)) {
				if(_nextNI._stateOfNI == ALL_JOIN_MAPPED) {
					_nextNI.MapTopAndCartesianProducts(queryID);
					continue;
				}

				cout << "Error: In LookAhead Search, Integration not done but enumeration returns no nis\n";
				assert(false);
			}
			else {
				lni.push_back(_nextNI);
			}

		}
		else if(lni.size() == 1) {
		    // If only one NI returned => No costing necessary
		    _nextNI = lni[0];
		    continue;
		}

		// Do look ahead search to find which ni
		vector<PathNetworkCost> costOfLNI;
		for(__int64_t j = 0; j < lni.size(); j++){
			vector<NetworkIntegrator> temp1;
			vector<NetworkIntegrator> temp2;

			temp1.push_back(lni[j]);

			for(__int64_t i = 0; i < NUMBER_LOOKAHEADS; i++) {
				// Pull from temp1 and push into temp2 after enumeration
				for(__int64_t k = 0; k < temp1.size(); k++) {
					// Pull from temp1 and enumerate
					vector<NetworkIntegrator> temp3 = temp1[k].Enumerate(queryID);

					for(__int64_t m = 0; m < temp3.size(); m++) {
						// push into temp2
						temp2.push_back(temp3[m]);
					}
				}

				// Now temp1 <--- temp2 only if temp2's size is != 0
				if(temp2.size() == 0) {
					break; // Break LookAhead loop since cannot enumerate further
				}
				else {
					temp1.clear();
					for(__int64_t k = 0; k < temp2.size(); k++) {
						temp1.push_back(temp2[k]);
					}
				}
			} // i for loop

			assert(temp1.size() >= 1);

			// Insert the cost of lni by chosing smallest among temp1
			PathNetworkCost leastIntermediateCost = temp1[0]._myCoster.GetCost();

			for(__int64_t i = 1; i < temp1.size(); i++) {
                		PathNetworkCost tempCost = temp1[i]._myCoster.GetCost();
				assert(tempCost._dataFlowCost >= 0);
				if(tempCost < leastIntermediateCost) {
					leastIntermediateCost = tempCost;
				}
			}

			costOfLNI.push_back(leastIntermediateCost);
		} // j for loop

		assert(lni.size() == costOfLNI.size());

		// Then find least cost

		__int64_t leastCostIndex = 0;
		PathNetworkCost leastCost = costOfLNI[0];

		for(__int64_t i = 1; i < lni.size(); i++) {
			if(costOfLNI[i] < leastCost) {
				leastCost = costOfLNI[i];
				leastCostIndex = i;
			}
		}

		// Use ni with least cost as next ni
		_nextNI = lni[leastCostIndex];
	}
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void Optimizer :: LoadConfiguration(){
  // Opimizer only supporting these 2 options
  _myConfig._costerMode = "DataFlow";
  _myConfig._searchType = "LookAhead";
}

void NetworkIntegrator :: InsertPredicate(PredicateType myType, Operator myOp, string parseString, string lhsRel, string rhsRel, string lhsAtt, string rhsAtt, __int64_t qID) {
  	string newParseString;
	if(myType == TableScan)
		newParseString = parseString;
	else
    newParseString = "(" + parseString + ")";

    OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
	newRec._myType = myType;
	newRec._myOp = myOp;
	newRec._parseString = newParseString;
	newRec._lhsRel = lhsRel;
	newRec._rhsRel = rhsRel;
	newRec._lhsAtt = lhsAtt;
	newRec._rhsAtt = rhsAtt;
	newRec._qID = qID;
	newRec._wyptID = NOT_MAPPED;
	newRec._groupID = NO_GROUP_OTHER;
	_myNetwork._records.push_back(newRec);
}

void NetworkIntegrator :: InsertTopPredicate(vector<TopAggregate> aggregatePreds, vector<TopDisjunction> disjunctionPreds, TopOutput outputPred, TopGroupBy groupByPred, __int64_t qID) {
    OptimizerRecord newRec(_myNetwork.GetNewID("RECORD"));
	newRec._myType = Top;
	newRec._myOp = NotApplicable;
	newRec._parseString = "";
	newRec._lhsRel = IGNORE_NAME;
	newRec._rhsRel = IGNORE_NAME;
	newRec._lhsAtt = IGNORE_NAME;
	newRec._rhsAtt = IGNORE_NAME;
	newRec._qID = qID;
	newRec._wyptID = NOT_MAPPED;
	newRec._groupID = NO_GROUP_OTHER;
	// Now Insert Top Predicate entries
	for(__int64_t i = 0; i < aggregatePreds.size(); i++) {
		newRec._aggregatePreds.push_back(aggregatePreds[i]);
	}

	for(__int64_t i = 0; i < disjunctionPreds.size(); i++) {
		disjunctionPreds[i].expression = "(" + disjunctionPreds[i].expression + ")";
		newRec._disjunctionPreds.push_back(disjunctionPreds[i]);
	}

	newRec._outputPred = outputPred;
	newRec._groupByPred = groupByPred;

	_myNetwork._records.push_back(newRec);

}
