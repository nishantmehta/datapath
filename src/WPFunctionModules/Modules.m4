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
include(Resources-T.m4)
m4_include(CodeGenerationFunctions.m4)
m4_include(FileLoaderFunctions.m4)
m4_include(WPLib.m4)
m4_include(WPJoinLib.m4)
m4_divert(-1)
m4_include(Attributes.m4)
m4_include(DataTypes.m4)
m4_include(GLA-templates.m4)
m4_divert(-1)

m4_changecom()

dnl Definition of the preamble of generated code
dnl This macro loads the header files used by all the modules
dnl Include other headers needed for the specific modules in the module definition
dnl Alternativelly, this can be changed to always include only 
dnl once the modules needed
m4_define(</M4_CODE_GENERATION_PREAMBLE/>,</dnl
<//>m4_divert(0)dnl
#include "WorkDescription.h"
#include "ExecEngineData.h"
#include "Chunk.h"
#include "DataTypes.h"
#include "MMappedStorage.h"
#include "ColumnIterator.h"
#include "ColumnIterator.cc"
#include "BString.h"
#include "BStringIterator.h"
#include "HStringIterator.h"
#include "Constants.h"
#include <limits.h>
#include "QueryManager.h"
#include <string.h>
#include "Logging.h" // for profiling facility
#include "Profiling.h"
#include "WPFExitCodes.h"
<//>m4_divert(-1)
/>)
 
dnl Definition of the Selection Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, predicate, listSynth)
dnl    where listSynth is list of: (synAtt, expr)
dnl  $3 = List of attrbutes with QuerySets for them
dnl  $4 = List of attributes to drop from the chunk
m4_define(</M4_SELECTION_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_QueryDesc/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries/>, </$3/>)
dnl <//>m4_define(</M4_Dropped_Attributes/>,</$4/>
<//>m4_divert(0)
<//>m4_include(Selection.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_QueryDesc/>)
<//>m4_undefine(</M4_Attribute_Queries/>)
dnl <//>m4_undefine(</M4_Dropped_Attributes/>)
/>)


dnl Definition of the Selection Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, ListAttsToPrint)
dnl  $3 = List of attrbutes with QuerySets for them
m4_define(</M4_PRINT_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Print_List/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(Print.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Print_List/>)
<//>m4_undefine(</M4_Attribute_Queries/>)w
/>)


dnl Definition of the Aggregate Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, listAggregates)
dnl    where listAggregates is list of: (aggName, aggType, aggExpr)
dnl  $3 = List of attrbutes with QuerySets for them
m4_define(</M4_AGGREGATE_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Aggregates/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(Aggregate.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Aggregates/>)
<//>m4_undefine(</M4_Attribute_Queries/>)
/>)

dnl Definition of the GLA Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, listGLAdesc)
dnl    see file ... for description
dnl  $3 = List of attrbutes with QuerySets for them
m4_define(</M4_GLA_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_GLADesc/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(GLA.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_GLADesc/>)
<//>m4_undefine(</M4_Attribute_Queries/>)
/>)

dnl Definition of the GT Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, listGTdesc)
dnl    see file ... for description
dnl  $3 = List of attrbutes with QuerySets for them
m4_define(</M4_GT_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_GTDesc/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(GT.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_GTDesc/>)
<//>m4_undefine(</M4_Attribute_Queries/>)
/>)

dnl Definition of the GIST Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, each element contains (qname, listGISTdesc)
dnl    see file GIST.cc.m4 for description
m4_define(</M4_GIST_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_GISTDesc/>, </$2/>)
<//>m4_divert(0)
<//>m4_include(GIST.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_GISTDesc/>)
/>)

dnl Definition of the TextLoader Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Attribute list
dnl  $3 = separator
m4_define(</M4_TEXTLOADER_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Columns/>, </$2/>)
<//>m4_define(</M4_Separator/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(TextLoader.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Columns/>)
<//>m4_undefine(</M4_Separator/>)
/>)

