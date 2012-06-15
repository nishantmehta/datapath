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
#include <stdio.h>
#include <vector>
#include "AST.h"
#include "ASTVisitor.h"
#include "Optimizer1.h"
#include "TopPredicates.h"

#include "MetadataDB.h"

using namespace std;

extern "C" {
  int yyparse(void);
}

extern vector<SFWQuery *> queries;
extern FILE *yyin;

int main(int argc, char **argv) {

  SetMetadataDB( "datapath.sqlite");
  
  // forever-increasing query id
  int qid = 0;

  // what 
  cout << "Created the metadata DB.\n";
  NetworkIntegrator ni;
  cout << "Created the network integrator.\n";
  Optimizer *opt = new Optimizer(ni, "output", LOOKAHEAD_SEARCH);
  cout << "Created the optimizer.\n";

  // forever ask for input files.
  while (true) {
    char iFile[512];
    printf("inputfile> ");
    scanf("%s", iFile);

    yyin = fopen(iFile, "r");
    if (yyin == NULL) {
      printf("not found!\n");
      continue;
    }
    
    yyparse();
    opt->ReInitialize();

    for (int i=0;i<queries.size();i++) {

      // do the type checking
      TypeChecker tc;
      bool ok = tc.CheckTypes(queries[i]);

      if (ok) {

	// print the query tree
	TreePrinter tp;
	tp.PrintQuery(queries[i]);
	
	// do the conversion
	Converter c(opt, qid++);
	c.FillOptimizer(queries[i]);

	// optimize
	opt->Optimize();
      }
    }

    opt->ProduceOutput();


    // clear the set of queries, prepare for the next file
    queries.clear();
  }
}
