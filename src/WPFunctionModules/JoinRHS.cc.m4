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
dnl  We assume tha this file is included from Modules.m4 and that all
dnl  the m4 libraries needed are loaded
dnl

dnl Arguments:
dnl
dnl M4_WPName -- name of the waypoint
dnl
dnl
dnl M4_Attribute_Queries_RHS -- the set of queries in which each attribute is used
dnl                elements of the form: ( attribute, serialized_QueryIDSet )
dnl M4_Query_Class_Hash -- the list of query classes. Each element is of the form
dnl       (  serialized_Queries, list_hash_atts (in order),
dnl                             hashed_attributes_queries (what attributes to put in hash, all inclusing the ones in list_hash)
dnl                             /*same structure as attribute queries */

m4_divert(-1)
include(Resources-T.m4)

#include "WorkDescription.h"
#include "ExecEngineData.h"
#include "Column.h"
#include "ColumnIterator.cc"
#include "MMappedStorage.h"
#include "BString.h"
#include "BStringIterator.h"
#include "IntFuncs.h"
#include "BitstringFuncs.h"
m4_divert(0)

extern "C"
int JoinRHSWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {

    // M4_WPNam = M4_WPName
    // M4_LHS_Has = M4_LHS_Hash
    // M4_Attribute_Queries_LH = M4_Attribute_Queries_LHS
    // M4_Atrribute_Queries_LHS_cop = M4_Atrribute_Queries_LHS_copy
    // M4_Query_Class_Has = M4_Query_Class_Hash
    // M4_Attribute_Queries_RH = M4_Attribute_Queries_RHS
    // M4_Atrribute_Queries_RHS_cop = M4_Atrribute_Queries_RHS_copy
    // M4_Hash_RHS_ATT = M4_Hash_RHS_ATTR
    // M4_Queries_Attribute_Comparisio = M4_Queries_Attribute_Comparision

    double start_time = global_clock.GetTime();

    // this is the area where all of the intermediate, serialized records are stored
    SerializedSegmentArray serializedSegments [NUM_SEGS];

    // this is the area where all of the records are serialized to;
    // 10K bytes are initially used for this
    void *serializeHere = (void *) malloc (10000);
    int storageSize = 10000;

    // go to the work description and get the input chunk
    JoinRHSWorkDescription myWork;
    myWork.swap (workDescription);
    Chunk &input = myWork.get_chunkToProcess ();

    // get the waypoint identifier
    unsigned int wayPointID = myWork.get_wayPointID ();

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

dnl M4_ACCESS_COLUMNS(</M4_Attribute_Queries_RHS/>,</input/>)dnl
<//>M4_ACCESS_COLUMNS(m4_quote(reval(</m4_args/>M4_Attribute_Queries_RHS)),</input/>)dnl

<//>M4_EXTRACT_BITMAP(</input/>)dnl

    int totalNum = 0; // counter for the tuples processed

    // now actually hash all of the tuples!
    FOR_EACH_TUPLE(</input/>) {
        QueryIDSet qry;
<//><//>GET_QUERIES(qry)


        // extract values of attributes from streams
dnl     M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries_RHS/>,qry)
        M4_ACCESS_ATTRIBUTES_TUPLE(m4_quote(reval(</m4_args/>M4_Attribute_Queries_RHS)),queriesToRun)

         if (qry.IsEmpty()){
             continue;
         }

        totalNum++;

dnl     # For each query class (queries with the same set of join attributes)
<//>m4_foreach(</_QC_/>,</M4_Query_Class_Hash/>,</dnl
        // Dealing with join attributes m4_second(_QC_)
        if (qry.Overlaps(QueryIDSet(m4_first(_QC_), true))) {

            HT_INDEX_TYPE hashValue = HASH_INIT;
dnl         # hash all hashing attributes (use chain hashing)
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>m4_second(_QC_))),</dnl
            hashValue = CongruentHash(Hash(_A_<//>), hashValue);
<//><//>/>)dnl

            // figure out which of the hash buckets it goes into
            unsigned int index = WHICH_SEGMENT (hashValue);

            // and serialize the record!  Begin with the bitstring.
            // TBD TBD SS: check if Bitstring takes value that way !
dnl         Bitstring myInBString(QueryIDSet(m4_first(_QC_), true));
            Bitstring myInBString(m4_first(_QC_), true);
            myInBString.Intersect(qry);

            int bytesUsed = M4_SERIALIZED_SIZE(Bitstring, myInBString);

            // Make sure we have the storage...
            if (bytesUsed > storageSize) {
                storageSize = bytesUsed;
                free (serializeHere);
                serializeHere = (void *) malloc (storageSize);
            }

            // do the serialization...
            void *location = M4_OPTIMIZED_SERIALIZE(Bitstring, myInBString, serializeHere);

            // printf("\n ================= StartNew , hashbucket index = %ld, hashval = %ld, slot = %ld =====================\n", index, hashValue,  WHICH_SLOT (hashValue));
            // remember the serialized value
            serializedSegments[index].StartNew (WHICH_SLOT (hashValue), wayPointID, 1, location, bytesUsed);

            // now, go thru all of the attributes that are used
<//><//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>m4_third(_QC_))),</dnl
            if (myInBString.Overlaps(QueryIDSet(M4_QUERIES_AQ(_AQ_), true))){

                bytesUsed = M4_SERIALIZED_SIZE(M4_ATT_TYPE(M4_ATT_AQ(_AQ_)), M4_ATT_AQ(_AQ_));
                if (bytesUsed > storageSize) {
                    storageSize = bytesUsed;
                    free (serializeHere);
                    serializeHere = (void *) malloc (storageSize);
                }

                // and record the serialized value
                location =  M4_OPTIMIZED_SERIALIZE(M4_ATT_TYPE(M4_ATT_AQ(_AQ_)), M4_ATT_AQ(_AQ_), serializeHere);
                serializedSegments[index].Append (M4_ATT_SLOT(M4_ATT_AQ(_AQ_)), location, bytesUsed);
            }
<//><//>/>)dnl
        }
<//>/>)dnl
    }

    // now we are done serializing the chunk
    free (serializeHere);

    // so actually do the hashing... first set up the list of the guys we want to hash
    int theseAreOK [NUM_SEGS];
    for (int i = 0; i < NUM_SEGS; i++) {
        theseAreOK[i] = 1;
    }

    // this is the set of sample collisions taken from the over-full segments
    HashSegmentSample mySamples;

    // now go through and, one-at-a-time, add the data to each table segment
    for (int i = 0; i < NUM_SEGS; i++) {
        // first get a segment to add data to
        HashTableSegment checkedOutCopy;
        int whichOne = myWork.get_centralHashTable ().CheckOutOne (theseAreOK, checkedOutCopy);
        theseAreOK[whichOne] = 0;

        // now add the data
        HashSegmentSample mySample;
        if (checkedOutCopy.Insert (serializedSegments[whichOne], mySample)) {

            // if we are in here, it means that the segment was over-full, so note that we will
            // need to empty it out... we record all of the samples
            mySamples.MoveToFinish ();
            mySample.MoveToStart ();
            mySamples.SwapRights (mySample);
        }

        // and then put the segment back in the hash table
        myWork.get_centralHashTable ().CheckIn (whichOne);
    }

    PROFILING(start_time, "M4_WPName", "RHS_hash", "%d", totalNum);
    PROFILING(0.0, "HashTable", "fillrate", "%2.4f", HashTableSegment::globalFillRate*100.0);

    // now we are finally done!
    JoinHashResult myResult (mySamples);
    myResult.swap (result);
    return 0;

/*
Below code is commented, as per new changes after mergining of branch
    // now go through and, one-at-a-time, add the data to each table segment
    for (int i = 0; i < NUM_SEGS; i++) {
        HashTableSegment checkedOutCopy;
        int whichOne = myWork.get_centralHashTable ().CheckOutOne (theseAreOK, checkedOutCopy);
        theseAreOK[whichOne] = 0;
        checkedOutCopy.Insert (serializedSegments[whichOne]);
        myWork.get_centralHashTable ().CheckIn (whichOne);
    }


    // now we are finally done!
    return 0;
*/
}
