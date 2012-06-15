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
#include "LT_GLA_large.h"
#include "AttributeManager.h"

LT_GLA_large::~LT_GLA_large(void){}

bool LT_GLA_large::GetConfig(WayPointConfigureData& where){

    // get the ID
    WayPointID glaIDOne = GetId ();

    // first, get the function we will send to it

//    WorkFunc tempFunc = NULL;
    WorkFuncContainer myGLAWorkFuncs;

        GLALargeProcessChunkWorkFunc GLAProcessChunkWF (NULL);
    GLALargeFragmentCountWorkFunc GLAFragCountWF (NULL);
    GLALargeFinalizeWorkFunc GLAFinalizeWF (NULL);
    GLALargeDeallocateWorkFunc GLADeallocateWF (NULL);

    myGLAWorkFuncs.Insert (GLAProcessChunkWF);
    myGLAWorkFuncs.Insert (GLAFragCountWF);
    myGLAWorkFuncs.Insert (GLAFinalizeWF);
    myGLAWorkFuncs.Insert (GLADeallocateWF);

    // this is the set of query exits that end at it, and flow through it
    QueryExitContainer myGLAEndingQueryExits;
    QueryExitContainer myGLAFlowThroughQueryExits;
    GetQueryExits (myGLAFlowThroughQueryExits, myGLAEndingQueryExits);
    PDEBUG("Printing query exits for AGGREGATE WP ID = %s", glaIDOne.getName().c_str());

    // here is the waypoint configuration data
    GLALargeConfigureData glaConfigure (glaIDOne, myGLAWorkFuncs,  myGLAEndingQueryExits, myGLAFlowThroughQueryExits);

    // and add it
    where.swap (glaConfigure);

    return true;
}

void LT_GLA_large::WriteM4File(ostream& out) {
    WriteM4DataStructures(out);
    out << "M4_GLA_LARGE_MODULE(" ;
    WriteM4FileAux(out);
}
