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
#include "AttributeManager.h"
#include "LemonTranslator.h"
#include <lemon/bfs.h>
#include <lemon/core.h>
#include <lemon/connectivity.h>
#include "LT_Waypoint.h"
#include "LT_Scanner.h"
#include "LT_Selection.h"
#include "LT_Join.h"
#include "LT_Aggregate.h"
#include "LT_Print.h"
#include "LT_TextLoader.h"
#include "LT_GLA.h"
#include "LT_GLA_large.h"
#include "AttributeManager.h"
#include "QueryManager.h"
#include "Errors.h"
#include "Debug.h"
#include <fstream>
#include <stdlib.h>
#include "ExternalCommands.h"
#include "DataTypeManager.h"

extern HashTable centralHashTable;

using namespace lemon;

static string GetAllAttrAsString(SlotContainer& atts) {
    AttributeManager &am = AttributeManager::GetAttributeManager();

    int len = atts.Length();
    atts.MoveToStart();
    string rez;
    while (!atts.AtEnd()){
        SlotID slot = atts.Current();
        rez += " ";
        rez += am.GetAttributeName(slot);
        atts.Advance();
    }
    return rez;
    //cout << "\nLHS: " << (l.getIDAsString()).c_str();
}

static string GetAllAttrAsString(SlotID& atts) {
    string rez;
    IDInfo l;
    (const_cast<SlotID&>(atts)).getInfo(l);
    rez += (l.getName()).c_str();
    return rez;
}
void LemonTranslator::PrintDOT(ostream& out){

    // preamble
    out <<  "\n\t/* Graph */\n";
    out <<  "digraph G {\n\tcompound=true;\n\trankstep=1.25;\n";
    out << "\tlabel=\"LEMON Data Path Network\";\n\tnode[shape=ellipse,fontsize=12,fontcolor=black,color=grey];\n";
    out << "\tbgcolor=white;\n\tedge [arrowsize=1,fontsize=10];\n";

    // nodes
    out << "\n\t/* Nodes */\n";
    {
        Bfs<ListDigraph> bfs(graph);
        bfs.init();
        for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
            if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                string wName = wp->GetWPName();

                out << "\tsubgraph cluster_" << wName << " {label=\""
                    << wName << "\"; labelloc=\"b\";};\n";
            }
            if (!bfs.reached(n)) {
                bfs.addSource(n);
                bfs.start();
            }
        }
    }
    // edges
    out << "\n\t/* Relationships */\n";
    // terminating edge

    // Traverse full graph step by step (lemon BFS algo) and fill the info
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
            LT_Waypoint* wp = nodeToWaypointData[n];
            string nID = wp->GetWPName();

            // Now find all the outlinks for the node
            for (ListDigraph::OutArcIt arc(graph, n); arc != INVALID; ++arc) {
                ListDigraph::Node next = graph.target(arc);
                bool isTerminating = terminatingArcMap[arc];
                if (next != topNode && next != bottomNode) {
                    LT_Waypoint* nextWP = nodeToWaypointData[next];
                    string nTopGuyID = nextWP->GetWPName();

                    // Get the top guy ID
                    WayPointID topGuyID = nextWP->GetId();

                    QueryExitContainer queryExitsFlowThrough;
                    QueryExitContainer queryExitsEnding;
                    nextWP->GetQueryExits(queryExitsFlowThrough, queryExitsEnding);

                    if (!isTerminating) {
                        string QEs;
                        queryExitsFlowThrough.MoveToStart ();
                        while (queryExitsFlowThrough.RightLength ()) {
                            QueryExit qe = queryExitsFlowThrough.Current ();
                            QEs += qe.GetStr();
                            queryExitsFlowThrough.Advance ();
                        }
                        if (QEs == "")
                            QEs = "error";
                        out <<     "\tedge [arrowsize=1,color=blue,label=\"" << QEs << "\"" << "]\t "
                            << nID << "->" << nTopGuyID << "\n";
                    } else {
                        string QEs;
                        queryExitsEnding.MoveToStart ();
                        while (queryExitsEnding.RightLength ()) {
                            QueryExit qe = queryExitsEnding.Current ();
                            QEs += qe.GetStr();
                            queryExitsEnding.Advance ();
                        }
                        if (QEs == "")
                            QEs = "error";
                        out <<     "\tedge [arrowsize=1,color=red,label=\"" << QEs << "\"" << "]\t "
                            << nID << "->" << nTopGuyID << "\n";
                    }
                }
            }

        }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }

    // finish
    out << "\n}\n";

}

// We create top and bottom nodes for the graph, this will serve as starting and
// termination points
// we also create the clearner waypoint
LemonTranslator::LemonTranslator():terminatingArcMap(graph), cleanerID("Cleaner") {
    topNode = graph.addNode();
    bottomNode = graph.addNode();
    isNewRun = false;
}

static void ConvertToSTLSet (SlotContainer& atts, set<SlotID>& attr) {

    int len = atts.Length();
    attr.clear();
    atts.MoveToStart();
    while (!atts.AtEnd()){
        SlotID slot = atts.Current();
        attr.insert(slot);
        atts.Advance();
    }
}

