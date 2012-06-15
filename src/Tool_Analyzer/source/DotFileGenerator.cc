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
#include "DotFileGenerator.h"

#include <fstream>
#include <string.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


void DotFileGenerator::CreateDotFile(string filename){
	fileName = filename;
	outfile.open ((fileName + ".dot").c_str());
	outfile << "digraph "<< fileName << " {" << endl;
}

void DotFileGenerator::AddNode(string src, string dest, string type ,long edgeCount, int c){
	string color[] = {"red", "navy", "green", "lightgoldenrod4", "black", "orange", "maroon", "yellow3"};

	outfile << src << " -> " << dest << "[label = \""<<c<<"-"<< type << "[" << edgeCount << "]\", fontcolor = "<<color[c%8]<<", color = "<<color[c%8]<<"]" << endl;
}


void DotFileGenerator::CloseDotFile(){
	outfile << "}" << endl;
	outfile.close();

	//execute dot tool to convert .dot file to .ps file which can be opened by a pdf reader
	system(("dot -Tps "+ fileName +".dot -o "+ fileName +".ps").c_str());
}

