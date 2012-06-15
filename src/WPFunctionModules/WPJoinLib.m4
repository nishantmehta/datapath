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
dnl ####################################################################
dnl # NEW MACROS FOR CLEANING .cc.m4 code - Hanif

dnl $1 M4_LeftHash

m4_define(</M4_EXTRACT_LHS_HASH_ATTRIB/>, </dnl
dnl These sets are computed when the set of queries each attribute uses are determined
m4_set_delete(ATTS_ALL)dnl This will contain the list of attributes accessed from the LHS
m4_set_add_all(</ATTS_ALL/>, $1)dnl so we can access the hashing attributes
/>)

m4_define(</M4_DEFINE_QUERY_IDS_FOR_QUERIES/>, </dnl
m4_foreach(</_A_/>, </$1/>, </dnl
<//>m4_define(_A_<//>_Qrys_<//>$2)dnl
	QueryID _A_<//>_Qrys=queriesToRun;
/>)dnl 
/>)


m4_define(</M4_CLEAR_LOCAL_ATTS/>, </dnl
<//>m4_set_delete(IN_LOCAL_ATTS)dnl the columns in input for this query
<//>m4_set_delete(OUT_LOCAL_ATTS)dnl the columns in output for this query
/>)

m4_define(</M4_POPULATE_LOCAL_ATTS/>, </dnl
<//>m4_set_add_all(IN_LOCAL_ATTS, M4_JOIN_COPY_LHS($1))dnl
<//>m4_set_add_all(CHRIS_LOCAL_ATTS, M4_JOIN_COPY_LHS($1))dnl
<//>m4_set_add_all(OUT_LOCAL_ATTS, M4_JOIN_COPY_LHS($1), M4_JOIN_COPY_RHS($1))dnl
/>)


m4_define(</M4_POPULATE_RHS_ATTS/>, </dnl
<//>m4_set_add_all(RHS_ATTS, M4_JOIN_HASH_RHS($1), M4_JOIN_COPY_RHS($1))dnl
<//>m4_set_add_all(RHS_COPY_ATTS, M4_JOIN_COPY_RHS($1))dnl
/>)


m4_define(</M4_DECLARE_QUERYID_FOR_EACH_IN_ATT/>, </dnl

<//>m4_set_foreach(IN_LOCAL_ATTS,</_A_/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </dnl
<//><//><//>m4_ifndef(_A_<//>_Qrys_<//>$1,</dnl
<//><//><//><//><//>m4_define(_A_<//>_Qrys_<//>$1)dnl
	QueryID _A_<//>_Qrys; // encountered first time so declared
<//><//><//>/>)dnl define
  _A_<//>_Qrys.Union(M4_QUERY_NAME($2));
<//><//>/>)
<//>/>)dnl foreach on _A_

<//>m4_set_add_all(</ATTS_ALL/>, m4_set_list(IN_LOCAL_ATTS))dnl
/>)


m4_define(</M4_DECLARE_QUERYID_FOR_EACH_OUT_ATT/>, </dnl
<//>m4_set_foreach(OUT_LOCAL_ATTS,</_A_/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </dnl
<//><//><//>m4_ifndef(_A_<//>_QOut_<//>$1,</dnl
<//><//><//><//><//>m4_define(_A_<//>_QOut_<//>$1)dnl
	QueryID _A_<//>_QOut; // encountered first time so declared
<//><//><//>/>)dnl define
	_A_<//>_QOut.Union(M4_QUERY_NAME($2));
<//><//>/>)
<//>/>)dnl foreach on _A_
<//>m4_set_add_all(</OUT_ATTS/>, m4_set_list(OUT_LOCAL_ATTS))dnl
/>)


m4_define(</M4_DECLARE_ALLOCMEM_FOR_EACH_OUT_ATT/>, </dnl
//in
dnl # declaring and allocating memory of all output columns
m4_set_foreach(</ATTS_ALL/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
	M4_ATT_TYPE(_A_)* M4_ATT_OUT_DATA(_A_) = NULL;
<//>/>)dnl
/>)dnl

// out

m4_set_foreach(</RHS_COPY_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl  
	M4_ATT_TYPE(_A_)* M4_ATT_OUT_DATA(_A_) = NULL;
	if (_A_<//>_QOut.Overlaps(queriesToRun)){			
		M4_ATT_OUT_DATA(_A_) = (M4_ATT_TYPE(_A_) *) malloc (INIT_SIZE * sizeof (M4_ATT_TYPE(_A_)));
  	}
<//>/>)dnl
/>)dnl

/>)