// Common processing function
// This will be called from each add waypoint function to add graph node
bool LemonTranslator::AddGraphNode(WayPointID WPID, WaypointType type, LT_Waypoint*& WP) {

    ListDigraph::Node u = graph.addNode();
    IDToNode[WPID] = u;

    if (type == PrintWaypoint){
        graph.addArc(u, topNode);
    } else if (type == ScannerWaypoint){
        graph.addArc(bottomNode, u);
    } else if (type == TextLoaderWaypoint){
        graph.addArc(bottomNode, u);
    }

    // fill our node to waypoint map
    nodeToWaypointData[u] = WP;
    return true;
}

// Common processing function
// This will be called from each add conditions function, like AddFilter
bool LemonTranslator::GetWaypointAttr(WayPointID WPID, SlotContainer& atts,
        set<SlotID>& attr, LT_Waypoint*& WP) {

    map<WayPointID, ListDigraph::Node>::const_iterator it = IDToNode.find(WPID);
    if (it == IDToNode.end()) {
        cout << "No graph node found for specified waypoint ID";
        return false;
    }
    WP = nodeToWaypointData[it->second];
    if (WP == NULL) {
        cout << "No waypoint details found for given node";
        return false;
    }
    attr.clear();
    atts.MoveToStart();
    while (!atts.AtEnd()){
        SlotID slot = atts.Current();
        attr.insert(slot);
        atts.Advance();
    }
    return true;
}

void LemonTranslator::ClearAllDataStructure()
{
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                wp->ClearAllDataStructure();
            }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }
}

string LemonTranslator::GetErrorMessage()
{
    //return error;
}

bool LemonTranslator::DeleteQuery(QueryID qID)
{

    deleteQueries.Union(qID); // remember the deleted queries

    // Traverse full graph step by step (lemon BFS algo) and delete query and related info
    // in each node if found.
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                wp->DeleteQuery(qID);
            }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }
}

bool LemonTranslator::AddScannerWP(WayPointID scannerID, string relName, SlotContainer& atts)
{
    PDEBUG("LemonTranslator::AddScannerWP(WayPointID scannerID = %s, SlotContainer& atts = %s)", scannerID.getName().c_str(), (GetAllAttrAsString(atts)).c_str());
    FATALIF(!scannerID.IsValid(), "Invalid WaypointID received in AddScannerWP");
    set<SlotID> attr;
    ConvertToSTLSet (atts, attr);
    LT_Waypoint* WP = new LT_Scanner(scannerID, relName, attr);
    return AddGraphNode(scannerID, ScannerWaypoint, WP);
}


bool LemonTranslator::AddTextLoaderWP(WayPointID loaderID, SlotContainer& atts,
        char separator, SlotContainer& atts2, string pattern, int count)
{
    PDEBUG("LemonTranslator::AddTextLoaderWP(WayPointID loaderID = %s, SlotContainer& atts = %s, char separator = %c,\
         SlotContainer& atts2 = %s, string pattern = %s, int count = %d", loaderID.getName().c_str(),
         (GetAllAttrAsString(atts)).c_str(), separator, (GetAllAttrAsString(atts2)).c_str(), pattern.c_str(), count);
    FATALIF(!loaderID.IsValid(), "Invalid WaypointID received in AddTextLoaderWP");
    set<SlotID> attr;
    ConvertToSTLSet (atts, attr);
    LT_Waypoint* WP = new LT_TextLoader(loaderID, attr, separator, atts2, pattern, count);
    return AddGraphNode(loaderID, TextLoaderWaypoint, WP);
}

bool LemonTranslator::AddSelectionWP(WayPointID selWPID)
{
    PDEBUG("LemonTranslator::AddSelectionWP(WayPointID selWPID = %s)", selWPID.getName().c_str());
    FATALIF(!selWPID.IsValid(), "Invalid WaypointID received in AddSelectionWP");
    LT_Waypoint* WP = new LT_Selection(selWPID);
    return AddGraphNode(selWPID, SelectionWaypoint, WP);
}

bool LemonTranslator::AddJoinWP(WayPointID joinWPID, SlotContainer& LHS_att, string defs)
{
    PDEBUG("LemonTranslator::AddJoinWP(WayPointID joinWPID = %s, SlotContainer& LHS_att = %s)", joinWPID.getName().c_str(), (GetAllAttrAsString(LHS_att)).c_str());
    FATALIF(!joinWPID.IsValid(), "Invalid WaypointID received in AddJoinWP");
    set<SlotID> attr;
    ConvertToSTLSet (LHS_att, attr);
    LT_Waypoint* WP = new LT_Join(joinWPID, attr, cleanerID, defs);
    return AddGraphNode(joinWPID, JoinWaypoint, WP);
}

bool LemonTranslator::AddAggregateWP(WayPointID aggWPID)
{
    PDEBUG("LemonTranslator::AddAggregateWP(WayPointID aggWPID = %s)", aggWPID.getName().c_str());
    FATALIF(!aggWPID.IsValid(), "Invalid WaypointID received in AddAggregateWP");
    LT_Waypoint* WP = new LT_Aggregate(aggWPID);
    return AddGraphNode(aggWPID, AggregateWaypoint, WP);
}

