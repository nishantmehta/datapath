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
dnl # Macro to allow printing of </i/>
dnl $1=index variable
m4_define(</M4_INDEX/>,</dnl
<//>[$1]dnl
/>)

dnl #################################################################
dnl # Macros to help in finding information on attributes and queries

dnl # Macro to get the type of an attribute
dnl # $1=attribute
m4_define(</M4_ATT_TYPE/>,</ATT_TYPE_$1/>)

dnl # Macro to get the column type for an attribute
dnl # $1=attribute
dnl m4_define(</M4_COL_TYPE/>,</ColumnIterator<ATT_TYPE_$1>/>)
m4_define(</M4_COL_TYPE/>,</M4_ITERATOR_TYPE(ATT_TYPE_$1)/>)

dnl # Macro to get access to attribute data
dnl # $1=attribute
m4_define(</M4_ATT_DATA/>,</$1_Column/>)

dnl # This macro shoudl be used only inside other definitions
dnl # the double brackets are basically a hack
dnl # Macro to get access to attribute data in inner loop
dnl # $1=attribute
dnl # $2=index
m4_define(</M4_ATT_DATA__/>,</data_$1<//>[$2]/>)
dnl # same macro but useful outside definitions
m4_define(</M4_ATT_DATA_/>,</data_$1<//>[$2]/>)

dnl # Macro to get access to attribute column
dnl # $1=attribute
m4_define(</M4_ATT_COL/>,</col_$1/>)

dnl # Macro to get the slot of an attribute
dnl # $1=attribute
m4_define(</M4_ATT_SLOT/>,</ATT_SLOT_$1/>)


dnl # Macro to get id of queries
dnl # $1=query
m4_define(</M4_QUERY_ID/>,</QID_$1/>)

dnl # ALIN: The following two macros are superhacked
dnl # DO NOT TOUCH, EVER
dnl
dnl # Aux macro for M4_VAL_SUBST
dnl # Defines the val macro. Do not call it with parameters
m4_define(</M4_VAL_DEF/>,</dnl
M4_ATT_DATA($1)<//>[i]dnl
/>)
dnl
dnl # Macro to substitute expressions
dnl # The values of attributes are asumed to be marked by val(att)
dnl # to access the current tuple it is assumed that index i is used
m4_define(</M4_VAL_SUBST/>,</dnl
m4_pushdef(</in_att/>,m4_defn(</M4_VAL_DEF/>))$1<//>m4_popdef(</in_att/>)/>)

dnl # Aux macro for M4_REM_VAL
dnl # Defines the val macro. Do not call it with parameters
m4_define(</M4_VAL_IDENT/>,</dnl
$1<//>dnl
/>)

dnl # Macro to remove val()
m4_define(</M4_REM_VAL/>,</dnl
m4_pushdef(</val/>,m4_defn(</M4_VAL_IDENT/>))$1<//>m4_popdef(</val/>)/>)


dnl # macro to get the list of attributes for a query as a list
m4_define(</M4_ATT_LIST/>,</reval(m4_shift<//>$1)/>)

dnl # macro to get the list of attributes for a query as a list
dnl # this variant jumps over first 2 entries
m4_define(</M4_ATT_LIST2/>,</reval(M4_ATT_LIST((M4_ATT_LIST($1))))/>)

dnl # variation of the above to be used with pure lists guarded by ()
m4_define(</M4_ATT_LIST_/>,</reval(</m4_args/>m4_second($1))/>)


dnl # macro to get the attribute name from a Predicate or Aggregate list
m4_define(</M4_ATT_NAME/>,</m4_first($1)/>)

dnl # macro to get the name of a query
dnl # $1=(query, ...) as it appears in Predicates and Aggregate lists
m4_define(</M4_QUERY_NAME/>,</m4_first($1)/>)

dnl # macro to get the stateVariable for a query
m4_define(</M4_QUERY_STATE/>,</state<//>M4_QUERY_NAME($1)/>)

dnl # macro to get the state type for a query
dnl # depends on M4_WPName macro
m4_define(</M4_QUERY_STATE_TYPE/>,</M4_QUERY_NAME($1)<//>_<//>M4_WPName<//>_State/>)

dnl # macro to get the name of an aggregate
dnl # $1=(agg, type, ...) as it appears in Aggregate lists
m4_define(</M4_AGG_NAME/>,</m4_first($1)/>)

dnl # macro to get the state variable type for an aggregate
m4_define(</M4_AGG_STATE_TYPE/>,</m4_second($1)<//>State/>)
dnl # to get the name of the variable, we use M4_AGG_NAME

dnl # macro to get the expression for an aggregate in an Agglist
m4_define(</M4_AGG_EXPR/>,</m4_third($1)/>)

dnl # macro to get the list of expressions for print 
m4_define(</M4_PRINT_LIST/>,</reval(</m4_args/>m4_second($1))/>)

dnl ################################################################
dnl # THE FOLLWING MACROS DEAL WITH BUILDING THE SET OF ATTRIBUTES

dnl # the macro M4_CURR_SET indicatesthe set being built from val() expressions

dnl # macro to add the attribute to the current set
dnl # this macro is not used directly but val is redefined to it
m4_define(</M4_ADD_CURR_SET/>,</m4_set_add(M4_CURR_SET,</</$1/>/>)/>)

dnl # macro to extract the list of attributes from expressions
dnl # the macro is looking for val() to know when an attribute is used
dnl # $1=the set that is created
dnl # $2=the expression to be processed
m4_define(</M4_EXTRACT_ATT/>,</dnl
dnl # start a new set
m4_pushdef(</M4_CURR_SET/>,</$1/>)<//>dnl
dnl # redefine val so it adds elements to this set
m4_pushdef(</val/>,m4_defn(</M4_ADD_CURR_SET/>))<//>dnl
m4_divert(-1)$2<//>m4_divert(0)<//>dnl
m4_popdef(</val/>)<//>dnl
m4_popdef(</M4_CURR_SET/>)<//>dnl
/>)




dnl ################################################################
dnl # MACROS FOR JOINS

dnl # the M4_QUERY_NAME gets the name of the query from Joins

dnl # macro to get the copy list of the LHS
dnl # $1=the entry in Joins list
m4_define(</M4_JOIN_COPY_LHS/>, </reval(</m4_args/>m4_second($1))/>)

dnl # macro to get the hash of the RHS
dnl # $1=the entry in Joins list
m4_define(</M4_JOIN_HASH_RHS/>, </reval(</m4_args/>m4_first(m4_third($1)))/>)

dnl # macro to get the hash of the LHS
dnl # $1=the entry in Joins list
m4_define(</M4_JOIN_COPY_RHS/>, </reval(</m4_args/>m4_second(m4_third($1)))/>)

dnl # Macro to get access to output attribute data
dnl # $1=attribute
m4_define(</M4_ATT_OUT_DATA/>,</out_$1/>)

dnl # macro to access the tuple in LHS code
m4_define(</M4_ATT_TUPLE/>,</dnl
tuple[ATT_SLOT_$1]dnl
/>)

dnl # macro to form the macro name for a query set
dnl # it is formed from the HASH_RHS list
dnl # $1= the entry in the M4_Joins list for the query
m4_define(</M4_QUERY_PART_NAME/>, </dnl
dnl we use m4_append on the HASH_RHS
<//>m4_pushdef(</LOCAL_VAR/>, </M4_WPName<//>_QSet/>)<//>dnl
<//>m4_foreach(</_A_/>, M4_JOIN_HASH_RHS($1), </dnl
<//><//>m4_append(</LOCAL_VAR/>, _A_, </_/>)<//>dnl
<//>/>)<//>dnl
<//>LOCAL_VAR<//>dnl
m4_popdef(</LOCAL_VAR/>)<//>dnl
/>)

dnl # macro to inject code if argument is a valid attribute
dnl # this is needed to avoid empty elements in lists that
dnl # creep in because of empty arguments
dnl # $1=_A_
dnl # $2=action
m4_define(</M4_IFVALID_ATT/>, </dnl
<//>m4_ifdef(</ATT_SLOT_$1/>, </$2/>)
/>)

dnl # macro to go over lists of attributes skipping the undefined ones
dnl # $1= att set
dnl # $2= action (that uses _A_ to refer to attribute
m4_define(</M4_FOREACH_ATT/>, </dnl
<//>m4_set_foreach($1, </_A_/>, </dnl
<//><//>M4_IFVALID_ATT(_A_, </$2/>)
<//>/>)														
/>)

dnl # macro to allow unrestricted strings
dnl # this is the beggining of string (no args)		
dnl m4_define(</BString/>, </dnl
dnl <//>m4_changecom()<//>dnl
dnl />)

dnl m4_define(</EString/>, </dnl
dnl <//>m4_changecom(</#/>)<//>dnl										
dnl />)

dnl ####################################################################
dnl # DEBUGGING MACROS

dnl # Macro to print a set 
dnl # $1=set
m4_define(</M4_PRINT_SET/>,</dnl
dnl <//>m4_foreach(</_A_/>, </m4_set_list($1)/>, </_A_ |/>)<//>dnl
cucu
/>)
m4_divert(0)<//>dnl