m4_define(</M4_SET_OUTPUT_BITMAP/>, </dnl
QueryIDSet *outBitmap = (QueryIDSet *) malloc (sizeof (QueryIDSet) * INIT_SIZE);
/>)


m4_define(</M4_EMPTY_SLOT_IN_OUPUT_BITMAP/>, </dnl
outBitmap<//>M4_INDEX($1).Empty();
/>)


m4_define(</M4_IS_EMPTY_SLOT_IN_OUPUT_BITMAP/>, </dnl
outBitmap<//>M4_INDEX($1).IsEmpty()dnl
/>)


m4_define(</M4_ALLOCATE_SPACE_FOR_VARCHAR/>,</dnl
dnl  M4_ATT_TUPLE(_A_) = (VAL_TYPE *) malloc (MAXSTRINGLEN);
dnl	VARCHAR _A_<//>_tuple ((char*) M4_ATT_TUPLE(_A_));
	mySize = sizeof (VAL_TYPE) * ((int) ceil (sizeof (char *) / (double) sizeof (VAL_TYPE)));
	M4_ATT_TUPLE($1) = (VAL_TYPE *) malloc (mySize);
	M4_ATT_TYPE($1)& $1<//>_tuple = *((M4_ATT_TYPE($1)*) M4_ATT_TUPLE($1));
/>)

m4_define(</M4_ALLOCATE_SPACE_FOR_FIXEDTYPES/>,</dnl
	mySize = sizeof (VAL_TYPE) * ((int) ceil (sizeof (M4_ATT_TYPE($1)) / (double) sizeof (VAL_TYPE)));
	M4_ATT_TUPLE($1) = (VAL_TYPE *) malloc (mySize);
	M4_ATT_TYPE($1)& $1<//>_tuple = *((M4_ATT_TYPE($1)*) M4_ATT_TUPLE($1));
/>)



m4_define(</M4_ALLOCATE_SPACE_FOR_ATTRIB_RHS_TUPLES/>, </dnl

int mySize;
m4_set_foreach(</RHS_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
<//><//>m4_case(M4_ATT_TYPE(_A_), VARCHAR,</dnl definitin for VARCHAR---changed by Chris because now we just store the actual varchar in the table
	M4_ALLOCATE_SPACE_FOR_VARCHAR(</_A_/>)dnl
<//>/>, </dnl definition for fixed types
	M4_ALLOCATE_SPACE_FOR_FIXEDTYPES(</_A_/>)dnl	
<//><//>/>)dnl m4_case
<//>/>)dnl ifvalid
/>)dnl foreach
/>)


m4_define(</M4_COMPUTE_EXPRESSION/>, </dnl
	dnl we compute first the expression
	m4_pushdef(</XORS/>,)dnl
	m4_foreach(</_A_/>, </M4_LeftHash/>, </dnl
	<//>m4_append(</XORS/>,Hash(M4_ATT_DATA__(_A_,i)),</ ^ />)dnl
	/>)dnl
/>)


m4_define(</M4_DEFINE_HASH/>, </dnl
	M4_COMPUTE_EXPRESSION dnl
	HT_INDEX_TYPE ind = HASH_SEED<//>XORS;
	m4_popdef(</XORS/>)dnl
        HT_INDEX_TYPE hash = CongruentHash (ind);
/>)

m4_define(</M4_WRITE_JOINID_INTO_HASH/>, </dnl
	HT_INDEX_TYPE myMask = 32767;
	myMask = ~myMask;
	hash = hash & myMask;
	hash = hash | myJoinID;		
/>)

m4_define(</M4_DEFINE_LOOKUP_ENTRY/>, </dnl
	HT_INDEX_TYPE whichEntry = hash >> (sizeof (HT_INDEX_TYPE) * 8 - $1.numBits);
/>)

m4_define(</M4_ZERO_OUT_STRINGS/>, </dnl

m4_set_foreach(</RHS_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
<//><//>m4_if(M4_ATT_TYPE(_A_), VARCHAR,</dnl
		M4_ATT_TUPLE(_A_)</</0/>/> = 0;
<//><//>/>)dnl m4_if
<//>/>)dnl ifvalid
/>)dnl foreach
/>)

