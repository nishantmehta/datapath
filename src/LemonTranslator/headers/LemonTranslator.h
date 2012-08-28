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
#ifndef _LEMON_TRANSLATOR_H
#define _LEMON_TRANSLATOR_H

#include "Catalog.h"
#include "ID.h"
#include "QueryExit.h"
#include "DataPathGraph.h"
#include "SymbolicWaypointConfig.h"
#include "WayPointConfigureData.h"
#include "ScannerConfig.h"
#include <lemon/list_graph.h>
#include <set>

using namespace lemon;
class LT_Waypoint;

/** This is the interface to the Lemon based Translator */
class LemonTranslator {
public:

#include "LemonTranslatorPrivate.h"

public:
    enum JoinType { Join_EQ, Join_IN, Join_NOTIN };

    LemonTranslator(void);

    // return a text error message
    string GetErrorMessage();

    // remove all info about a query from the system
    // the list needs to be made available to the Scanners as well
    bool DeleteQuery(QueryID query);

    bool AddQuery(string qName, QueryID query);

    /************ Adding new waypoints ***************/
    // Check error message and return status of all functions returning bool below
    // It is important not to corrupt graph by not ignoring error status
    bool AddScannerWP(WayPointID scanner, string relName, SlotContainer& attributes /** atrributes that scanner can read */);

    bool AddTextLoaderWP(WayPointID loaderWP,
            SlotContainer& attributes, /* atrributes that the loader can read */
            char separator, /* the separator used in the
                               text */
            SlotContainer& attributesInOrder, /** attributs to
                                                read from file, in order. if SlotID=Invalid, the
                                                slot is skipped */
            string pattern,
            int count
            );

    bool AddSelectionWP(WayPointID selWP);
    bool AddJoinWP(WayPointID joinWP, SlotContainer& LHS_att, string defs);
    bool AddAggregateWP(WayPointID aggWP);
    bool AddPrintWP(WayPointID printWP);
    bool AddGLAWP(WayPointID glaWP);
    bool AddGLALargeWP(WayPointID glaWP);

    // Adding Edges to the graph (terminating or non-terminating)
    // The edge direction should be provided correctly, always bottom to top
    // (bottom, top)
    bool AddEdge(WayPointID start, WayPointID end);
    bool AddTerminatingEdge(WayPointID start, WayPointID end);

    /******* Adding information for queries (per query) ****************/

    // These functions might not be apropriate for the type
    // of waypoint defined earlier. If that is the case, false is
    // returned and error is produced

    // Selection
    bool AddFilter(WayPointID wp, QueryID query, SlotContainer& atts,
                                 string expr /*filter cond*/, string initializer = "",
                                 string defs = "");

    // Selection. Need a list of these
    bool AddSynthesized(WayPointID wp, QueryID query, SlotID attribute /** this is the synthesiezd attribute*/, SlotContainer& atts, string expr, string initializer, string defs);
    // Selection
//    bool AddSynthesized(WayPointID wp, QueryID query,
//                                            SlotID attribute, /** this is the synthesiezd attribute*/
//                                            SlotContainer& atts, /** attributes in expression */
//                                            string expr
//                                            );


    // Aggregate. Need a list of these
    bool AddAggregate(WayPointID wp, QueryID query,
                                        SlotID aggAttrib, /* attribute corresponding to aggregate */
                                        string aggregateType, /* type, just pass along */
                                        SlotContainer& atts, string expr, string initializer = "",
                                        string defs = "");

    //GLA, one per query basis
    bool AddGLA(WayPointID wp, QueryID query,
                SlotContainer& resultAtts, /*list of attributes produced as the result */
                string glaName, /*name of the GLA eg. AverageGLA, CountGLA, myGLA etc */
                            string glaDef, /* definitions to inject at top of file */
                string constructorExp, /*expression in GLA constructor */
                SlotContainer& atts, string expr, string initializer = "");

    bool AddGLALarge(WayPointID wp, QueryID query,
             SlotContainer& resultAtts, /*list of attributes produced as the result */
             string glaName, /*name of the GLA eg. AverageGLA, CountGLA, myGLA etc */
             string glaDef, /* definitions to inject at top of file */
             string constructorExp, /*expression in GLA constructor */
             SlotContainer& atts, string expr, string initializer = "");

    // Selection, Join. Queries added one by one
    bool AddBypass(WayPointID wp, QueryID query);

    // Join
    bool AddJoin(WayPointID wp, QueryID query, SlotContainer& RHS_atts /* right hand side attributes */,
         JoinType type, string defs);

    // Print. Need a list of these
    bool AddPrint(WayPointID wp, QueryID query, SlotContainer& atts,
                  string expr /* what to print */, string initializer="", string name="", string type="",
                  string file="", string defs = "");

    bool AddWriter(WayPointID wpID, QueryID query);

    /****Interface for the rest of the system***************/

    void ClearAllDataStructure();

    // All the queries in the argument should be ready to go
    // and the translator should produce the instructions ..
    // dir is the directory where the results are placed
    bool Run(QueryIDSet whatQueries);

    // write M4 files for all nodes
    // Params:
    //   dir: the directory where the code is
    //   RETURNS: objects that were generated
    string WriteM4Files(string dir);

    // Generate Messages for all nodes
    bool GenerateMessages();

    // compile the code
    // Params:
    //   dir: the directory where the code is
    //   objects: list of objects that need to be included
    //   RETURNS: name of the library created
    string CompileCode(string dir, string objects);

    // Function to plot the query plan graph in a dot file so it can be plotted
    void PrintDOT(ostream& out);

    // function to compute the configuration message for the rest of the system
    // newQueries will contain the new queries (since last call)
    bool GetConfig(std::string, QueryExitContainer& newQueries, DataPathGraph&, WayPointConfigurationList&);

    void PopulateWayPointConfigurationData(WayPointConfigurationList& myConfigs);

    void FillTypeMap(std::map<WayPointID, WaypointType>& typeMap);

    void WriteM4FileCleaner (ostream& out);

    void GetAccumulatedLHSRHS(set<SlotID>& LHS, set<SlotID>& RHS, QueryIDSet& queries);
};


#endif // _LEMON_TRANSLATOR_H
