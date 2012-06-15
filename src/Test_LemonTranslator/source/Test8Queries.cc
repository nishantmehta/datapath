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
/** This encodes the query plan with the 8 queries in the DataPath paper 

		Modeled after example.dp

*/

#include "EventProcessor.h"
#include "LemonTranslator.h"
#include "MetadataDB.h"
#include "AttributeManager.h"
#include "QueryManager.h"
#include "Logging.h"

#include <sys/stat.h>
#include <sys/types.h>

int main(void){
	
	// starting the logging
  StartLogging();
	LOG_ENTRY(1, "Starting the test");
	
	SetMetadataDB("catalog.sqlite");

	LemonTranslator t;
	
	// attribute manager that does the translation from attributes to SlotID
	// when it starts it will define the attributes of all relations
	AttributeManager& am = AttributeManager::GetAttributeManager();

	// Catalog and definition of file scanners
	Catalog& catalog=Catalog::GetCatalog();

	StringContainer relations = catalog.GetRelationNames();
	for (int i=0;  i<relations.size(); i++){
		string relName=relations[i];
		
		// we do not have real scanners(i.e. we do not start FileScanner)
		// we just define the WayPointIDs for all the scanners
		WayPointID id(relName);
	}

	// get it ready for adding queries
	QueryManager& qm = QueryManager::GetQueryManager();

	SlotContainer attribs;
	// new scanner part;
	cout << "Part\n";
	am.GetAttributesSlots("part", attribs); // put attributes in attribs
	WayPointID part=WayPointID::GetIdByName("part");
	t.AddScannerWP(part, attribs); 
	attribs.Clear();

	// new scanner lineitem;
	cout << "lineitem\n";
	am.GetAttributesSlots("lineitem", attribs); // put attributes in attribs
	WayPointID lineitem = WayPointID::GetIdByName("lineitem");
	t.AddScannerWP(lineitem, attribs); 
	attribs.Clear();

	cout << "customer\n";
	// new scanner customer;
	am.GetAttributesSlots("customer", attribs); // put attributes in attribs
	WayPointID customer = WayPointID::GetIdByName("customer");
	t.AddScannerWP(customer, attribs); 
	attribs.Clear();

	cout << "orders\n";
	// new scanner orders;
	am.GetAttributesSlots("orders", attribs); // put attributes in attribs
	WayPointID orders = WayPointID::GetIdByName("orders");
	t.AddScannerWP(orders, attribs); 
	attribs.Clear();

	cout << "supplier\n";
	// new scanner supplier;
	am.GetAttributesSlots("supplier", attribs); // put attributes in attribs
	WayPointID supplier = WayPointID::GetIdByName("supplier");
	t.AddScannerWP(supplier, attribs); 
	attribs.Clear();

	cout << "nation\n";
	//new scanner nation;
	am.GetAttributesSlots("nation", attribs); // put attributes in attribs
	WayPointID nation = WayPointID::GetIdByName("nation");
	t.AddScannerWP(nation, attribs); 
	attribs.Clear();

	// new waypoint W5  = Select:-part;
	WayPointID W5("W5");
	t.AddSelectionWP(W5);
	t.AddEdge(part, W5);

	// new waypoint W4  = Select:-lineitem;
	WayPointID W4("W4");
	t.AddSelectionWP(W4);
	t.AddEdge(lineitem, W4);
	
	// new waypoint W24 = Select:-customer;
	WayPointID W24("W24");
	t.AddSelectionWP(W24);
	t.AddEdge(customer, W24);
	

	// new waypoint W3  = Select:-orders;
	WayPointID W3("W3");
	t.AddSelectionWP(W3);
	t.AddEdge(orders, W3);

	// new waypoint W15 = Select:-supplier;
	WayPointID W15("W15");
	t.AddSelectionWP(W15);
	t.AddEdge(supplier, W15);

	// new waypoint W14 = Select:-nation;
	WayPointID W14("W14");
	t.AddSelectionWP(W14);
	t.AddEdge(nation, W14);

	//  JOINS
	SlotContainer atts;
	SlotID att;

	// new waypoint W17 = Join(lineitem_l_partkey):-W4, ?W5;
	WayPointID W17("W17");
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_partkey");
	atts.Append(att);
	t.AddJoinWP(W17, atts);
	t.AddEdge(W4, W17);
	t.AddTerminatingEdge(W5, W17);
							 
	// new waypoint W6  = Join(lineitem_l_orderkey):-W4, ?W3;
	WayPointID W6("W6");
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_orderkey") ;
	atts.Append(att);
	t.AddJoinWP(W6, atts);
	t.AddEdge(W4, W6);
	t.AddTerminatingEdge(W3, W6);
	

	// new waypoint W29 = Join(customer_c_nationkey):-W24, ?W14;
	WayPointID W29("W29");
	atts.Clear();
	att = am.GetAttributeSlot("customer_c_nationkey") ;
	atts.Append(att);
	t.AddJoinWP(W29, atts);
	t.AddEdge(W24, W29);
	t.AddTerminatingEdge(W14, W29);
	
	// new waypoint W16 = Join(supplier_s_nationkey):-W15, ?W14;
	WayPointID W16("W16");
	atts.Clear();
	att = am.GetAttributeSlot("supplier_s_nationkey") ;
	atts.Append(att);
	t.AddJoinWP(W16, atts);
	t.AddEdge(W15, W16);
	t.AddTerminatingEdge(W14, W16);
	
	// new waypoint W7  = Join(lineitem_l_partkey):- W6, ?W5;
	WayPointID W7("W7");
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_partkey") ;
	atts.Append(att);
	t.AddJoinWP(W7, atts);
	t.AddEdge(W6, W7);
	t.AddTerminatingEdge(W5, W7);
	
	// new waypoint W18 = Join(lineitem_l_suppkey):- W17, ?W16;
	WayPointID W18("W18");
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_suppkey") ;
	atts.Append(att);
	t.AddJoinWP(W18, atts);
	t.AddEdge(W17, W18);
	t.AddTerminatingEdge(W16, W18);
	
	// new waypoint W25 = Join(customer_c_custkey):- W24, ?W6;
	WayPointID W25("W25");
	atts.Clear();
	att = am.GetAttributeSlot("customer_c_custkey") ;
	atts.Append(att);
	t.AddJoinWP(W25, atts);
	t.AddEdge(W24, W25);
	t.AddTerminatingEdge(W6, W25);
	
	// new waypoint W33 = Join(customer_c_custkey):- W29, ?W6;
	WayPointID W33("W33");
	atts.Clear();
	att = am.GetAttributeSlot("customer_c_custkey") ;
	atts.Append(att);
	t.AddJoinWP(W33, atts);
	t.AddEdge(W29, W33);
	t.AddTerminatingEdge(W6, W33);
	
	// new waypoint W30 = Join(customer_c_custkey):- W29, ?W3;
	WayPointID W30("W30");
	atts.Clear();
	att = am.GetAttributeSlot("customer_c_custkey") ;
	atts.Append(att);
	t.AddJoinWP(W30, atts);
	t.AddEdge(W29, W30);
	t.AddTerminatingEdge(W3, W30);

	// new waypoint W26 = Join(customer_c_nationkey):- W25, ?W16;
	WayPointID W26("W26");
	atts.Clear();
	att = am.GetAttributeSlot("customer_c_nationkey") ;
	atts.Append(att);
	t.AddJoinWP(W26, atts);
	t.AddEdge(W25, W26);
	t.AddTerminatingEdge(W16, W26);



	////// AGGREGATES
	
	// new waypoint W10 = Aggregate:-W4;
	WayPointID W10("W10");
	t.AddAggregateWP(W10);
	t.AddEdge(W4, W10);
	
	// new waypoint W21 = Aggregate:-W17;
	WayPointID W21("W21");
	t.AddAggregateWP(W21);
	t.AddEdge(W17, W21);
	
	// new waypoint W36 = Aggregate:-W6;
	WayPointID W36("W36");
	t.AddAggregateWP(W36);
	t.AddEdge(W6, W36);
	
	// new waypoint W8  = Aggregate:-W7;
	WayPointID W8("W8");
	t.AddAggregateWP(W8);
	t.AddEdge(W7, W8);
	
	// new waypoint W19 = Aggregate:-W18;
	WayPointID W19("W19");
	t.AddAggregateWP(W19);
	t.AddEdge(W18, W19);
	
	// new waypoint W34 = Aggregate:-W33;
	WayPointID W34("W34");
	t.AddAggregateWP(W34);
	t.AddEdge(W33, W34);
	
	// new waypoint W31 = Aggregate:-W30;
	WayPointID W31("W31");
	t.AddAggregateWP(W31);
	t.AddEdge(W30, W31);
	
	// new waypoint W27 = Aggregate:-W26;
	WayPointID W27("W27");
	t.AddAggregateWP(W27);
	t.AddEdge(W26, W27);
	

	///// PRINTS

	// new waypoint W11 = Print:-W10;
	WayPointID W11("W11");
	t.AddPrintWP(W11);
	t.AddTerminatingEdge(W10, W11);
	
	// new waypoint W22 = Print:-W21;
	WayPointID W22("W22");
	t.AddPrintWP(W22);
	t.AddTerminatingEdge(W21, W22);
	
	// new waypoint W37 = Print:-W36;
	WayPointID W37("W37");
	t.AddPrintWP(W37);
	t.AddTerminatingEdge(W36, W37);
	
	// new waypoint W9  = Print:-W8;
	WayPointID W9("W9");
	t.AddPrintWP(W9);
	t.AddTerminatingEdge(W8, W9);
	
	// new waypoint W20 = Print:-W19;
	WayPointID W20("W20");
	t.AddPrintWP(W20);
	t.AddTerminatingEdge(W19, W20);
	
	// new waypoint W35 = Print:-W34;
	WayPointID W35("W35");
	t.AddPrintWP(W35);
	t.AddTerminatingEdge(W34, W35);
	
	// new waypoint W32 = Print:-W31;
	WayPointID W32("W32");
	t.AddPrintWP(W32);
	t.AddTerminatingEdge(W31, W32);
	
	// new waypoint W28 = Print:-W27;
	WayPointID W28("W28");
	t.AddPrintWP(W28);
	t.AddTerminatingEdge(W27, W28);
	

	////  LONG SPEC
	AttributeType doubleType = AttributeType::CreateFromName("DOUBLE");

	// new query Q1 {
	QueryID Q1;
	qm.AddNewQuery("Q1", Q1);

	//     // code for Q1 for each waypoint
	//     // order of waypoints does not matter
	//     waypoint W4 {
	//     	filter lineitem_l_shipdate > '1998-09-01';
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_shipdate");
	atts.Append(att);
	t.AddFilter(W4, Q1, atts, "(lineitem_l_shipdate) > Date(\"1998-09-01\")");


	//     }   
	//     waypoint W10 {
	//     	aggregate agg_q1_1=Count(1.0);
	atts.Clear();
	SlotID agg_q1_1 = am.AddSynthesizedAttribute(Q1, "agg_q1_1", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_1, "Count", atts, "1.0");

	// 	aggregate agg_q1_2=Average(lineitem_l_discount);
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q1_2 = am.AddSynthesizedAttribute(Q1, "agg_q1_2", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_2, "Average", atts, "(lineitem_l_discount)");
	
	// 	aggregate agg_q1_3=Average(lineitem_l_extendedprice);
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	SlotID agg_q1_3 = am.AddSynthesizedAttribute(Q1, "agg_q1_3", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_3, "Average", atts, "(lineitem_l_extendedprice)");
	
	// 	aggregate agg_q1_4=Average(lineitem_l_quantity);
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_quantity");
	atts.Append(att);
	SlotID agg_q1_4 = am.AddSynthesizedAttribute(Q1, "agg_q1_4", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_4, "Average", atts, "(lineitem_l_quantity)");
	
	// 	aggregate agg_q1_5=Sum(((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)) * (1.000000 + lineitem_l_tax)));
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_tax");
	atts.Append(att);
	SlotID agg_q1_5 = am.AddSynthesizedAttribute(Q1, "agg_q1_5", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_5, "Sum", atts, "(((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)) * (1.000000 + lineitem_l_tax)))");
	
	// 	aggregate agg_q1_6=Sum((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)));
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q1_6 = am.AddSynthesizedAttribute(Q1, "agg_q1_6", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_6, "Sum", atts, "(lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))");
	// 	aggregate agg_q1_7=Sum(lineitem_l_quantity); 
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_quantity");
	atts.Append(att);
	SlotID agg_q1_7 = am.AddSynthesizedAttribute(Q1, "agg_q1_7", doubleType);
	t.AddAggregate(W10, Q1, agg_q1_7, "Sum", atts, "(lineitem_l_quantity)");
	
	//    }	

	//    waypoint W11 {
	//    	print agg_q1_1, agg_q1_2, agg_q1_3, agg_q1_4, 
	// 	      agg_q1_5, agg_q1_6, agg_q1_7;
	atts.Clear();
	atts.Append(agg_q1_1);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_1)");
	
	atts.Clear();
	atts.Append(agg_q1_2);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_2)");
	 
	atts.Clear();
	atts.Append(agg_q1_3);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_3)");
	
 	atts.Clear();
	atts.Append(agg_q1_4);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_4)");
	
	atts.Clear();
	atts.Append(agg_q1_5);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_5)");
	
	atts.Clear();
	atts.Append(agg_q1_6);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_6)");
	
	atts.Clear();
	atts.Append(agg_q1_7);
	t.AddPrint(W11, Q1, atts, "(Q1_agg_q1_7)");
	