m4_define(</M4_IS_ENTRY_USED/>, </dnl
IS_USED($1.myEntries<//>M4_INDEX(whichEntry))
/>)

m4_define(</M4_CHECK_FOR_NEWTUPLE_CORRECT_HASH/>, </dnl
$1.myEntries<//>M4_INDEX(whichEntry).value == hash && 
				IS_NEW_TUPLE($1.myEntries<//>M4_INDEX(whichEntry))
/>)


m4_define(</M4_GET_OFFSET_TIL_NEXT/>, </dnl
GET_OFFSET_TIL_NEXT($1.myEntries<//>M4_INDEX($2))
/>)


m4_define(</M4_LOOKUP_ENTRY/>, </dnl
whichEntry dnl
/>)

m4_define(</M4_SET_TUPLE/>, </dnl
tuple<//>M4_INDEX($1)<//>M4_INDEX($2)dnl
/>)

m4_define(</M4_DEFINE_REZ_QUERYIDSET/>, </dnl
		QueryIDSet rez=queriesToRun;
		rez.Intersect(queries<//>M4_INDEX(i));
		rez.Intersect(*tupleBitstring);
/>)


m4_define(</M4_ACCESS_ALL_COLUMNS/>, </dnl
// Declaring and extracting all the columns that are needed
  int temp;
<//>dnl # declaring and extracting all the columns
<//>m4_set_foreach(</ATTS_ALL/>, </_A_/>, </dnl
	M4_EXTRACT_COLUMN(</_A_/>,$1)dnl
/>)dnl
/>)


m4_define(</M4_POPULATE_EQ_LIST/>, </dnl
<//>m4_ifdef(</EQ_LIST/>,</m4_undefine(</EQ_LIST/>)/>)dnl
<//>m4_pushdef(</TMP_LIST/>, </M4_JOIN_HASH_RHS($1)/>)dnl
<//>m4_foreach(</_A_/>, </M4_LeftHash/>, </dnl
// C
<//><//>m4_ifval(</TMP_LIST/>,, </m4_errprint(Not enough attributes in RHS hash of _Q_ )/>)dnl
// D
<//><//>m4_append(</EQ_LIST/>, M4_ATT_DATA__(_A_,i) == m4_car(TMP_LIST)<//>_tuple , && )dnl
<//><//>m4_pushdef(</TMP_LIST/>, m4_cdr(TMP_LIST))dnl
<//>/>)dnl
<//>m4_popdef(</TMP_LIST/>)dnl

/>)

m4_define(</M4_EQ_LIST/>, </dnl
EQ_LIST dnl
/>)

dnl $1 outslot
dnl $2 attribute
m4_define(</M4_SET_IN_OUT_BITMAP/>, </dnl
outBitmap<//>M4_INDEX($1).Union(M4_QUERY_NAME($2));
/>)

dnl $1 outslot
dnl $2 attribute
m4_define(</M4_WRITE_LHS/>, </dnl
<//>m4_foreach(</_A_/>, </M4_JOIN_COPY_LHS($2)/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </dnl
					M4_ATT_OUT_DATA(_A_)<//>M4_INDEX($1) = M4_ATT_DATA_(_A_,i);
<//>/>)dnl
/>)dnl
/>)

dnl $1 outslot
dnl $2 attribute
m4_define(</M4_WRITE_RHS/>, </dnl
<//>m4_foreach(</_A_/>, </M4_JOIN_COPY_RHS($2)/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </dnl
				M4_ATT_OUT_DATA(_A_)<//>M4_INDEX($1) = _A_<//>_tuple;
<//>/>)dnl
/>)dnl
/>)

dnl $1 curNumOutSlots
dnl $2 outslot
m4_define(</M4_WRITE_DEEP_LHS/>, </dnl
<//>m4_set_foreach(</CHRIS_LOCAL_ATTS/>, </_A_/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </dnl
				if (&(M4_ATT_DATA_(_A_,0)) != NULL) {
					// allocate the space
        	     			M4_ATT_OUT_DATA(_A_) = (M4_ATT_TYPE(_A_) *) malloc ($1 * sizeof (M4_ATT_TYPE(_A_)));
					// and copy all of the data over
					memmove (M4_ATT_OUT_DATA(_A_), &(M4_ATT_DATA_(_A_,0)), ($2) * sizeof (M4_ATT_TYPE(_A_)));
					M4_ATT_OUT_DATA(_A_)<//>M4_INDEX($2) = M4_ATT_DATA_(_A_,i);
				}
<//>/>)dnl
/>)dnl
/>)