dnl Definition of the Join Module
dnl  $1 = ID of the waypoint where it is going to be placed 
dnl  $2 = Query list, See JoinLHS.cc.m4 for full description
dnl  $3 = 
m4_define(</M4_JOINMERGE_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Attribute_Queries_LHS/>, </$2/>)
<//>m4_define(</M4_Attribute_Queries_RHS/>, </$3/>)
<//>m4_define(</M4_Attribute_Queries_LHS_Copy/>, </$4/>)
<//>m4_define(</M4_Attribute_Queries_RHS_Copy/>, </$5/>)
<//>m4_define(</M4_Queries_Attribute_Comparision/>, </$6/>)
<//>m4_divert(0)
<//>m4_include(JoinMergeWorkFuncs.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Attribute_Queries_LHS/>)
<//>m4_undefine(</M4_Attribute_Queries_RHS/>)
<//>m4_undefine(</M4_Attribute_Queries_LHS_Copy/>)
<//>m4_undefine(</M4_Attribute_Queries_RHS_Copy/>)
<//>m4_undefine(</M4_Queries_Attribute_Comparision/>)
/>)

m4_define(</M4_JOIN_RHS_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Attribute_Queries_RHS/>, </$2/>)
<//>m4_define(</M4_Query_Class_Hash/>, </$3/>)
<//>m4_divert(0)
<//>m4_include(JoinRHS.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Attribute_Queries_RHS/>)
<//>m4_undefine(</M4_Query_Class_Hash/>)
/>)

m4_define(</M4_JOIN_LHS_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_Attribute_Queries_LHS/>, </$2/>)
<//>m4_define(</M4_Atrribute_Queries_LHS_copy/>, </$3/>)
<//>m4_define(</M4_Atrribute_Queries_RHS_copy/>, </$4/>)
<//>m4_define(</M4_LHS_Hash/>, </$5/>)
<//>m4_define(</M4_Hash_RHS_ATTR/>, </$6/>)
<//>m4_define(</M4_Queries_Attribute_Comparision/>, </$7/>)
<//>m4_define(</M4_ExistsTarget/>, </$8/>)
<//>m4_define(</M4_NotExistsTarget/>, </$9/>)
<//>m4_divert(0)
<//>m4_include(JoinLHS.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_Attribute_Queries_LHS/>)
<//>m4_undefine(</M4_Atrribute_Queries_LHS_copy/>)
<//>m4_undefine(</M4_Atrribute_Queries_RHS_copy/>)
<//>m4_undefine(</M4_LHS_Hash/>)
<//>m4_undefine(</M4_Hash_RHS_ATTR/>)
<//>m4_undefine(</M4_Queries_Attribute_Comparision/>)
<//>m4_undefine(</M4_ExistsTarget/>)
<//>m4_undefine(</M4_NotExistsTarget/>)
/>)

dnl Definition of the Join Module
dnl  $1 = M4_WPName
dnl  $2 = M4_LHS_Hash
dnl  $3 = M4_Attribute_Queries_LHS
dnl  $4 = M4_Atrribute_Queries_LHS_copy
dnl  $5 = M4_Query_Class_Hash
dnl  $6 = M4_Attribute_Queries_RHS
dnl  $7 = M4_Atrribute_Queries_RHS_copy
dnl  $8 = M4_Hash_RHS_ATTR
dnl  $9 = M4_Queries_Attribute_Comparision
dnl  $10 = M4_ExistsTarget
dnl  $11 = M4_NotExistsTarget
m4_define(</M4_JOIN_MODULE/>, </dnl
<//>M4_JOIN_LHS_MODULE($1, $3, $4, $7, $2, $8, $9, $10, $11)
<//>M4_JOIN_RHS_MODULE($1, $6, $5)
dnl add JOIN_MERGE as well in future
/>)

m4_define(</M4_CLEANER_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_LHS_Attr/>, </$2/>)
<//>m4_define(</M4_RHS_Attr/>, </$3/>)
<//>m4_define(</M4_All_Queries/>, </$4/>)
<//>m4_divert(0)
<//>m4_include(Cleaner.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_LHS_Attr/>)
<//>m4_undefine(</M4_RHS_Attr/>)
<//>m4_undefine(</M4_All_Queries/>)
/>)

m4_define(</M4_JOINLHSHASH_MODULE/>, </dnl
<//>m4_define(</M4_WPName/>, </$1/>)
<//>m4_define(</M4_LHS_Attr/>, </$2/>)
<//>m4_define(</M4_LHS_Hash/>, </$3/>)
<//>m4_define(</M4_Atrribute_Queries_LHS_copy/>, </$4/>)
<//>m4_divert(0)
<//>m4_include(JoinLHSHash.cc.m4)
<//>m4_divert(-1)
<//>m4_undefine(</M4_WPName/>)
<//>m4_undefine(</M4_LHS_Attr/>)
<//>m4_undefine(</M4_LHS_Hash/>)
<//>m4_undefine(</M4_Atrribute_Queries_LHS_copy/>)
/>)
