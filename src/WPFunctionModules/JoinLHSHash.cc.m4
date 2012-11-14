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


extern "C"
int JoinLHSHashWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {

    // M4_WPNam = M4_WPName
    // M4_LHS_Att = M4_LHS_Attr
    // M4_LHS_Has = M4_LHS_Hash
    // M4_Atrribute_Queries_LHS_cop = M4_Atrribute_Queries_LHS_copy

    double start_time = global_clock.GetTime();

    // this is the area where all of the intermediate, serialized records are stored
    SerializedSegmentArray serializedSegments [NUM_SEGS];

    // this is the area where all of the records are serialized to;
    // 10K bytes are initially used for this
    void *serializeHere = (void *) malloc (10000);
    int storageSize = 10000;

    // go to the work description and get the input chunk
    JoinLHSHashWorkDescription myWork;
    myWork.swap (workDescription);
    Chunk &input = myWork.get_chunkToProcess ();

    // get the waypoint identifier
    unsigned int wayPointID = myWork.get_wayPointID ();

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)<//>dnl

dnl M4_ACCESS_COLUMNS(m4_quote(reval(</m4_args/>M4_LHS_Attr)),</input/>)<//>dnl
<//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>M4_LHS_Attr)), </dnl
    QueryIDSet M4_ATT_AQ(_A_)<//>_Qrys(M4_QUERIES_AQ(_A_), true);
    M4_EXTRACT_COLUMN(M4_ATT_AQ(_A_),input)<//>dnl
<//>/>)<//>dnl

<//>M4_EXTRACT_BITMAP(</input/>)<//>dnl

    int totalNum = 0; // counter for the tuples processed

    // now actually hash all of the tuples!
    FOR_EACH_TUPLE(</input/>) {
        QueryIDSet qry;
<//><//>GET_QUERIES(qry)

dnl     // extract values of attributes from streams
<//><//>M4_ACCESS_ATTRIBUTES_TUPLE(m4_quote(reval(</m4_args/>M4_LHS_Attr)),queriesToRun)

        if (qry.IsEmpty()){
            continue;
        }

        totalNum++;

        HT_INDEX_TYPE hashValue = HASH_INIT;
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>M4_LHS_Hash)),</dnl
        hashValue = CongruentHash(Hash(_A_), hashValue);
<//><//>/>)<//>dnl

        // figure out which of the hash buckets it goes into
        unsigned int index = WHICH_SEGMENT (hashValue);

        // and serialize the record!  Begin with the bitstring.
        int bytesUsed = M4_SERIALIZED_SIZE(Bitstring, qry);

        // Make sure we have the storage...
        if (bytesUsed > storageSize) {
            storageSize = bytesUsed;
            free (serializeHere);
            serializeHere = (void *) malloc (storageSize);
        }

        // do the serialization...
        void *location = M4_OPTIMIZED_SERIALIZE(Bitstring, qry, serializeHere);

        // remember the serialized value
        serializedSegments[index].StartNew (WHICH_SLOT (hashValue), wayPointID, 0, location, bytesUsed);

        // now, go thru all of the attributes that are used
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>M4_LHS_Hash)),</dnl

        bytesUsed = M4_SERIALIZED_SIZE(M4_ATT_TYPE(_A_), _A_);
        if (bytesUsed > storageSize) {
            storageSize = bytesUsed;
            free (serializeHere);
            serializeHere = (void *) malloc (storageSize);
        }

        // and record the serialized value
        location =  M4_OPTIMIZED_SERIALIZE(M4_ATT_TYPE(_A_), _A_, serializeHere);
        serializedSegments[index].Append (M4_ATT_SLOT(_A_), location, bytesUsed);

<//><//>/>)<//>dnl
<//><//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)),</dnl
        if (qry.Overlaps(QueryIDSet(M4_QUERIES_AQ(_AQ_), true))){

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
<//><//>/>)<//>dnl

dnl // lastly, we need to advance in the INPUT tuples
dnl <//>m4_foreach( </_A_/>, m4_quote(reval(</m4_args/>M4_LHS_Attr)), </dnl
dnl <//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
dnl M4_ATT_DATA(M4_ATT_AQ(_A_)).Advance();
dnl <//>/>)<//>dnl
dnl <//>/>)<//>dnl

    } // for each tuple

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

    PROFILING(start_time, "M4_WPName", "LHS_hash", "%d", totalNum);
    PROFILING(0.0, "HashTable", "fillrate", "%2.4f", HashTableSegment::globalFillRate);

    // now we are finally done!
    JoinHashResult myResult (mySamples);
    myResult.swap (result);
    return 0;
}