//    }
// }
	

// // forward definitions of queries
// new query Q0, Q2, Q3, Q4, Q5, Q6, Q7;
	QueryID Q0;
	qm.AddNewQuery("Q0", Q0);
	QueryID Q2;
	qm.AddNewQuery("Q2", Q2);
	QueryID Q3;
	qm.AddNewQuery("Q3", Q3);
	QueryID Q4;
	qm.AddNewQuery("Q4", Q4);
	QueryID Q5;
	qm.AddNewQuery("Q5", Q5);
	QueryID Q6;
	qm.AddNewQuery("Q6", Q6);
	QueryID Q7;
	qm.AddNewQuery("Q7", Q7);

// waypoint W3 {
//   query Q0 { filter (orders_o_orderdate >= Date('1997-2-20'))  && (orders_o_orderdate<Date('1997-3-24')); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	t.AddFilter(W3, Q0, atts, "((orders_o_orderdate >= Date(\"1997-2-20\"))  && (orders_o_orderdate<Date(\"1997-3-24\")))");
//   query Q4 { filter (orders_o_orderdate>='1997-03-01') && (orders_o_orderdate<'1997-04-07'); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	t.AddFilter(W3, Q4, atts, "(orders_o_orderdate>=Date(\"1997-03-01\")) && (orders_o_orderdate<Date(\"1997-04-07\"))");
//   query Q5 { filter (orders_o_orderdate >= '1997-03-02') && (orders_o_orderdate <= '1997-05-09'); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	t.AddFilter(W3, Q5, atts, "((orders_o_orderdate >= Date(\"1997-03-02\")) && (orders_o_orderdate <= Date(\"1997-05-09\")))");
//   query Q6 { filter (orders_o_orderdate > '1997-03-01') && (orders_o_orderdate <= '1997-04-07'); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	t.AddFilter(W3, Q6, atts, "((orders_o_orderdate > Date(\"1997-03-01\")) && (orders_o_orderdate <= Date(\"1997-04-07\")))");
//   query Q7 { filter (orders_o_orderdate > '1997-02-01') && (orders_o_orderdate <= '1997-05-07'); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	att = am.GetAttributeSlot("orders_o_orderdate");
	atts.Append(att);
	t.AddFilter(W3, Q7, atts, "((orders_o_orderdate > Date(\"1997-02-01\")) && (orders_o_orderdate <= Date(\"1997-05-07\")))");