bool LemonTranslator::AddGLAWP(WayPointID glaWPID)
{
    PDEBUG("LemonTranslator::AddGLAWP(WayPointID glaWPID = %s)", glaWPID.getName().c_str());
    FATALIF(!glaWPID.IsValid(), "Invalid WaypointID received in AddGLAWP");
    LT_Waypoint* WP = new LT_GLA(glaWPID);
    return AddGraphNode(glaWPID, GLAWaypoint, WP);
}

bool LemonTranslator::AddGLALargeWP(WayPointID glaWPID)
{
    PDEBUG("LemonTranslator::AddGLALargeWP(WayPointID glaWPID = %s)", glaWPID.getName().c_str());
    FATALIF(!glaWPID.IsValid(), "Invalid WaypointID received in AddGLALargeWP");
    LT_Waypoint* WP = new LT_GLA_large(glaWPID);
    return AddGraphNode(glaWPID, GLALargeWaypoint, WP);
}


bool LemonTranslator::AddPrintWP(WayPointID printWPID)
{
    PDEBUG("LemonTranslator::AddPrintWP(WayPointID printWPID = %s)", printWPID.getName().c_str());
    FATALIF(!printWPID.IsValid(), "Invalid WaypointID received in AddPrintWP");
    LT_Waypoint* WP = new LT_Print(printWPID);
    return AddGraphNode(printWPID, PrintWaypoint, WP);
}

bool LemonTranslator::AddEdge(WayPointID start, WayPointID end)
{
    PDEBUG("LemonTranslator::AddEdge(WayPointID start = %s, WayPointID end = %s)", start.getName().c_str(), end.getName().c_str());
    FATALIF(!start.IsValid(), "Invalid WaypointID received in AddEdge");
    FATALIF(!end.IsValid(), "Invalid WaypointID received in AddEdge");
    map<WayPointID, ListDigraph::Node>::const_iterator itStart = IDToNode.find(start);
    if (itStart == IDToNode.end()) {
        cout << "No start node found in graph for specified waypoint ID";
        return false;
    }
    map<WayPointID, ListDigraph::Node>::const_iterator itEnd = IDToNode.find(end);
    if (itEnd == IDToNode.end()) {
        cout << "No end node found in graph for specified waypoint ID";
        return false;
    }
    ListDigraph::Arc arc = graph.addArc(itStart->second, itEnd->second);
    terminatingArcMap[arc] = false;
}

bool LemonTranslator::AddTerminatingEdge(WayPointID start, WayPointID end)
{
    PDEBUG("LemonTranslator::AddTerminatingEdge(WayPointID start = %s, WayPointID end = %s)", start.getName().c_str(), end.getName().c_str());
    FATALIF(!start.IsValid(), "Invalid WaypointID received in AddTerminatingEdge");
    FATALIF(!end.IsValid(), "Invalid WaypointID received in AddTerminatingEdge");
    map<WayPointID, ListDigraph::Node>::const_iterator itStart = IDToNode.find(start);
    if (itStart == IDToNode.end()) {
        cout << "No start node found in graph for specified waypoint ID";
        return false;
    }
    map<WayPointID, ListDigraph::Node>::const_iterator itEnd = IDToNode.find(end);
    if (itEnd == IDToNode.end()) {
        cout << "No end node found in graph for specified waypoint ID";
        return false;
    }
    ListDigraph::Arc arc = graph.addArc(itStart->second, itEnd->second);
    terminatingArcMap[arc] = true;
}

