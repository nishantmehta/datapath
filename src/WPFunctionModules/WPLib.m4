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
m4_divert(-1)
dnl # macro to determine the class of a type
dnl # $1=type
m4_define(</M4_DT_TYPE/>,</dnl
m4_ifdef(</DT_TYPE_$1/>, </m4_defn(</DT_TYPE_$1/>)/>, </m4_fatal(</Type $1 cannot be serialized/>)/>)<//>dnl
/>)

dnl # Macro to determine the class of the type of an attribute
dnl # this is used by the serialize, deserialize code
dnl # $1 = attribute
m4_define(</M4_ATT_DT_TYPE/>,</M4_DT_TYPE(M4_ATT_TYPE($1))/>)

dnl # serialization/deserialization macros

dnl # macro to get the size of an object representing an attribute
dnl # $1=type of object
dnl # $2=object
m4_define(</M4_SERIALIZED_SIZE/>,</dnl
m4_case(M4_DT_TYPE($1),DT_SIMPLE,sizeof($1),DT_COMPLEX,$2.GetSize(),noSizeInfo)dnl
/>)dnl

dnl # macro to serialize an object in an optimal way
dnl # $1=type of object
dnl # $2=object
dnl # $3=buffer /* if cannot serialize in place */
m4_define(</M4_OPTIMIZED_SERIALIZE/>,</dnl
m4_case(M4_DT_TYPE($1),DT_SIMPLE,(void*)&($2),DT_COMPLEX,$2.OptimizedSerialize($2,$3),noSizeInfo)dnl
/>)dnl

dnl # macro to serialize an object in an optimal way
dnl # $1=type of object
dnl # $2=object
dnl # $3=buffer /* if cannot serialize in place */
dnl # $4=offset in buffer
m4_define(</M4_OPTIMIZED_DESERIALIZE/>,</dnl
m4_case(M4_DT_TYPE($1),DT_SIMPLE,*(($1 *) ($3 + $4)),DT_COMPLEX,$2.Deserialize($3 + $4),noSizeInfo)dnl
/>)dnl


dnl # Macro to extract the attribute from an Attribute_QueryIDs list
dnl # $1 = element
m4_define(</M4_ATT_AQ/>,</m4_first($1)/>)dnl

dnl # Macro to extract querys from Attribute_QueryIDs list element
dnl # $1 = element
m4_define(</M4_QUERIES_AQ/>,</m4_second($1)/>)dnl

dnl # Macro to define all query sets used
dnl # deals with both queries and attributess
dnl # $1 = M4_Query_Desc: Query desc for each query
dnl # $2 = M4_Attribute_Queries: Query set for each attribute used
m4_define(</M4_DECLARE_QUERYIDS/>, </dnl
    // get access to query manager
    QueryManager& qm=QueryManager::GetQueryManager();
    // set up the QueryIDs of the queries involved
<//>dnl # defining the query ids. The ids look like QID_queryName
<//>m4_foreach(</_Q_/>, </$1/>, </dnl
<//><//>M4_DEFINE_QUERYID(</_Q_/>,</qm/>)dnl
<//>/>)dnl

    // defining querysets for each attribute.
dnl <//>m4_foreach(</_A_/>, </$2/>, </dnl
dnl <//><//>M4_ATT_QUERYSET(</_A_/>,</qm/>)dnl
dnl <//>/>)dnl
/>)dnl


dnl # Macro to define constants used in the computation

dnl $1 = part in description
dnl $2 = QueryManager variable name
m4_define(</M4_DEFINE_QUERYID/>,</dnl
<//>m4_ifval( M4_QUERY_NAME($1), </dnl
    QueryID M4_QUERY_NAME($1)=</$2/>.GetQueryID("M4_QUERY_NAME($1)");
<//>/>, <//>)dnl
/>)dnl