// }

// waypoint W4 { 
//   bypass Q0, Q2, Q3, Q4, Q6, Q7;
	t.AddBypass(W4, Q0);
	t.AddBypass(W4, Q2);
	t.AddBypass(W4, Q3);
	t.AddBypass(W4, Q4);
	t.AddBypass(W4, Q6);
	t.AddBypass(W4, Q7);
// }

// waypoint W5 {
//   bypass Q0;
	t.AddBypass(W5, Q0);
//   query Q2 { filter (part_p_size >= 5.000000) && (part_p_size <= 10.000000); } 
	atts.Clear();
	att = am.GetAttributeSlot("part_p_size");
	atts.Append(att);
	t.AddFilter(W5, Q2, atts, "((part_p_size >= 5.000000) && (part_p_size <= 10.000000))");
//   query Q3 { filter (part_p_size == 13.000000); }
	atts.Clear();
	att = am.GetAttributeSlot("part_p_size");
	atts.Append(att);
	t.AddFilter(W5, Q3, atts, "((part_p_size >= 5.000000) && (part_p_size <= 10.000000))");
// }

// waypoint W6 { 
//   query Q0, Q4, Q6, Q7 { join orders_o_orderkey; }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_orderkey");
	atts.Append(att);
	t.AddJoin(W6, Q0, atts);
	t.AddJoin(W6, Q4, atts);
	t.AddJoin(W6, Q6, atts);
	t.AddJoin(W6, Q7, atts);