// Selection, Join
bool LemonTranslator::AddFilter(WayPointID wpID, QueryID queryID, SlotContainer& atts, string expr, string initializer, string defs)
{
    PDEBUG("LemonTranslator::AddFilter(WayPointID wpID = %s, QueryID queryID = %s, SlotContainer& atts = %s, string expr = %s, string initializer = %s)",
         wpID.getName().c_str(), queryID.ToString().c_str(), (GetAllAttrAsString(atts)).c_str(), expr.c_str(), initializer.c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddFilter");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddFilter(queryID, attr, expr, initializer, defs);
}

bool LemonTranslator::AddSynthesized(WayPointID wpID, QueryID queryID, SlotID att,
        SlotContainer& atts, string expr, string initializer, string defs)
{
    //PDEBUG("LemonTranslator::AddSynthesized(WayPointID wpID = %s, QueryID queryID = %s, SlotID attr = %s)", wpID.getName().c_str(), queryID.ToString().c_str(), (GetAllAttrAsString(attr)).c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddSynthesized");
    LT_Waypoint* WP = NULL;
    map<WayPointID, ListDigraph::Node>::const_iterator it = IDToNode.find(wpID);
    if (it == IDToNode.end()) {
        cout << "No graph node found for specified waypoint ID";
        return false;
    }
    WP = nodeToWaypointData[it->second];
    if (WP == NULL) {
        cout << "No waypoint details found for gven node";
        return false;
    }
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddSynthesized(queryID, att, attr, expr, initializer, defs);
}

// Aggregate. Need a list of these
bool LemonTranslator::AddAggregate(WayPointID wpID, QueryID query,
        SlotID aggAttrib,  /* attribute corresponding to aggregate */
        string aggregateType, /* type, just pass along */
        SlotContainer& atts, string expr, string initializer,
        string defs)
{
    PDEBUG("LemonTranslator::AddAggregate(WayPointID wpID = %s, QueryID query = %s, SlotID aggAttrib = %s, string aggregateType = %s, SlotContainer& atts = %s, string expr = %s, string initializer = %s)", wpID.getName().c_str(), query.ToString().c_str(), (GetAllAttrAsString(aggAttrib)).c_str(), aggregateType.c_str(), (GetAllAttrAsString(atts)).c_str(), expr.c_str(), initializer.c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddAggregate");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddAggregate(query, aggAttrib, aggregateType, attr, expr, initializer, defs);
}

//GLA, one per query basis
bool LemonTranslator::AddGLA(WayPointID wpID, QueryID query,
        SlotContainer& resultAtts, /*list of attributes produced as the result */
        string glaName, /*name of the GLA eg. AverageGLA, CountGLA, myGLA etc */
        string glaDef,
        string constructorExp, /*expression in GLA constructor */
        SlotContainer& atts, string expr, string initializer)
{
    PDEBUG("LemonTranslator::AddGLA(WayPointID wpID = %s, QueryID query = %s, SlotContainer resultAtts = %s, string glaName = %s, string constructorExp = %s, SlotContainer& atts = %s, string expr = %s, string initializer = %s)", wpID.getName().c_str(), query.ToString().c_str(), (GetAllAttrAsString(resultAtts)).c_str(), glaName.c_str(), constructorExp.c_str(), (GetAllAttrAsString(atts)).c_str(), expr.c_str(), initializer.c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddAggregate");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddGLA(query, resultAtts, glaName, glaDef, constructorExp, attr, expr, initializer);
}

//GLA, one per query basis
bool LemonTranslator::AddGLALarge(WayPointID wpID, QueryID query,
            SlotContainer& resultAtts, /*list of attributes produced as the result */
            string glaName, /*name of the GLA eg. AverageGLA, CountGLA, myGLA etc */
            string glaDef,
            string constructorExp, /*expression in GLA constructor */
            SlotContainer& atts, string expr, string initializer)
{
    PDEBUG("LemonTranslator::AddGLALarge(WayPointID wpID = %s, QueryID query = %s, SlotContainer resultAtts = %s, string glaName = %s, string constructorExp = %s, SlotContainer& atts = %s, string expr = %s, string initializer = %s)", wpID.getName().c_str(), query.ToString().c_str(), (GetAllAttrAsString(resultAtts)).c_str(), glaName.c_str(), constructorExp.c_str(), (GetAllAttrAsString(atts)).c_str(), expr.c_str(), initializer.c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddGLALarge");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddGLA(query, resultAtts, glaName, glaDef, constructorExp, attr, expr, initializer);
}

// Selection, Join. Queries added one by one
bool LemonTranslator::AddBypass(WayPointID wpID, QueryID query)
{
    PDEBUG("LemonTranslator::AddBypass(WayPointID wpID = %s, QueryID query %s)", wpID.getName().c_str(), query.ToString().c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddBypass");
    LT_Waypoint* WP = NULL;
    SlotContainer atts; // dummy
    set<SlotID> attr; // dummy
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    return WP->AddBypass(query);
}

// Join
bool LemonTranslator::AddJoin(WayPointID wpID, QueryID query,
        SlotContainer& RHS_atts /* right hand side attributes */,
        JoinType type, string defs)
{
    PDEBUG("LemonTranslator::AddJoin(WayPointID wpID = %s, QueryID query = %s, SlotContainer& RHS_atts = %s)", wpID.getName().c_str(), query.ToString().c_str(), (GetAllAttrAsString(RHS_atts)).c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddJoin");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, RHS_atts, attr, WP) == false) return false;
    return WP->AddJoin(query, attr, type, defs);
}


// Print
bool LemonTranslator::AddPrint(WayPointID wpID, QueryID query,
        SlotContainer& atts, string expr /* what to print */,
        string initializer, string name, string type, string file,
        string defs)
{
    PDEBUG("LemonTranslator::AddPrint(WayPointID wpID = %s, QueryID query = %s, SlotContainer& atts = %s, string expr = %s, string initializer = %s)", wpID.getName().c_str(), query.ToString().c_str(), (GetAllAttrAsString(atts)).c_str(), expr.c_str(), initializer.c_str());
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddPrint");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    queryToRootMap[query] = IDToNode[wpID];
    return WP->AddPrint(query, attr, expr, initializer, name, type, file, defs);
}

// Add writing capabilities to a scanner
// query specifies which query the scanner acts as a writer
bool LemonTranslator::AddWriter(WayPointID wpID, QueryID query){
    PDEBUG("LemonTranslator::AddWriter(WayPointID wpID, QueryID query)");
    FATALIF(!wpID.IsValid(), "Invalid WaypointID received in AddPrint");
    LT_Waypoint* WP = NULL;
    set<SlotID> attr;
    SlotContainer atts;
    if (GetWaypointAttr(wpID, atts, attr, WP) == false) return false;
    queryToRootMap[query] = IDToNode[wpID];
    return WP->AddWriter(query);
}

bool LemonTranslator::Run(QueryIDSet queries)
{
    PDEBUG("LemonTranslator::Run(QueryIDSet queries = %s)", queries.ToString().c_str());
    ofstream file;
        file.open("DOT.dot");
           PrintDOT(file);
    file.close();

    // Before running analysis bottom up in topological order, check if it is DAG
    // because topological traversal is possible only on DAG
    if (!dag(graph)) {
        cout << "This graph is not Directed acyclic graph";
        return false;
    }
    //ClearAllDataStructure(); otherwise all scanner atts will go away
    // Bottom up traversal required before top down
    AnalyzeAttUsageBottomUp(queries);

    //for each query in queries, do top down analysis
  QueryIDSet tmp = queries.Clone ();
    while (!tmp.IsEmpty())
    {
      QueryID q = tmp.GetFirst ();
      FATALIF(queryToRootMap.find(q) == queryToRootMap.end(), "Query has no top");

        WayPointID wp = nodeToWaypointData[queryToRootMap[q]]->GetId();

    FATALIF(!wp.IsValid(), "No valid top node, what is happening");

        QueryExit exitWP(q, wp);
        set<SlotID> dummy;
        // Call analysis
        AnalyzeAttUsageTopDown(q, queryToRootMap[q], exitWP, dummy);
    }
    isNewRun = true;

    // add the queries to the list of new queries
    newQueries.Union(queries);

    return true;
}

bool LemonTranslator::GetConfig(string dir,
                                                                QueryExitContainer& newQueries,
        DataPathGraph& graph,
        WayPointConfigurationList& wpList){
    PDEBUG("LemonTranslator::GetConfig(string dir,DataPathGraph& graph,WayPointConfigurationList& wpList)");

    if (isNewRun) {
        string objects = WriteM4Files(dir);

        // touch the files in touchlist.sh to ensure that they do not get
        // regenerated. This allows debugging
        execute_command ("bash touchlist.sh");

        CompileCode(dir, objects);

        PopulateGraph(graph);
        PopulateWayPointConfigurationData(wpList);
        PlotGraph(dir);

        // compute the list of

        isNewRun = false;
        return true;
    } else {
        return false;
    }
}

void LemonTranslator::PlotGraph(string dir){
}

void LemonTranslator::PopulateWayPointConfigurationData(WayPointConfigurationList& myConfigs){

    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];

                //WayPointConfigureData wpConfig;
                //if (wp->GetConfig(wpConfig))
                //    myConfigs.Insert(wpConfig);
                // Below code should be made active and above if condition must be commented
                // JOINMERGE
                WayPointConfigureData wpConfig;
                WayPointConfigurationList confList;
                if (wp->GetType() != JoinWaypoint && wp->GetConfig(wpConfig))
                     myConfigs.Insert(wpConfig);
                else if (wp->GetConfigs(confList)) {
                    confList.MoveToStart();
                    while (confList.RightLength()) {
                        myConfigs.Insert(confList.Current());
                        confList.Advance();
                    }
                }
            }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }

    // Now deal with the cleaner
    WorkFunc tempFunc = NULL;
    CleanerWorkFunc myCleanerWorkFunc (tempFunc);
    WorkFuncContainer myCleanerWorkFuncs;
    myCleanerWorkFuncs.Insert (myCleanerWorkFunc);

    // this is the set of query exits that end at it, and flow through it (none!)
    QueryExitContainer myCleanerEndingQueryExits;
    QueryExitContainer myCleanerFlowThroughQueryExits;

    // here is the waypoint configuration data
    HashTable tempTable;
    tempTable.Clone (centralHashTable);
    HashTableCleanerConfigureData cleanerConfigure (cleanerID, myCleanerWorkFuncs,
        myCleanerEndingQueryExits, myCleanerFlowThroughQueryExits, tempTable);

    myConfigs.Insert (cleanerConfigure);


}

void LemonTranslator::PopulateWaypoints(SymbolicWPConfigContainer& waypoints){
    SymbolicWPConfigContainer tmp;
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                SymbolicWaypointConfig symbolicWP(wp->GetType(), wp->GetId());
                //wp->GetSymbolicWPConfig(symbolicWP); TBD for future when more details needed
                tmp.Insert(symbolicWP);
        }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }
    tmp.swap(waypoints);
}

void LemonTranslator::PopulateGraph(DataPathGraph& rez){
    DataPathGraph g;

    {
    // First iterate and add all nodes, then add edges later in other loop
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        //if (!bfs.reached(n)) {
            if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];

                // The waypoint ID
                WayPointID ID = wp->GetId();

                g.AddNode(ID);

                // JOINMERGE
                if (wp->GetType() == JoinWaypoint) {
                    // add join merge waypoint node
                    //string name = ID.getName();
                    //name += "_merge";
                    //WayPointID joinmergeID(name.c_str());
                    //g.AddNode(joinmergeID);
                    // add writer node
                    string namew = ID.getName();
                    namew += "_writer";
                    WayPointID joinwriterID(namew.c_str());
                    g.AddNode(joinwriterID);
                }

    //        }
            if (!bfs.reached(n)) {
                bfs.addSource(n);
                bfs.start();
            }
        }
    }
    g.AddNode(cleanerID);
    }

    // Traverse full graph step by step (lemon BFS algo) and fill the info for edges
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
            if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];

                // The waypoint ID
                WayPointID ID = wp->GetId();

                //g.AddNode(ID);

                // Now find all the outlinks for the node
                for (ListDigraph::OutArcIt arc(graph, n); arc != INVALID; ++arc) {
                    ListDigraph::Node next = graph.target(arc);
                    bool isTerminating = terminatingArcMap[arc];
                    if (next != topNode && next != bottomNode) {
                        LT_Waypoint* nextWP = nodeToWaypointData[next];

                        // get the query exits of top guy
                        QueryExitContainer queryExitsFlowThrough;
                        QueryExitContainer queryExitsEnding;
                        nextWP->GetQueryExits(queryExitsFlowThrough, queryExitsEnding);

                        // Get the top guy ID
                        WayPointID topGuyID = nextWP->GetId();

                        // JOINMERGE
                        // If join waypoint, add the other 3 nodes
                        // join_merge, join_writer, cleaner
                        // JOINMERGE
                        if (wp->GetType() == JoinWaypoint) {
                            //string name = ID.getName();
                            //name += "_merge";
                            //WayPointID joinmergeID(name.c_str());
                            string namew = ID.getName();
                            namew += "_writer";
                            WayPointID joinwriterID(namew.c_str());
                            if (!isTerminating) {
                                queryExitsFlowThrough.MoveToStart ();
                                while (queryExitsFlowThrough.RightLength ()) {
                                    QueryExit qe = queryExitsFlowThrough.Current ();
                                    //g.AddLink (joinmergeID, topGuyID, qe);
                                    g.AddLink (joinwriterID, topGuyID, qe); // Is this also with same qe?
                                    QueryID nullOne;
                                    QueryExit writerExit (nullOne, joinwriterID);
                                    g.AddLink (cleanerID, joinwriterID, writerExit);
                                    queryExitsFlowThrough.Advance ();
                                }
                            } else {
                                queryExitsEnding.MoveToStart ();
                                while (queryExitsEnding.RightLength ()) {
                                    QueryExit qe = queryExitsEnding.Current ();
                                    //g.AddLink (joinmergeID, topGuyID, qe);
                                    g.AddLink (joinwriterID, topGuyID, qe); // Is this also with same qe?
                                    QueryID nullOne;
                                    QueryExit writerExit (nullOne, joinwriterID);
                                    g.AddLink (cleanerID, joinwriterID, writerExit);
                                    queryExitsEnding.Advance ();
                                }
                            }
                        }

                        if (!isTerminating) {
                            queryExitsFlowThrough.MoveToStart ();
                            while (queryExitsFlowThrough.RightLength ()) {
                                QueryExit qe = queryExitsFlowThrough.Current ();
                                g.AddLink (ID, topGuyID, qe);
                                queryExitsFlowThrough.Advance ();
                            }
                        } else {

                            queryExitsEnding.MoveToStart ();
                            while (queryExitsEnding.RightLength ()) {
                                QueryExit qe = queryExitsEnding.Current ();
                                g.AddLink (ID, topGuyID, qe);
                                queryExitsEnding.Advance ();
                            }
                        }
                    }
                }
            }
            if (!bfs.reached(n)) {
                bfs.addSource(n);
                bfs.start();
            }
    }