dnl Macro to define the queryset for an attribute
dnl $1 = part in description
m4_define(</M4_ATT_QUERYSET/>,</dnl
<//>m4_ifval(M4_ATT_AQ($1),</dnl
<//>QueryIDSet M4_ATT_AQ($1)<//>_Qrys(M4_QUERIES_AQ($1), true);
<//>/>)dnl
/>)dnl


dnl  $1 = Work description
m4_define(</M4_GET_QUERIES_TO_RUN/>, </dnl
    QueryIDSet queriesToRun = QueryExitsToQueries(</$1/>.get_whichQueryExits ());
/>)dnl

m4_define(</M4_QUERIES_TO_RUN/>, </dnl
queriesToRun dnl
/>)dnl

dnl  $1 = attribute
dnl  $2 = Chunk input variable name
dnl  $3 = start fragment range
dnl  $4 = end fragment range
m4_define(</M4_EXTRACT_COLUMN_FRAGMENT/>, </dnl
            // extracting $1
<//>M4_IFVALID_ATT($1, </dnl

            Column M4_ATT_COL($1);

            if ($1<//>_Qrys.Overlaps(queriesToRun)){
                </$2/>.SwapColumn(M4_ATT_COL($1), M4_ATT_SLOT($1));
                if (! M4_ATT_COL($1).IsValid()){
                printf("ERROR: Column $1 not found in M4_WPName\n");
                exit(1);
                }
            }
            M4_COL_TYPE($1) M4_ATT_DATA($1) (M4_ATT_COL($1)/*, 8192*/, $3, $4);
<//>/>)dnl
/>)dnl


dnl  $1 = attribute
dnl  $2 = Chunk input variable name
m4_define(</M4_EXTRACT_COLUMN/>, </dnl
    // extracting $1
<//>M4_IFVALID_ATT($1, </dnl

    Column M4_ATT_COL($1);

    if ($1<//>_Qrys.Overlaps(queriesToRun)){
        </$2/>.SwapColumn(M4_ATT_COL($1), M4_ATT_SLOT($1));
        if (! M4_ATT_COL($1).IsValid()){
            printf("ERROR: Column $1 not found in M4_WPName\n");
            exit(1);
        }
    }
    M4_COL_TYPE($1) M4_ATT_DATA($1) (M4_ATT_COL($1)/*, 8192*/);
<//>/>)dnl
/>)dnl

dnl $1 = M4_Attribute_Queries
dnl  $2 = Chunk input variable name
m4_define(</M4_ACCESS_COLUMNS/>, </dnl
    // Declaring and extracting all the columns that are needed
<//>dnl # declaring and extracting all the columns
<//>m4_foreach( </_A_/>, </$1/>, </dnl
<//><//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
    QueryIDSet M4_ATT_AQ(_A_)_Qrys(M4_QUERIES_AQ(_A_), true);
<//>M4_EXTRACT_COLUMN(M4_ATT_AQ(_A_),$2)dnl
<//><//>/>)dnl
<//>/>)dnl
/>)dnl

dnl # macro that generates code to extract the existing bitmap
m4_define(</M4_EXTRACT_BITMAP/>, </dnl
    Column inBitCol;
    //SS </$1/>.SwapBitmap (inBitCol);
    //SS BStringIterator queries (inBitCol);
    BStringIterator queries;
    </$1/>.SwapBitmap (queries);
/>)dnl

dnl # macro to create the output bitmap
m4_define(</M4_START_OUTPUT_BITMAP/>,</dnl
    MMappedStorage bitStore;
    Column outBitCol(bitStore);
    BStringIterator outQueries (outBitCol, queriesToRun);
/>)dnl

dnl # macro to put the output bitmap into the chunk instead of the inbitmap
m4_define(</M4_PUTBACK_OUTBITMAP/>,</dnl
    // close both in and out iterators
    queries.Done(inBitCol);
    //SSoutQueries.Done(outBitCol);
    outQueries.Done();
    // the out one gets placed in chunk
    //SS</$1/>.SwapBitmap (outBitCol);
    </$1/>.SwapBitmap (outQueries);
/>)dnl