// }

// waypoint W7 {
//   query Q0 { join part_p_partkey; }
	atts.Clear();
	att = am.GetAttributeSlot("part_p_partkey");
	atts.Append(att);
	t.AddJoin(W7, Q0, atts);
// }

// waypoint W8 {
//   query Q0 { aggregate agg_q0_1=Average((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))); }
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q0_1 = am.AddSynthesizedAttribute(Q0, "agg_q0_1", doubleType);
	t.AddAggregate(W8, Q0, agg_q0_1, "Average", atts, "(lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))");
// }

// waypoint W9 {
//   query Q0 { print agg_q0_1; }
	atts.Clear();
	att = am.GetAttributeSlot(Q0, "agg_q0_1");
	atts.Append(att);
	t.AddPrint(W9, Q0, atts, "Q0_agg_q0_1");
// }

// waypoint W14 { 
//   query Q2 { filter nation_n_name == 'RUSSIA'; }
	atts.Clear();
	att = am.GetAttributeSlot("nation_n_name");
	atts.Append(att);
	t.AddFilter(W14, Q2, atts, "(nation_n_name == \"RUSSIA\")");
//   query Q4 { filter nation_n_name == 'JAPAN'; }
	t.AddFilter(W14, Q4, atts, "(nation_n_name == \"JAPAN\")");