#ifdef DEBUG
    g.Print();
#endif
    ofstream file1;
        file1.open("DOT1.dot");
           g.PrintDOT(file1);
    file1.close();
    g.swap(rez);
}


string LemonTranslator::CompileCode(string dir, string objects){
    // let's make the call string for generate.sh, which takes the
    // directory itself as a parameter
  string call = "./generate.sh " + dir + " \"" + objects + '\"';// + " 1>&2";
#ifdef DEBUG
    cout << call << "\n";
#endif
    // do the system call, generate.sh must return 0 on success
    int sysret = execute_command(call.c_str());
    if (sysret == -1){
      perror("LemonTranslator compile");
      FATAL("Unable to do the code generation on directory %s!",
            dir.c_str());
    }

    return dir+"/Generated.so";
}


bool LemonTranslator::AnalyzeAttUsageBottomUp(QueryIDSet queries)
{
    // Create a NodeMap which is kind of hash to store int for each corrosponding graph node
    ListDigraph::NodeMap<int> order(graph);
    // sort topologically
    topologicalSort(graph, order);
    // create a reverse map, sort order -> node
    map<int, ListDigraph::Node> sortedOrder; // we need reverse mapping (order, node)
    for (ListDigraph::ArcIt a(graph); a != INVALID; ++a) {
        sortedOrder[order[graph.source(a)]] = graph.source(a);
        sortedOrder[order[graph.target(a)]] = graph.target(a); // not to miss corner nodes
    }

    // Traversal must start from bottom-most point in toppological sorted order
    map<int, ListDigraph::Node>::const_iterator it = sortedOrder.begin();
    // assert if we are not starting from bottom node (which is kind of virtual node)
    assert(it->second == bottomNode);
    // Iterate on all nodes in topo sort order
    for (;it != sortedOrder.end(); ++it) {
        // get the node from the sort map
        ListDigraph::Node node = it->second;
        // get the actual waypoint from another map
        LT_Waypoint* thisWP = nodeToWaypointData[node];
        // If we found some WP, start processing (will be null for virtual bottom node and virtual top node)
        if (thisWP) {
            // if textLoader or scanner, just add all queries to them (they already have all
            // attributes needed). All attributes will ge given to each query to start with
            if (thisWP->GetType() == ScannerWaypoint || thisWP->GetType() == TextLoaderWaypoint )
                thisWP->AddScanner(queries); // same function for both waypoints

            // get the attributes which we need to send up
            map<QueryID, set<SlotID> > attributes;
            if ( !thisWP->PropagateUp(attributes) ){
                cout << "Wrong call to PropagateUp";
                return false;
            }
            // Pass the attributes to all outgoing connected waypoint nodes
            for (ListDigraph::OutArcIt arc(graph, node); arc != INVALID; ++arc) {
                // get the next up node
                ListDigraph::Node next = graph.target(arc);
                // get the waypoint from map
                LT_Waypoint* nextWP = nodeToWaypointData[next];

                // pass the attributes to above waypoint. Terminating edges are right of join and
                // edges going to print waypoints. Rest are non terminating
                // Right now we dont have terminating functions for scanner and textloader, and we dont need them basically
                // so explicit check to make sure correct functions are called if we have terminating edges from textloader to scanner
                // actually textLoader check can be removed because it is at lowest end and it can never be nextWaypoint pointer
                if (nextWP) {
                    //if ((thisWP->GetType() == ScannerWaypoint || thisWP->GetType() == TextLoaderWaypoint) || !terminatingArcMap[arc])
                    if ((nextWP->GetType() == ScannerWaypoint || nextWP->GetType() == TextLoaderWaypoint) || !terminatingArcMap[arc])
                        nextWP->ReceiveAttributes(attributes); // Will copy only if have those queries
                    else
                        nextWP->ReceiveAttributesTerminating(attributes); // Will copy only if have those queries
                }
            }
        }
    }
    // make sure we end at virtual top node
    assert(it->second == topNode);
    return true;
}