dnl $1 curNumOutSlots
m4_define(</M4_RESIZE_RHS_COPY_ATTS/>, </dnl
m4_set_foreach(</RHS_COPY_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
					if (M4_ATT_OUT_DATA(_A_) != NULL)
						M4_ATT_OUT_DATA(_A_) = (M4_ATT_TYPE(_A_) *) realloc (M4_ATT_OUT_DATA(_A_), 2 * $1 * sizeof (M4_ATT_TYPE(_A_)));
<//>/>)dnl IFVALID
/>)dnl
/>)


dnl $1 curNumOutSlots
m4_define(</M4_RESIZE_OUT_ATTS/>, </dnl
m4_set_foreach(</OUT_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
					if (M4_ATT_OUT_DATA(_A_) != NULL)
						M4_ATT_OUT_DATA(_A_) = (M4_ATT_TYPE(_A_) *) realloc (M4_ATT_OUT_DATA(_A_), 2 * $1 * sizeof (M4_ATT_TYPE(_A_)));
<//>/>)dnl IFVALID
/>)dnl foreach
/>)


dnl $1 outslot
m4_define(</M4_ZERO_OUT_STRING_OUT_ATTS/>, </dnl
m4_set_foreach(</OUT_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
<//><//>m4_case(M4_ATT_TYPE(_A_),VARCHAR,</dnl definitin for VARCHAR
			if (M4_ATT_OUT_DATA(_A_) != NULL)
				M4_ATT_OUT_DATA(_A_)<//>M4_INDEX($1) = NULL;
<//><//>/>, <//>)dnl
<//>/>)dnl IFVALID
/>)dnl 
/>)

m4_define(</M4_DELETE_TEMP_RHS_ATTS/>, </dnl
m4_set_foreach(</RHS_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
	free (M4_ATT_TUPLE(_A_)); 
<//>/>)dnl IFVALID
/>)dnl
/>)

m4_define(</M4_FREE_TUPLES/>, </dnl
	free (tupleBitstring);
	free (tuple);	
/>)

dnl $1 outslot
m4_define(</M4_LOAD_OUTPUT/>, </dnl
	Bitmap myBitmap;	
	myBitmap.SetBits ($1, outBitmap);
	$2.SwapBitmap (myBitmap);
/>)

dnl $1 Chunk input
dnl $2 Chunk output
m4_define(</M4_SETUP_METADATA/>, </dnl
	ChunkID tempChunkID;
	tempChunkID=$1.GetChunkId();
	$2.SetChunkID (tempChunkID);
	QueryExitContainer myQueries;
	$1.RemoveQueries (myQueries);
	$2.AddQueries (myQueries);
/>)


dnl $1 outslot
dnl $2 Chunk output
m4_define(</M4_LOAD_RHS_COPY_ATTS/>, </dnl
m4_set_foreach(</RHS_COPY_ATTS/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
	Column _A_<//>_oCol;
  	_A_<//>_oCol.LoadColumnFixedLen(M4_ATT_OUT_DATA(_A_), $1);
  	$2.SwapColumn(_A_<//>_oCol, M4_ATT_SLOT(_A_));

<//>/>)dnl IFVALID
/>)dnl
/>)


dnl $1 outslot
dnl $2 Chunk output
m4_define(</M4_LOAD_ALL_ATTS/>, </dnl

m4_set_foreach(</ATTS_ALL/>, </_A_/>, </dnl
<//>M4_IFVALID_ATT(_A_, </dnl
	if (M4_ATT_OUT_DATA(_A_) == NULL) {
		Column _A_<//>_oCol;
		_A_<//>_oCol.copy (M4_ATT_COL(_A_));
  		$2.SwapColumn(_A_<//>_oCol, M4_ATT_SLOT(_A_));
	} else {
		Column _A_<//>_oCol;
<//><//>m4_case(M4_ATT_TYPE(_A_), VARCHAR,</dnl definitin for VARCHAR
		_A_<//>_oCol.LoadColumnVarLen(M4_ATT_OUT_DATA(_A_), $1);
<//><//>/>, </dnl definition for fixed types
  		_A_<//>_oCol.LoadColumnFixedLen(M4_ATT_OUT_DATA(_A_), $1);
<//><//>/>)dnl
		$2.SwapColumn(_A_<//>_oCol, M4_ATT_SLOT(_A_));				 
	}

<//>/>)dnl IFVALID
/>)dnl
/>)
m4_divert(0)dnl