//   query Q5 { filter (nation_n_name == 'FRANCE' || nation_n_name == 'GERMANY'); }
	t.AddFilter(W14, Q5, atts, "(nation_n_name == \"FRANCE\" || nation_n_name == \"GERMANY\")");
//   query Q6 { filter nation_n_name == \"ALGERIA\"; }
	t.AddFilter(W14, Q6, atts, "(nation_n_name == \"ALGERIA\")");
// }

// waypoint W15 {
//   bypass Q2, Q4;
	t.AddBypass(W15, Q2);
	t.AddBypass(W15, Q4);
// }

// waypoint W16 {
//   query Q2, Q4 { join nation_n_nationkey; } 
	atts.Clear();
	att = am.GetAttributeSlot("nation_n_nationkey");
	atts.Append(att);
	t.AddJoin(W16, Q2, atts);
	t.AddJoin(W16, Q4, atts);
// }

// waypoint W17 {
//   query Q2, Q3 { join part_p_partkey; }
	atts.Clear();
	att = am.GetAttributeSlot("part_p_partkey");
	atts.Append(att);
	t.AddJoin(W17, Q2, atts);
	t.AddJoin(W17, Q3, atts);
// }

// waypoint W18 {
//   query Q2 { join supplier_s_suppkey; }
	atts.Clear();
	att = am.GetAttributeSlot("supplier_s_suppkey");
	atts.Append(att);
	t.AddJoin(W18, Q2, atts);