bool LemonTranslator::AnalyzeAttUsageTopDown(QueryID query,
        ListDigraph::Node node,
        QueryExit exitWP,
        set<SlotID>& propagated) {

    //exitWP.Print(); cout << flush;
    assert(exitWP.IsValid());
    // Proceed only if we are not virtual nodes
    if (node != topNode && node != bottomNode)
    {
        LT_Waypoint* thisWP = nodeToWaypointData[node];

        // If waypoint do not have query, just return from this recursion hierarchy
        if (thisWP && !thisWP->DoIHaveQueries(query)) return false;

        // This will be used in case we find terminating arc if this is terminating node
        // Because we need to keep on updating queryExits if we find some terminating arc
        // on the way down
        QueryExit qe(query, thisWP->GetId());

        // scan incoming edges to find next below nodes because graph is created in directed way upwards
        for (ListDigraph::InArcIt arc(graph, node); arc != INVALID; ++arc) {
            // get the down guy
            ListDigraph::Node next = graph.source(arc);
            //LT_Waypoint* wp = nodeToWaypointData[next];
            // see if its terminating edge
            bool isTerminating = terminatingArcMap[arc];

            set<SlotID> propDown;
            if (!isTerminating) {
                if (!thisWP->PropagateDown(query, propagated, propDown, exitWP) ){
                    cout << "\nWrong call to PropagateDown " << thisWP->GetType();
                    return false;
                }
            }
            else {
                QueryExit queryExit;
                queryExit.copy(qe);
                if ( !thisWP->PropagateDownTerminating(query, propagated, propDown, queryExit) ){
                    cout << "\nWrong call to PropagateDownTerminating " << thisWP->GetType();
                    return false;
                }
            }
            // recursive call
            QueryExit queryExit(exitWP);
            if (isTerminating)
                queryExit.copy(qe);
            AnalyzeAttUsageTopDown(query, next, queryExit, propDown);
        }
        // Removed scanner code from here
    }
}

