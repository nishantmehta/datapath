dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
dnl We assume tha this file is included from Modules.m4 and that all
dnl the m4 libraries needed are loaded
dnl
dnl Also, the following macros should be defined
dnl M4_WPName(identifier) -- the name of the waypoint we define
dnl M4_AttsPrint(list of tuples: (query, ListToPrint)
dnl   The ListToPrint is a list of attributes to print for each query
dnl   (the attributes are printed in the specified order)
dnl   (this list has to be specified between () not [])
dnl
dnl For each attribute, we assume    that the the following macros are defined
dnl ATT_TYPE_longAttName as the type of the attribute
dnl ATT_SLOT_longAttName as the clot number of the attribute
dnl
dnl For each query, we assume that the following variable is defined
dnl QINDEX_qName as the index associated with the query. The index is used to get the QueryID
dnl

// module specifsic headers to allow separate compilation
#include <iostream>
#include <string.h>

extern "C"
int PrintWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {

    // TODO: Make this configurable
    const char DELIM = '|';

    // get the work description
    PrintWorkDescription myWork;
    myWork.swap (workDescription);
    Chunk &input = myWork.get_chunkToPrint ();

<//>M4_DECLARE_QUERYIDS(</M4_Print_List/>,</M4_Attribute_Queries/>)dnl

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)dnl
<//>M4_ACCESS_COLUMNS(</M4_Attribute_Queries/>,</input/>)dnl

<//>M4_EXTRACT_BITMAP(</input/>)dnl

    // for each query, define a stream variable
<//>m4_foreach(</_Q_/>, </M4_Print_List/>, </dnl
    FILE* file_<//>M4_QUERY_NAME(_Q_) = myWork.get_streams().Find(M4_QUERY_NAME(_Q_));
<//>/>)dnl
    // PRINTING
    char buffer[10000]; // ALIN, CHANGE THIS TO A DEFINED CONSTANT

    FOR_EACH_TUPLE(</input/>){
        QueryIDSet qry;
<//><//>GET_QUERIES(qry)

        // extract values of attributes from streams
<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries/>,queriesToRun)

dnl     qry.Print();
<//>m4_foreach(</_Q_/>, </M4_Print_List/>, </dnl
        // do M4_QUERY_NAME(_Q_)
        if (qry.Overlaps(M4_QUERY_NAME(_Q_))){
            int curr=0; // the position where we write the next attribute
            //strcpy(buffer, "OUTPUT M4_QUERY_NAME(_Q_)<//>|");
            //curr = strlen(buffer);

<//><//>m4_foreach(</_A_/>, M4_PRINT_LIST(_Q_),</dnl
            curr+=ToString(M4_VAL_SUBST(_A_),buffer+curr);
            buffer[curr-1] = DELIM;

<//><//>/>)dnl
            // now we print the buffer
            buffer[curr-1]='\n';
            buffer[curr]=0; // end of string
            fprintf(file_<//>M4_QUERY_NAME(_Q_), "%s", buffer);
        }

<//>/>)dnl
    }

    // just return some arbitrary value... don't worry about reconstructing the chunk
    return 0;
}