// }

// waypoint W19 {
//   query Q2 { aggregate agg_q2_1=Average(lineitem_l_extendedprice); }
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	SlotID agg_q2_1 = am.AddSynthesizedAttribute(Q2, "agg_q2_1", doubleType);
	t.AddAggregate(W19, Q2, agg_q2_1, "Average", atts, "(lineitem_l_extendedprice)");
// }

// waypoint W20 {
//   query Q2 { print agg_q2_1; }
	atts.Clear();
	att = am.GetAttributeSlot(Q2, "agg_q2_1");
	atts.Append(att);
	t.AddPrint(W20, Q2, atts, "Q2_agg_q2_1");
// }

// waypoint W21 {
//   query Q3 { 
//     aggregate agg_q3_1=Sum( ( (Like(part_p_type,'%PROMO%')) ? ((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))) : (0.000000) ) ); 
	atts.Clear();
	att = am.GetAttributeSlot("part_p_type");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q3_1 = am.AddSynthesizedAttribute(Q3, "agg_q3_1", doubleType);
	t.AddAggregate(W21, Q3, agg_q3_1, "Sum", atts, "( ( (Like(part_p_type,\"%PROMO%\")) ? ((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))) : (0.000000) ) )");
//     aggregate agg_q3_2=Sum( (lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)) );
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q3_2 = am.AddSynthesizedAttribute(Q3, "agg_q3_2", doubleType);
	t.AddAggregate(W21, Q3, agg_q3_2, "Sum", atts, "((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)))");
//   }
// }

// waypoint W22 {
//   query Q3 { print ((100.000000 * agg_q3_1) / agg_q3_2); }	
	atts.Clear();
	att = am.GetAttributeSlot(Q3, "agg_q3_1");
	atts.Append(att);
	att = am.GetAttributeSlot(Q3, "agg_q3_2");
	atts.Append(att);
	t.AddPrint(W22, Q3, atts, "((100.000000 * Q3_agg_q3_1) / Q3_agg_q3_2)");
// } 

// waypoint W24 {
//   bypass Q4, Q5, Q6;
	t.AddBypass(W24, Q4);
	t.AddBypass(W24, Q5);
	t.AddBypass(W24, Q6);
// }

// waypoint W25 {
//   query Q4 { join orders_o_custkey; }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_custkey");
	atts.Append(att);
	t.AddJoin(W25, Q4, atts);
// }

// waypoint W26 {
//   query Q4 { join supplier_s_nationkey; }
	atts.Clear();
	att = am.GetAttributeSlot("supplier_s_nationkey");
	atts.Append(att);
	t.AddJoin(W26, Q4, atts);
// }

// waypoint W27 {
//   query Q4 {
//     aggregate agg_q4_1=Sum(( (nation_n_name == 'JAPAN') ? ((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))) : (0.000000) ));
	atts.Clear();
	att = am.GetAttributeSlot("nation_n_name");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q4_1 = am.AddSynthesizedAttribute(Q4, "agg_q4_1", doubleType);
	t.AddAggregate(W27, Q4, agg_q4_1, "Sum", atts, "(( (nation_n_name == \"JAPAN\") ? ((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))) : (0.000000) ))");
