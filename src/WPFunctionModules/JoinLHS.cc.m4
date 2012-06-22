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


dnl Arguments:
dnl
dnl M4_WPName -- name of the waypoint
dnl
dnl
dnl M4_Attribute_Queries_LHS -- the list  of queries in which each attribute is used
dnl                elements of the form: ( attribute, serialized_QueryIDSet )
dnl M4_Atrribute_Queries_LHS_copy -- same set but for copy attributes
dnl M4_Atrribute_Queries_RHS_copy
dnl
dnl M4_LHS_Hash -- list of hash attributes (similar to the RHS list in QueryClasses)

extern "C"
int JoinLHSWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {
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
    char *serializeHere = (char *) malloc (10000);

    // this is the output chunk
    Chunk output;

    // go to the work description and get the input chunk
    JoinLHSWorkDescription myWork;
    myWork.swap (workDescription);
    Chunk &input = myWork.get_chunkToProcess ();

    // get the waypoint ID from the chunk
    int wayPointID = myWork.get_wayPointID ();

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)dnl

    // get ready the input columns
<//>dnl SS Find some easy way to write qrys in here, not like JoinMerge way
<//>dnl M4_ACCESS_COLUMNS(</M4_Attribute_Queries_LHS/>,</input/>)dnl
<//>m4_foreach( </_A_/>, m4_quote(reval(</m4_args/>M4_Attribute_Queries_LHS)), </dnl
    QueryIDSet M4_ATT_AQ(_A_)_Qrys(M4_QUERIES_AQ(_A_), true);
<//>M4_EXTRACT_COLUMN(M4_ATT_AQ(_A_),input)dnl
<//>/>)dnl

<//>dnl SS M4_EXTRACT_BITMAP(</input/>)dnl
<//>dnl//Column bitmapIn;
<//>dnl//input.SwapBitmap (bitmapIn);
<//>dnl//BStringIterator myInBStringIter (bitmapIn);
    BStringIterator myInBStringIter;
    input.SwapBitmap (myInBStringIter);

    // start the iterators for the output columns for LHS; used only if stillShallow = 0
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)),</dnl
<//>dnl for attribute bla the iterator is bla_Out
    M4_COL_TYPE(m4_first(_AQ_)) m4_first(_AQ_)<//>_Column_Out;
<//>/>)dnl

    // these manage the output columns that come from the RHS (now stored in the hash table)
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_RHS_copy)),</dnl
<//>dnl for attribute bla the iterator is bla_Out
     MMappedStorage m4_first(_AQ_)<//>_Column_store;
    Column  m4_first(_AQ_)<//>_Column_Ocol(m4_first(_AQ_)<//>_Column_store);
    M4_COL_TYPE(m4_first(_AQ_)) M4_ATT_AQ(_AQ_)_Column_Out(M4_ATT_AQ(_AQ_)_Column_Ocol);
<//>/>)dnl

    // this is the ouput bitstring
    MMappedStorage myStore;
    Column bitmapOut (myStore);
    BStringIterator myOutBStringIter (bitmapOut, queriesToRun);

    // now we extract all of the hash table segments... after this, myEntries will hold them all
    HashTableView myView;
    myWork.get_centralHashTable ().EnterReader (myView);
    HashTableSegment myEntries[NUM_SEGS];
    myView.ExtractAllSegments (myEntries);

    // this tells us that we are "still shallow"---not making a deep copy of the LHS atts to the output
    int stillShallow = 1;

    // the bitstring that will be exracted from the hash table
    Bitstring64 *bitstringRHS = 0;

    // these are all of the attribute values that come from the hash table...
    // for each att we need a pointer as well as a dummy value that the pointer will be set to by default
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>M4_Hash_RHS_ATTR)),</dnl
    M4_ATT_TYPE(_A_) _A_<//>RHSShadow;
    M4_ATT_TYPE(_A_) *<//>_A_<//>RHS = NULL;
    M4_ATT_TYPE(_A_) _A_<//>RHSobj;