dnl # same macro but for input
m4_define(</M4_PUTBACK_INBITMAP/>,</dnl
    // close both in and out iterators
    //SSqueries.Done(inBitCol);
    queries.Done();
    //SS </$1/>.SwapBitmap (inBitCol);
    </$1/>.SwapBitmap (queries);
/>)dnl

dnl $1 = Chunk output
dnl This macro depends on the M4_DEFINE_OUT_BITMAP to define the output bitmap resBitMap
m4_define(</M4_SET_OUT_BITMAP/>, </dnl
</$1/>.SwapBitmap (resBitmap);
/>)dnl


m4_define(</FOR_EACH_TUPLE/>, </dnl
while (!queries.AtEndOfColumn ())dnl
/>)dnl

dnl # $1 is the variable that is set
m4_define(</GET_QUERIES/>, </dnl
        $1 = queries.GetCurrent();
        qry.Intersect(queriesToRun);
        queries.Advance();
/>)dnl

dnl # $1 is the variable that is set
m4_define(</GET_NEXT_QUERIES/>, </dnl
$1 = queries.GetCurrent();
        queries.Advance();
/>)dnl


m4_define(</SET_QUERIES/>, </dnl
        outQueries.Insert($1);
        outQueries.Advance();
/>)dnl

dnl $1 = M4_Attribute_Queries
dnl $2 = QueryIDSet variable
m4_define(</M4_ACCESS_ATTRIBUTES_TUPLE/>, </dnl
        // extract values of attributes from streams

<//>m4_foreach(</_A_/>, </$1/>, </dnl
<//>dnl access only valid attributes
<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        // extracting M4_ATT_AQ(_A_)
        const M4_ATT_TYPE(M4_ATT_AQ(_A_))& M4_ATT_AQ(_A_) =  M4_ATT_AQ(_A_)<//>_Column.GetCurrent();     
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl $1 = M4_Attribute_Queries
dnl $2 = QueryIDSet variable
m4_define(</M4_ADVANCE_ATTRIBUTES_TUPLE/>, </dnl
        // extract values of attributes from streams

<//>m4_foreach(</_A_/>, </$1/>, </dnl
<//>dnl access only valid attributes
<//>M4_IFVALID_ATT(M4_ATT_AQ(_A_), </dnl
        // advancing M4_ATT_AQ(_A_)
        M4_ATT_AQ(_A_)<//>_Column.Advance();
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl # Macro to remove val()
m4_define(</M4_EVAL_EXPR/>,</dnl
m4_pushdef(</in_att/>,m4_defn(</M4_VAL_IDENT/>))$1<//>m4_popdef(</in_att/>)dnl
/>)

dnl # extract the filter from $1
m4_define(</_SEL_PREDICATE/>, </dnl
<//>m4_second(</$1/>)dnl
/>)dnl

dnl # extract the initializer from $1
m4_define(</_SEL_INITIALIZER/>, </dnl
<//>m4_third(</$1/>)dnl
/>)dnl

dnl # extract the initializer from $1
m4_define(</_AGG_INITIALIZER/>, </dnl
<//>m4_second(</$1/>)<//>dnl
/>)dnl

dnl # extract the initializer from $1
m4_define(</_G_INITIALIZER/>, </dnl
<//>m4_seventh(</$1/>)<//>dnl
/>)dnl

m4_define(</_GLA_INITIALIZER/>, m4_defn(</_G_INITIALIZER/>))dnl

dnl # version of macro that includes file
m4_define(</GLA_TEMPLATE_INCLUDE/>,</dnl
INCLUDING AAA$1BBB
m4_divert(-1)dnl
m4_include(</$1/>)dnl
m4_divert(0)dnl
/>)

dnl # version of macro that ignores argument
m4_define(</GLA_TEMPLATE_IGNORE/>,<//>)

dnl # macro to interpret GLA template definitions
m4_define(</GLA_DEFINE_TEMPLATES/>,</
dnl # interpret the input with no output
m4_ifdef(</GLA_TEMPLATE/>,</m4_undefine(</GLA_TEMPLATE/>)/>)dnl
m4_define(</GLA_TEMPLATE/>, m4_defn(</GLA_TEMPLATE_INCLUDE/>))dnl
$1
dnl # redefine macro so it does nothing
m4_undefine(</GLA_TEMPLATE/>)dnl
m4_define(</GLA_TEMPLATE/>,m4_defn(</GLA_TEMPLATE_IGNORE/>))dnl
/>)dnl

m4_define(</GLA_DEF_ONE/>,</dnl
dnl # interpret the GLA definition
m4_divert(-1)dnl
GLA_DEFINE_TEMPLATES($1)dnl
m4_divert(0)dnl
dnl # expand the code
$1<//>dnl
/>)dnl

dnl # extract the initializer from $1
dnl # due to the templated GLAs, we need to do more manuevers
dnl # we process the argument twice, once to extract the template
dnl # definitions and once to generate the code
m4_define(</GLA_DEFS/>, </dnl
GLA_DEF_ONE(m4_third(</$1/>))dnl
/>)dnl


dnl # extract the initializer from $1
m4_define(</G_INIT_STATE/>, </dnl
<//>m4_fourth(</$1/>)<//>dnl
/>)dnl

m4_define(</GLA_INIT_STATE/>, m4_defn(</G_INIT_STATE/>))

dnl # form the name of the variable that has the GLA state based on query description.
m4_define(</G_EXPRESSION/>, </dnl
<//>m4_sixth(</$1/>)<//>dnl
/>)dnl

m4_define(</GLA_EXPRESSION/>, m4_defn(</G_EXPRESSION/>))

dnl # form the name of the variable that has the GLA state based on query description.
m4_define(</G_TYPE/>, </dnl
<//>m4_second(</$1/>)<//>dnl
/>)dnl

m4_define(</GLA_TYPE/>, m4_defn(</G_TYPE/>))

dnl # kind of GLA (single, multi, state)
m4_define(</G_KIND/>, </dnl
<//>reval(G_TYPE(</$1/>)</_REZTYPE/>)<//>dnl
/>)dnl

m4_define(</GLA_KIND/>, m4_defn(</G_KIND/>))

dnl # the output of the GLA
m4_define(</G_OUTPUTS/>, </dnl
<//>m4_fifth(</$1/>)<//>dnl
/>)dnl

m4_define(</GLA_OUTPUTS/>, m4_defn(</G_OUTPUTS/>))

dnl # the type of output container used by the GLA
m4_define(</GLA_OUTPUT_TYPE/>, </dnl
<//>m4_eighth(</$1/>)<//>dnl
/>)dnl

dnl # determine if a GLA is iterable
m4_define(</GLA_ITERABLE/>, </dnl
<//>m4_ifdef(</GLA_ITERABLE_/>GLA_TYPE(</$1/>), 1, 0)<//>dnl
/>)dnl

dnl # Whether or not a gla needs to be finalized when being returned as itself
m4_define(</GLA_FINALIZE_AS_STATE/>, </dnl
<//>m4_ifdef(</GLA_FINALIZE_AS_STATE_/>GLA_TYPE(</$1/>), 1, 0)<//>dnl
/>)dnl

dnl # M4 macros dealing with const states

dnl # determine if a GLA requires constant states
m4_define(</G_REQ_CONST_STATE/>, </dnl
<//>m4_ifval(m4_quote(m4_defn(G_TYPE(</$1/>)</_CONST_STATE/>)), 1, 0)<//>dnl
/>)dnl

m4_define(</GLA_REQ_CONST_STATE/>, m4_defn(</G_REQ_CONST_STATE/>))

dnl # the list of const states required by the GLA
m4_define(</G_CONST_STATES/>, </dnl
<//>reval(G_TYPE(</$1/>)</_CONST_STATE/>)<//>dnl
/>)dnl

m4_define(</GLA_CONST_STATES/>, m4_defn(</G_CONST_STATES/>))

dnl # the list of const states that should be generated by the waypoint
m4_define(</G_CONST_GENERATED/>, </dnl
<//>reval(G_TYPE(</$1/>)</_CONST_GEN/>)<//>dnl
/>)dnl

m4_define(</GLA_CONST_GENERATED/>, m4_defn(</G_CONST_GENERATED/>))

dnl # the list of const states that are received from other waypoints
m4_define(</G_CONST_RECEIVE/>, </dnl
<//>reval(G_TYPE(</$1/>)</_CONST_REC/>)<//>dnl
/>)dnl

m4_define(</GLA_CONST_RECEIVE/>, m4_defn(</G_CONST_RECEIVE/>))

dnl # form the name of the variable that has the GLA state based on query description.
m4_define(</G_STATE/>, </dnl
<//>G_TYPE(</$1/>)_<//>M4_QUERY_NAME(</$1/>)<//>dnl
/>)dnl

m4_define(</GLA_STATE/>, m4_defn(</G_STATE/>))



dnl $1 = WPName
dnl $2 = Chunk input variable name
m4_define(</M4_PUTBACK_COLUMNS/>, </dnl
<//>m4_foreach(</_A_/>, </$1/>, </dnl
<//><//>m4_ifval( M4_ATT_AQ(_A_), </dnl
    // putting back column of M4_ATT_AQ(_A_)
    if (M4_ATT_AQ(_A_)<//>_Qrys.Overlaps(queriesToRun)){
        M4_ATT_AQ(_A_)<//>_Column.Done(col_<//>M4_ATT_AQ(_A_));
        </$2/>.SwapColumn(M4_ATT_COL(M4_ATT_AQ(_A_)), M4_ATT_SLOT(M4_ATT_AQ(_A_)));
    }
<//><//>/>)dnl
<//>/>)dnl
/>)dnl


m4_define(</M4_ADD_ITEM_TO_STATE/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_ATT_LIST((M4_ATT_LIST(</$1/>)))), </dnl
<//><//>M4_QUERY_STATE($1).M4_AGG_NAME(_A_).AddItem(M4_VAL_SUBST(M4_AGG_EXPR(_A_)));
<//>/>)dnl
/>)dnl

m4_define(</M4_DEFINE_STATES/>,</dnl
  // create the states updated by the aggregation
<//>m4_foreach(</_Q_/>, </$1/>, </dnl
<//><//>// declaring state of M4_QUERY_NAME(_Q_)
<//><//>struct M4_QUERY_STATE_TYPE(_Q_) M4_QUERY_STATE(_Q_);
<//>/>)
/>)dnl

dnl $1 = Each Query Desc
dnl $2 = AggStorageMap name
m4_define(</M4_ADD_OLD_STATE_TO_NEW/>,</dnl
// we have, so get the data
<//>QueryID foo;
<//>AggStorage res;
<//></$2/>.Remove (M4_QUERY_NAME($1), foo, res);
<//>M4_QUERY_NAME($1)=foo; // not to loose M4_QUERY_NAME($1)

<//>struct M4_QUERY_STATE_TYPE($1) M4_QUERY_STATE($1)<//>Old;
<//>res.Pop(&M4_QUERY_STATE($1)<//>Old);
// now add the old state to the new state
<//>m4_foreach(</_A_/>, m4_quote(M4_ATT_LIST((M4_ATT_LIST(</$1/>)))), </dnl
<//><//>M4_QUERY_STATE($1).M4_AGG_NAME(_A_).AddState(M4_QUERY_STATE($1)<//>Old.M4_AGG_NAME(_A_));
/>)dnl
/>)dnl

dnl $1 = Each Query Desc
dnl $2 = AggStorageMap name
m4_define(</M4_ADD_NEW_RESULT_TO_MAP/>,</dnl
// put the new result in aggFuncs
<//>AggStorage M4_QUERY_NAME($1)<//>St;
<//>M4_QUERY_NAME($1)<//>St.Push(&M4_QUERY_STATE($1), sizeof(M4_QUERY_STATE($1)));
<//></$2/>.Insert(M4_QUERY_NAME($1), M4_QUERY_NAME($1)<//>St);
/>)dnl

dnl $1 = All Query Descriptions
dnl $2 = AggStorageMap name
m4_define(</M4_ADD_AGGR_INFO_TO_MAP/>,</dnl
<//>m4_foreach(</_Q_/>, </$1/>, </dnl
<//>// do M4_QUERY_NAME(_Q_)
<//>M4_IF_QUERY_IN_MAP(</_Q_/>,</$2/>)
<//>{
<//><//>M4_ADD_OLD_STATE_TO_NEW(</_Q_/>,</$2/>)dnl
<//>}
<//>M4_ADD_NEW_RESULT_TO_MAP(</_Q_/>,</$2/>)dnl
<//>/>)dnl
/>)dnl

m4_define(</M4_DROP_ATTS/>,</dnl
  // dropping the columns we do not need. We simply swap them out and not put them in
<//>m4_ifset(</$1/>, m4_foreach(</_A_/>, </$1/>,</dnl
<//><//>// extracting _A_
<//><//>Column M4_ATT_COL(_A_)<//>_Drop;
<//><//>input.SwapColumn(M4_ATT_COL(_A_)<//>_Drop, M4_ATT_SLOT(_A_));
<//><//>/>),</  // nothing to drop/>)dnl
/>)dnl

dnl $1 = Chunk &output
dnl $2 = QueryExitContainer &addToOutChunk
dnl $3 = TableScanID &addMeTo
m4_define(</M4_ADD_METADATA_OUTPUTCHUNK/>,</dnl
  // empty out the output chunk
  Chunk empty;
  empty.swap (</$1/>);

  // add the metadata to it
  </$1/>.AddQueries (</$2/>);

  ChunkID ALL (</$3/>);
  </$1/>.SetChunkID (ALL);
/>)dnl

dnl $1 = AggStorageMap &aggFuncs
dnl $2 = Each Query desc
m4_define(</M4_GET_QUERY_DATA/>, </dnl
    QueryID foo;
    AggStorage res;
    </$1/>.Remove (M4_QUERY_NAME($2), foo, res);
    M4_QUERY_NAME($2)=foo; // not to loose M4_QUERY_NAME(_Q_)

    struct M4_QUERY_STATE_TYPE($2) M4_QUERY_STATE($2);
    res.Pop(&M4_QUERY_STATE($2));
/>)dnl

dnl $1 = Aggregate expression
dnl $2 = Query desc
m4_define(</M4_COMPUTE_AGGEGATE/>,</dnl
            MMappedStorage storage;
            Column col(storage);
            ColumnIterator<M4_ATT_TYPE(M4_ATT_NAME($1))> colIter(col);
            M4_ATT_TYPE(M4_ATT_NAME($1)) val = M4_QUERY_STATE($2).M4_AGG_NAME($1).ComputeAggregate();
            colIter.Insert(val);
            colIter.Advance();
            colIter.Done(col);
/>)dnl

dnl $1 = Aggregate expression
dnl This depends on the M4_COMPUTE_AGGEGATE macro for defining the Column col
m4_define(</M4_PUT_RESULT_IN_OUTPUT/>,</dnl
     </$1/>.SwapColumn (col, M4_ATT_SLOT(M4_ATT_NAME(</$2/>)));
/>)dnl

dnl $1 = Query Desc
m4_define(</M4_COMPUTE_AGGREGATE/>,</dnl
<//>// now get each aggregate computes and put it in the output chunk
<//>m4_foreach(</_A_/>, m4_quote(M4_ATT_LIST((M4_ATT_LIST(</$1/>)))), </dnl
<//><//>{
<//><//><//>M4_COMPUTE_AGGEGATE(</_A_/>,</$1/>)dnl
<//><//><//>M4_PUT_RESULT_IN_OUTPUT(</output/>,</_A_/>)dnl
<//><//>}
<//>/>)dnl
/>)dnl

dnl # variation of the above to be used with pure lists guarded by ()
m4_define(</M4_SYNTH_LIST/>,</reval(</m4_args/>m4_fourth($1))/>)

dnl # extract variable name
m4_define(</M4_SYNTH_VAR/>, </m4_first($1)/>)

dnl # extract variable expression
m4_define(</M4_SYNTH_EXPR/>, </m4_second($1)/>)

dnl macro to declare local variables for synthesized
dnl $1 = Query Description
m4_define(</M4_DECLARE_SYNTHESIZED/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_SYNTH_LIST(</$1/>)), </dnl
<//><//>m4_ifval(_A_,</dnl
        M4_ATT_TYPE(m4_first(_A_)) m4_first(_A_);
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl macro to compute the synthesized attributes
dnl $1 = Query Description
m4_define(</M4_COMPUTE_SYNTHESIZED/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_SYNTH_LIST(</$1/>)), </dnl
<//><//>m4_ifval(_A_,</dnl
            m4_first(_A_) = m4_second(_A_);
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl macro to create sysntesized columns
dnl $1 = Query Description
m4_define(</M4_CREATE_SYNTHESIZED/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_SYNTH_LIST(</$1/>)), </dnl
<//>m4_ifval(_A_,</dnl
    MMappedStorage storage_<//>M4_SYNTH_VAR(_A_);
    Column col_<//>M4_SYNTH_VAR(_A_)(storage_<//>M4_SYNTH_VAR(_A_));
    M4_COL_TYPE(M4_SYNTH_VAR(_A_)) colI_<//>M4_SYNTH_VAR(_A_)(col_<//>M4_SYNTH_VAR(_A_));
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl macro to create sysntesized columns
dnl $1 = Query Description
dnl $2 = output chunk
dnl $3 = queries active
m4_define(</M4_CLOSE_SYNTHESIZED/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_SYNTH_LIST(</$1/>)), </dnl
<//><//>m4_ifval(_A_,</dnl
        colI_<//>m4_first(_A_)<//>.Done(col_<//>m4_first(_A_));
        </$2/>.SwapColumn(col_<//>m4_first(_A_), M4_ATT_SLOT(m4_first(_A_)));
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl macro to write local variables for synthesized to columns
dnl $1 = Query Description
m4_define(</M4_WRITE_SYNTHESIZED/>,</dnl
<//>m4_foreach(</_A_/>, m4_quote(M4_SYNTH_LIST(</$1/>)), </dnl
<//><//>m4_ifval(_A_,</dnl
        colI_<//>m4_first(_A_).Insert(m4_first(_A_));
        colI_<//>m4_first(_A_).Advance();
<//>/>)dnl
<//>/>)dnl
/>)dnl


m4_define(</M4_DEFINE_OUT_BITMAP/>, </dnl
  Bitmap resBitmap (1);
  int dummy;
  Bitstring *outString = resBitmap.GetBits (dummy);
  outString->Empty();
/>)dnl

dnl $1 = Query Desc
dnl This macro depends on the M4_DEFINE_OUT_BITMAP to define the output bitstring outString
m4_define(</M4_MARK_QUERY_IN_OUT_BITMAP/>, </dnl
outString->Union(M4_QUERY_NAME($1));
/>)dnl


dnl $1 = QueryDesc
m4_define(</M4_DEFINE_AGG_STATES/>, </dnl
<//>m4_foreach(</_Q_/>, </$1/>, </dnl
<//>// defining struct M4_QUERY_STATE_TYPE(_Q_)
<//>struct M4_QUERY_STATE_TYPE(_Q_){
<//><//>m4_foreach(</_A_/>, m4_quote(M4_ATT_LIST((M4_ATT_LIST(</_Q_/>)))), </dnl
<//><//>  M4_AGG_STATE_TYPE(_A_) M4_AGG_NAME(_A_);
<//><//>/>)dnl
<//>};
<//>/>)dnl
/>)dnl
m4_divert(0)dnl