//     aggregate agg_q4_2=Sum((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)));
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q4_2 = am.AddSynthesizedAttribute(Q4, "agg_q4_2", doubleType);
	t.AddAggregate(W27, Q4, agg_q4_2, "Sum", atts, "((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount)))");
//   } 
// }

// waypoint W28 {
//   query Q4 { print (agg_q4_1 / agg_q4_2); }
	atts.Clear();
	att = am.GetAttributeSlot(Q4, "agg_q4_1");
	atts.Append(att); 
	t.AddPrint(W28, Q4, atts, "Q4_agg_q4_1");
// }

// waypoint W29 {
//   query Q5, Q6 { join nation_n_nationkey; }
	atts.Clear();
	att = am.GetAttributeSlot("nation_n_nationkey");
	atts.Append(att);
	t.AddJoin(W29, Q5, atts);
	t.AddJoin(W29, Q6, atts);
// }

// waypoint W30 {
//   query Q5 { join orders_o_custkey; }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_custkey");
	atts.Append(att);
	t.AddJoin(W30, Q5, atts);
// }

// waypoint W31 {
//   query Q5 { aggregate agg_q5_1=Average(orders_o_totalprice); }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_totalprice");
	atts.Append(att);
	SlotID agg_q5_1 = am.AddSynthesizedAttribute(Q5, "agg_q5_1", doubleType);
	t.AddAggregate(W31, Q5, agg_q5_1, "Average", atts, "(orders_o_totalprice)");
// }

// waypoint W32 {
//   query Q5 { print agg_q5_1; }
	atts.Clear();
	att = am.GetAttributeSlot(Q5, "agg_q5_1");
	atts.Append(att);
	t.AddPrint(W32, Q5, atts, "Q5_agg_q5_1");
// }

// waypoint W33 {
//   query Q6 { join orders_o_custkey; }
	atts.Clear();
	att = am.GetAttributeSlot("orders_o_custkey");
	atts.Append(att);
	t.AddJoin(W33, Q6, atts);
// }

// waypoint W34 {
//   query Q6 { aggregate agg_q6_1=Count(1.0); }
	atts.Clear();
	SlotID agg_q6_1 = am.AddSynthesizedAttribute(Q6, "agg_q6_1", doubleType);
	t.AddAggregate(W34, Q6, agg_q6_1, "Count", atts, "(1.0)");
// }

// waypoint W35 {
//   query Q6 { print agg_q6_1; }
	atts.Clear();
	att = am.GetAttributeSlot(Q6, "agg_q6_1");
	atts.Append(att);
	t.AddPrint(W35, Q6, atts, "Q6_agg_q6_1");
// }

// waypoint W36 {
//   query Q7 { aggregate agg_q7_1=Sum((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))); }
	atts.Clear();
	att = am.GetAttributeSlot("lineitem_l_extendedprice");
	atts.Append(att);
	att = am.GetAttributeSlot("lineitem_l_discount");
	atts.Append(att);
	SlotID agg_q7_1 = am.AddSynthesizedAttribute(Q7, "agg_q7_1", doubleType);
	t.AddAggregate(W36, Q7, agg_q7_1, "Sum", atts, "(((lineitem_l_extendedprice * (1.000000 - lineitem_l_discount))))");
// }

// waypoint W37 {
//   query Q7 { print agg_q7_1; }
	atts.Clear();
	att = am.GetAttributeSlot(Q7, "agg_q7_1");
	atts.Append(att);
	t.AddPrint(W37, Q7, atts, "Q7_agg_q7_1");
// }

// run query Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7;


	string dir="Generated";
	// make the directory if it does not already exist
	mkdir(dir.c_str(), 0777);

	QueryIDSet qSet;
	qSet.Union(Q0);
	qSet.Union(Q1);
	qSet.Union(Q2);
	qSet.Union(Q3);
	qSet.Union(Q4);
	qSet.Union(Q5);
	qSet.Union(Q6);
	qSet.Union(Q7);
	t.Run(qSet, dir);
	
	return 0;
}