void LemonTranslator::AddPreamble(ostream &out) {

    // get the current time in a nice, ascii form
    time_t rawtime = time(NULL);
    tm *timeinfo = localtime(&rawtime);

    // add the preamble with some debugging comments
    out << "dnl # CODE GENERATED BY DATAPATH ON " << asctime(timeinfo) << endl;
    out << "include(Modules.m4)" << endl;
    out << "M4_CODE_GENERATION_PREAMBLE" << endl << endl;
}


string LemonTranslator::WriteM4Files(string dir)
{
    // we accumulate the objects here
    string objects;

    // we assume the directory is already created

    // ask the attribute manager to drop its info in our directory
    AttributeManager &am = AttributeManager::GetAttributeManager();
    string amFname = dir + "/Attributes.m4";
    am.GenerateM4Files(amFname);

    // ask the query manager to drop its info in our directory
    QueryManager &qm = QueryManager::GetQueryManager();
    string qmFname = dir + "/Queries.m4";
    qm.GenerateM4Files(qmFname);

    // go though out waypoint and generate the code in dir
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                if (wp->GetType() != ScannerWaypoint){

                    objects+=wp->GetWPName()+".o ";

                    string wpFname = dir + "/"+wp->GetWPName()+".m4";
                    ofstream mout;
                    mout.open(wpFname.c_str());

                    // insert the preamble
                    AddPreamble(mout);

                    // for now we write on cout
                    // switch to a file/waypoint
                    wp->WriteM4File(mout); // not for scanners

                    if (wp->GetType() == JoinWaypoint) {
                        //objects+=wp->GetWPName()+"_merge"+".o ";
                        //objects+=wp->GetWPName()+"_writer"+".o ";
                    }

                    // close our Waypoint.m4 file
                    mout.close();
                }
        }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }

    // Now write M4 for cleaner
    objects+="Cleaner.o ";
    string wpFname = dir + "/"+"Cleaner"+".m4";
    ofstream mout;
    mout.open(wpFname.c_str());
    AddPreamble(mout);
    WriteM4FileCleaner(mout);
    mout.close();

    return objects;
}