<//><//>/>)dnl

    // now actually try to match up all of the tuples!
    int totalNum = 0;
<//>dnl SS FOR_EACH_TUPLE(</input/>){
<//>dnl SS QueryIDSet qry;
<//>dnl    SS GET_QUERIES(qry);
    while (!myInBStringIter.AtEndOfColumn ()) { // TBD, probably this is not working TBD

        // counts how many matches for this query
        int numHits = 0;

        // extract values of attributes from streams
        // now go through the LHS input atts one at a time and extract if it is needed by an active query
<//><//>dnl SS M4_ACCESS_ATTRIBUTES_TUPLE(</M4_Attribute_Queries_LHS/>,queriesToRun)

        // see which queries match up
        Bitstring curBits = myInBStringIter.GetCurrent ();
        curBits.Intersect (queriesToRun);

        // if the input query is not empty
        if (!curBits.IsEmpty ()) {

            totalNum++;

            // compute the hash for LHS
            HT_INDEX_TYPE hashValue = HASH_INIT;
<//><//><//>dnl # hash all hashing attributes (use chain hashing)
<//><//><//>dnl SS Chain hashing is not provided, check TBD
<//><//>m4_foreach(</_A_/>,m4_quote(reval(</m4_args/>M4_LHS_Hash)),</dnl
dnl         //hashValue = Hash(M4_ATT_DATA(_A_).GetCurrent(), hashValue);
            hashValue = CongruentHash(Hash(_A_<//>_Column.GetCurrent()), hashValue);
<//><//>/>)dnl

            // figure out which of the hash buckets it goes into
            unsigned int index = WHICH_SEGMENT (hashValue);

            // now, go to that index and extract matching tuples!
            HT_INDEX_TYPE curSlot = WHICH_SLOT (hashValue);
            hashValue = curSlot;

            // this loops through all of the possible RHS hits
            while (1) {

                // this is the bitstring that will go in the output
                Bitstring bitstringLHS;
                bitstringLHS.Empty ();

                // for safety (in case we look at a bitstring that spans multiple
                // entries that is not done being written by a concurrent writer)
                // empty out the inital bitstring
                ((Bitstring *) serializeHere)->Empty ();

                // give safe "shadow" values to all of the RHS attributes
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>M4_Hash_RHS_ATTR)),</dnl
                _A_<//>RHS = &<//>_A_<//>RHSShadow;
<//><//>/>)dnl

                // here we go through and extract the atts one at a time from the hash
                // table.  Note that the atts must be extracted IN ORDER.  That is, the
                // bitstring is first, followed by the att mapped to the lowerest column
                // position, followed by the att mapped to the next lowest, and so on.

                // The Extract function pulls an attribute out of the hash table...
                int lenSoFar = 0, dummy, done;
                //    printf("\n  ====================================   LHS BITMAP hashbucketindex = %ld, hashvalue = %ld, curSlot = %ld ====================== \n", index, hashValue, curSlot);
                int lastLen = myEntries[index].Extract (serializeHere, curSlot, hashValue, wayPointID, BITMAP, dummy, done);

                // if we cannot find a bitstring, there was no tuple here, and we are done
                if (lastLen == 0) {
                  //cout << "\nEMMMMMMMMMMMMMMMMPPPPPPPPPPPPPPPPPPPPPPPPPTTTTTTTTTTTTTTTTTTTTTTTTYYYYYYYYYYYYYYYYYY\n";
                    break;
                }

                // remember the bitstring
                bitstringRHS = (Bitstring *) serializeHere;
                lenSoFar += lastLen;

                // next look for other hashed attributes
<//><//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>M4_Hash_RHS_ATTR)),</dnl
dnl             # SS Just created imaginary M4_Hash_RHS_ATTR to proceed
dnl             # SS TBD TBD void* used in arithemetic
                lastLen = myEntries[index].Extract (serializeHere + lenSoFar, curSlot, hashValue, wayPointID, M4_ATT_SLOT(_A_), dummy, done);

                // see if we got attribute
                if (lastLen > 0) {
dnl                 # _A_<//>RHS = ((M4_ATT_TYPE(_A_) *) (serializeHere + lenSoFar));
                    _A_<//>RHSobj = M4_OPTIMIZED_DESERIALIZE(M4_ATT_TYPE(_A_), _A_<//>RHSobj, serializeHere, lenSoFar);
                    _A_<//>RHS = &<//>_A_<//>RHSobj;
                    lenSoFar += lastLen;
                }
<//><//>/>)dnl

                // see if we have any query matches
                bitstringRHS->Intersect (curBits);
                Bitstring qBits;
                //printf("TPLLLLL: cust_acctbal = %f    orders_custkey = %d   cust_custkey = %d\n", *customer_c_acctbalRHS, orders_o_custkey_Column.GetCurrent(), *customer_c_custkeyRHS);

<//>m4_foreach( </_A_/>, </M4_Queries_Attribute_Comparision/>,</dnl
                // See if any query in query class is eligible for this comparision
                qBits = QueryIDSet(m4_first(_A_), true);
                qBits.Intersect(*bitstringRHS);
<//><//><//><//>m4_define(</list/>, </m4_second(_A_)/>)dnl
                if (
                    !qBits.IsEmpty () &&
<//>m4_foreach( </_B_/>, m4_quote(reval(</m4_args/>list)),</dnl
                     *<//>m4_second(_B_)<//>RHS == m4_first(_B_)<//>_Column.GetCurrent() &&
<//>/>)dnl
                     1 )
                {
                    bitstringLHS.Union (qBits);
                    //printf("TPL: %f\t%d\t%d\n", *customer_c_acctbalRHS, orders_o_custkey_Column.GetCurrent(), *customer_c_custkeyRHS);
                }
<//>/>)dnl

                // if any of them hit...
                if (!bitstringLHS.IsEmpty ()) {

                    numHits++;

                    // see if we need to move from shallow to deep
                    if (numHits == 2 && stillShallow) {

dnl                     # SS Query check is missing as was in original version
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)),</dnl
dnl                     # for attribute bla the iterator is bla_Out
                        M4_ATT_DATA(m4_first(_AQ_))<//>_Out.CreateDeepCopy (M4_ATT_DATA(m4_first(_AQ_)));
                        M4_ATT_DATA(m4_first(_AQ_))<//>_Out.Insert (M4_ATT_DATA(m4_first(_AQ_)).GetCurrent());
                        M4_ATT_DATA(m4_first(_AQ_))<//>_Out.Advance();

<//>/>)dnl
                        stillShallow = 0;
                    }

                    // now, add all of the outputs over... first deal with the LHS input atts
                    // that get copied into output atts
                    if (!stillShallow) {
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)),</dnl
dnl                     # SS Query check is missing
dnl                     # for attribute bla the iterator is bla_Out
                        M4_ATT_DATA(m4_first(_AQ_))<//>_Out.Insert (M4_ATT_DATA(m4_first(_AQ_)).GetCurrent());
                        M4_ATT_DATA(m4_first(_AQ_))<//>_Out.Advance();

<//>/>)dnl
                    }

                    // now, deal with the output atts that come from the hash table
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_RHS_copy)),</dnl
dnl                 # SS Query check is missing
                    M4_ATT_DATA(M4_ATT_AQ(_AQ_))_Out.Insert (*<//>M4_ATT_AQ(_AQ_)RHS);
                    M4_ATT_DATA(M4_ATT_AQ(_AQ_))_Out.Advance();

<//>/>)dnl
                    // finally, set the bitmap
                    myOutBStringIter.Insert (bitstringLHS);
                    myOutBStringIter.Advance ();
                }
            }
        }

        // at this point, we are done trying to join this tuple... any join results have been
        // written to the output columns.  Note that we don't have to advance in the output data
        // columns; if we are shallow, we don't touch the output columns.  If we are not shallow,
        // if there were no results, we have nothing to write.  HOWEVER, if we are shallow and
        // we did not get a match, we need to add an empty btstring
        if (stillShallow && numHits == 0) {
<//>m4_foreach(</_AQ_/>,m4_quote(reval(</m4_args/>M4_Atrribute_Queries_RHS_copy)),</dnl
dnl         # SS Query check is missing
            M4_ATT_TYPE(M4_ATT_AQ(_AQ_)) tmp_<//>M4_ATT_AQ(_AQ_);
            M4_ATT_DATA(M4_ATT_AQ(_AQ_))_Out.Insert (tmp_<//>M4_ATT_AQ(_AQ_));
            M4_ATT_DATA(M4_ATT_AQ(_AQ_))_Out.Advance();
<//>/>)dnl

            Bitstring bitstringLHS;
            bitstringLHS.Empty ();
            myOutBStringIter.Insert (bitstringLHS);
            myOutBStringIter.Advance ();
        }

        // lastly, we need to advance in the INPUT tuples
<//>m4_foreach( </_A_/>, m4_quote(reval(</m4_args/>M4_Attribute_Queries_LHS)), </dnl
M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        M4_ATT_DATA(M4_ATT_AQ(_A_)).Advance();
<//>/>)dnl
<//>/>)dnl

        // advance the input bitstring
        myInBStringIter.Advance ();
    }

    // DONE!  So construct the output tuple

    // if we are still shallow, put the original data into the output
    if (stillShallow) {
<//>m4_foreach( </_A_/>, m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)), </dnl
<//><//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        Column col_<//>M4_ATT_AQ(_A_);
        M4_ATT_DATA(M4_ATT_AQ(_A_)).Done(col_<//>M4_ATT_AQ(_A_));
        output.SwapColumn (col_<//>M4_ATT_AQ(_A_), M4_ATT_SLOT(M4_ATT_AQ(_A_)));
<//><//>/>)dnl
<//>/>)dnl
    } else {
<//>m4_foreach( </_A_/>, m4_quote(reval(</m4_args/>M4_Atrribute_Queries_LHS_copy)), </dnl
<//><//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        Column col_<//>M4_ATT_AQ(_A_);
        M4_ATT_DATA(M4_ATT_AQ(_A_))_Out.Done(col_<//>M4_ATT_AQ(_A_));
        output.SwapColumn (col_<//>M4_ATT_AQ(_A_), M4_ATT_SLOT(M4_ATT_AQ(_A_)));
<//><//>/>)dnl
<//>/>)dnl
    }

    {
<//>m4_foreach(</_A_/>, m4_quote(reval(</m4_args/>M4_Atrribute_Queries_RHS_copy)), </dnl
<//><//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        Column col_<//>M4_ATT_AQ(_A_);
        M4_ATT_DATA(M4_ATT_AQ(_A_))_Out.Done(col_<//>M4_ATT_AQ(_A_));
        output.SwapColumn (col_<//>M4_ATT_AQ(_A_), M4_ATT_SLOT(M4_ATT_AQ(_A_)));
<//><//>/>)dnl
<//>/>)dnl
    }

    // put in the output bitmap
dnl //myOutBStringIter.Done (bitmapOut);
    myOutBStringIter.Done ();
dnl //output.SwapBitmap (bitmapOut);
    output.SwapBitmap (myOutBStringIter);

    // and give back the result
    ChunkContainer tempResult (output);
    tempResult.swap (result);

    PROFILING(start_time, "M4_WPName", "LHS_lookup", "%d", totalNum);
    PROFILING(0.0, "HashTable", "fillrate", "%2.4f", HashTableSegment::globalFillRate*100.0);

    free (serializeHere);
    return 1;
}