void LemonTranslator::WriteM4FileCleaner (ostream& out) {

    set<SlotID> LHS;
    set<SlotID> RHS;
    QueryIDSet queries;
    GetAccumulatedLHSRHS (LHS, RHS, queries);

    AttributeManager& am = AttributeManager::GetAttributeManager();
    DataTypeManager & dTM = DataTypeManager::GetDataTypeManager();

    // Make sure we have included the files we need for the different data types
    // LHS attributes
    out << "m4_divert(0)" << endl;
    out << "// Includes for LHS" << endl;
    for (set<SlotID>::const_iterator it = LHS.begin(); it != LHS.end(); ++it)
    {
        SlotID s = *it;
        string attName = am.GetAttributeName(s);
        string attType = am.GetAttributeType(attName);
        string attFile = dTM.GetTypeFile(attType);

        out << "m4_include(</" << attFile <<  "/>)" << endl;
    }

    // RHS attributes
    out << "// Includes for RHS" << endl;
    for (set<SlotID>::const_iterator it = RHS.begin(); it != RHS.end(); ++it)
    {
        SlotID s = *it;
        string attName = am.GetAttributeName(s);
        string attType = am.GetAttributeType(attName);
        string attFile = dTM.GetTypeFile(attType);

        out << "m4_include(</" << attFile <<  "/>)" << endl;
    }

    string wpname = "Cleaner";
    out << "M4_CLEANER_MODULE(" << wpname << ", ";


    // LHS attributes
    out << "</";
    out << "(";
    for (set<SlotID>::const_iterator it = LHS.begin(); it != LHS.end();)
    {
        SlotID s = *it;
        out << am.GetAttributeName(s).c_str();
        ++it;
        if (it!=LHS.end())
            out << ",";
    }
    out << ")";
    out << "/>,";

    // RHS attributes
    out << "</";
    out << "(";
    for (set<SlotID>::const_iterator it = RHS.begin(); it != RHS.end();)
    {
        SlotID s = *it;
        out << am.GetAttributeName(s).c_str();
        ++it;
        if (it!=RHS.end())
            out << ",";
    }
    out << ")";
    out << "/>,";

    // Write the queryIDSet
    out << "</";
    out << queries.GetInt64();
    out << "/>";

    out << ")";

}

void LemonTranslator::GetAccumulatedLHSRHS(set<SlotID>& LHS, set<SlotID>& RHS, QueryIDSet& queries) {

    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                if (wp->GetType() == JoinWaypoint) {
                    wp->GetAccumulatedLHSRHS(LHS, RHS, queries);
                }
        }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }
}

bool LemonTranslator::GenerateMessages()
{
}

void LemonTranslator::FillTypeMap(std::map<WayPointID, WaypointType>& typeMap) {
    Bfs<ListDigraph> bfs(graph);
    bfs.init();
    for (ListDigraph::NodeIt n(graph); n != INVALID; ++n) {
        if (n != topNode && n != bottomNode) {
                LT_Waypoint* wp = nodeToWaypointData[n];
                typeMap[wp->GetId()] = wp->GetType();
        }
        if (!bfs.reached(n)) {
            bfs.addSource(n);
            bfs.start();
        }
    }
}
