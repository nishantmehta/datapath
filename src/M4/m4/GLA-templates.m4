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
m4_include(lists.m4)dnl
m4_divert(-1)

dnl # macro to scan a file and extract info
dnl # $1=name of file without extension
m4_define(</SCAN_GLA_FILE/>, </dnl
m4_ifval($1,</dnl
m4_divert_push(-1)
m4_include(</$1.h/>)dnl
m4_divert_pop(-1)dnl
/>)dnl
/>)dnl

dnl # quoted definition macro
dnl # outputs a quoted m4_define statement defining the second argument as the
dnl # definition of the first argument
dnl # Note: the arguments will be evaluated
m4_define(</m4_qdefine/>,</dnl
<//>m4_quote(</m4_define(/>m4_dquote(m4_quote($1))</,/>m4_dquote(m4_quote($2))</)/>)dnl
<//>m4_define(m4_quote($1),m4_quote($2))dnl
/>)

m4_define(</m4_autoincr/>, </dnl
<//>m4_define(</$1/>, m4_incr(m4_defn(</$1/>)))dnl
/>)

dnl # Macros defining options for a description

m4_define(</_DESC_NAME/>, </dnl
<//></$0($@)/>dnl
<//>m4_redefine(</__META_NAME/>, </$1/>)dnl
/>)dnl

m4_define(</_DESC_INPUTS/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_INPUT/>, </$*/>)dnl
/>)dnl

m4_define(</_DESC_OUTPUTS/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_OUTPUT/>, </$*/>)dnl
/>)dnl

m4_define(</_DESC_RESULT_TYPE/>, </dnl
<//></$0($@)/>dnl
<//><//>m4_define(__META_TYPE</_REZTYPE_/>__META_NAME, </$1/>)dnl
<//><//>m4_define(__META_NAME</_REZTYPE/>, </$1/>)dnl
/>)dnl

m4_define(</_DESC_CONSTRUCTOR/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_INIT/>, </$*/>)dnl
/>)dnl

dnl # Macros to make it easier to add options to GLAs
m4_define(</_DESC_OPT_CHUNK_BOUNDARY/>, </dnl
<//></$0/>dnl
<//><//>m4_define(__META_TYPE</_CHUNKBOUNDARY_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_CHUNKBOUNDARY/>, <//>)dnl
/>)dnl

m4_define(</_DESC_OPT_ITERABLE/>, </dnl
<//></$0/>dnl
<//><//>m4_define(</GLA_ITERABLE_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_CONST_GEN/>, </(myConstState, />__META_NAME</_ConstState)/>)dnl
/>)dnl

m4_define(</_DESC_REQ_CONST_STATES/>, </dnl
<//></$0(</$*/>)/>dnl
<//>m4_define(__META_NAME</_CONST_REC/>, </$*/>)dnl
/>)dnl

m4_define(</_DESC_GEN_CONST_STATES/>, </dnl
<//></$0(</$*/>)/>dnl
<//>m4_define(__META_NAME</_CONST_GEN/>, </$*/>)dnl
/>)dnl

dnl # Designates that the GLA needs to have FinalizeState() called before being
dnl # directly used.
m4_define(</_DESC_OPT_FINALIZE_AS_STATE/>, </dnl
<//></$0/>dnl
<//><//>m4_define(</GLA_FINALIZE_AS_STATE_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_FINALIZE_AS_STATE/>, <//>)dnl
/>)dnl

dnl # GIST-specific options

dnl # type of the local scheduler
m4_define(</_DESC_LOCAL_SCHEDULER_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_LS_TYPE/>, </$1/>)dnl
/>)dnl

dnl # type of the GLA used by the GIST
m4_define(</_DESC_GLA_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_GLA_TYPE/>, </$1/>)dnl
/>)dnl

m4_define(</_DESC_TASK_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_TASK_TYPE/>, </$1/>)dnl
/>)dnl

dnl # Options for type descriptions.
m4_define(</_DESC_SIMPLE_TYPE/>, </dnl
<//></$0($@)/>dnl
<//>m4_if(__META_TYPE, </DATATYPE/>, <//>, </m4_fatal(</Can only declare types as simple!/>[__META_TYPE])/>)dnl
<//>m4_define(</DT_TYPE_/>__META_NAME, </DT_SIMPLE/>)dnl
m4_case($#, 0, </
<//>m4_define(</DT_ITERATOR_/>__META_NAME, </ColumnIterator< />__META_NAME</ >/>)dnl
/>, 1, </dnl
<//>m4_define(</DT_ITERATOR_/>__META_NAME, </$1/>)dnl
/>, </dnl
m4_fatal(</Can't give more than 1 argument to SIMPLE_TYPE/>)dnl
/>)dnl
/>)dnl

m4_define(</_DESC_COMPLEX_TYPE/>, </dnl
<//></$0($@)/>dnl
<//>m4_if(__META_TYPE, </DATATYPE/>, <//>, </m4_fatal(</Can only declare types as complex!/>)/>)dnl
<//>m4_define(</DT_TYPE_/>__META_NAME, </DT_COMPLEX/>)dnl
<//>m4_define(</DT_ITERATOR_/>__META_NAME, </$1/>)dnl
/>)dnl

dnl # For types that require a dictionary
m4_define(</_DESC_DICTIONARY/>, </dnl
<//></$0($1)/>dnl
<//>m4_define(__META_NAME</_DICT/>, </$1/>)dnl
/>)dnl

dnl # Macro for defining what libraries a GLA requires to be linked
m4_define(</_DESC_LIBS/>, </dnl
<//></$0($@)/>dnl
<//>m4_divert_push(8)dnl
<//>m4_foreach(</_LIB_/>, </$@/>, </dnl
<//><//>_LIB_
/>)dnl
<//>m4_divert_pop(8)dnl
/>)

dnl # here we define which options each type supports

dnl # used to set and unset options
m4_define(</__SET_OPTION/>, </dnl
<//>m4_pushdef(</$1/>, m4_defn(</_DESC_$1/>))dnl
/>)dnl

m4_define(</__UNSET_OPTION/>, </dnl
<//>m4_popdef(</$1/>)dnl
/>)dnl

m4_define(</__SET_GLA_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</OUTPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</OPT_CHUNK_BOUNDARY/>)dnl
<//>__SET_OPTION(</OPT_ITERABLE/>)dnl
<//>__SET_OPTION(</OPT_FINALIZE_AS_STATE/>)dnl
<//>__SET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__UNSET_GLA_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</OUTPUTS/>)dnl
<//>__UNSET_OPTION(</RESULT_TYPE/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</OPT_CHUNK_BOUNDARY/>)dnl
<//>__UNSET_OPTION(</OPT_ITERABLE/>)dnl
<//>__UNSET_OPTION(</OPT_FINALIZE_AS_STATE/>)dnl
<//>__UNSET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__SET_GF_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__UNSET_GF_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__SET_GT_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</OUTPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)

m4_define(</__UNSET_GT_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</OUTPUTS/>)dnl
<//>__UNSET_OPTION(</RESULT_TYPE/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)

m4_define(</__SET_GIST_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</OUTPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
<//>__SET_OPTION(</OPT_FINALIZE_AS_STATE/>)dnl
<//>__SET_OPTION(</LOCAL_SCHEDULER_TYPE/>)dnl
<//>__SET_OPTION(</GLA_TYPE/>)dnl
<//>__SET_OPTION(</TASK_TYPE/>)dnl
/>)dnl

m4_define(</__UNSET_GIST_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</OUTPUTS/>)dnl
<//>__UNSET_OPTION(</RESULT_TYPE/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</GEN_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
<//>__UNSET_OPTION(</OPT_FINALIZE_AS_STATE/>)dnl
<//>__UNSET_OPTION(</LOCAL_SCHEDULER_TYPE/>)dnl
<//>__UNSET_OPTION(</GLA_TYPE/>)dnl
<//>__UNSET_OPTION(</TASK_TYPE/>)dnl
/>)dnl

m4_define(</__SET_FUNC_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__UNSET_FUNC_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</RESULT_TYPE/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__SET_DATATYPE_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</SIMPLE_TYPE/>)dnl
<//>__SET_OPTION(</COMPLEX_TYPE/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
<//>__SET_OPTION(</DICTIONARY/>)dnl
/>)dnl

m4_define(</__UNSET_DATATYPE_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</SIMPLE_TYPE/>)dnl
<//>__UNSET_OPTION(</COMPLEX_TYPE/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
<//>__UNSET_OPTION(</DICTIONARY/>)dnl
/>)dnl

dnl # Macros to make it easier to specify meta-information about GLAs.
dnl # These macros also define aliases for the options they use.

m4_define(</GLA_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </GLA/>)dnl
<//>__SET_GLA_OPTIONS<//>dnl
/>)dnl

m4_define(</GF_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </GF/>)dnl
<//>__SET_GF_OPTIONS<//>dnl
/>)dnl

m4_define(</GT_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </GT/>)dnl
<//>__SET_GT_OPTIONS<//>dnl
/>)dnl

m4_define(</GIST_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </GIST/>)dnl
<//>__SET_GIST_OPTIONS<//>dnl
/>)dnl

m4_define(</FUNC_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </FUNC/>)dnl
<//>__SET_FUNC_OPTIONS<//>dnl
/>)dnl

m4_define(</OP_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </FUNC/>)dnl
<//>__SET_FUNC_OPTIONS<//>dnl
/>)dnl

m4_define(</TYPE_DESC/>, </dnl
<//></$0/>dnl
<//>m4_redefine(</__META_TYPE/>, </DATATYPE/>)dnl
<//>__SET_DATATYPE_OPTIONS<//>dnl
/>)dnl

m4_define(</END_DESC/>, </dnl
<//></$0/>dnl
<//>m4_ifndef(__META_NAME</_CONST_GEN/>, </m4_define(__META_NAME</_CONST_GEN/>, <//>)/>)<//>dnl
<//>m4_ifndef(__META_NAME</_CONST_REC/>, </m4_define(__META_NAME</_CONST_REC/>, <//>)/>)<//>dnl
<//>m4_define(__META_NAME</_CONST_STATE/>, m4_quote(GLUE_LISTS(m4_quote(m4_defn(__META_NAME</_CONST_GEN/>)), m4_quote(m4_defn(__META_NAME</_CONST_REC/>)))))<//>dnl
<//>reval(m4_defn(</__UNSET_/>__META_TYPE</_OPTIONS/>))<//>dnl
<//>m4_undefine(</__META_TYPE/>)dnl
<//>m4_undefine(</__META_NAME/>)dnl
/>)dnl

dnl # Macros used for generating descriptions of templates.
m4_define(</GLA_TEMPLATE_DESC/>, <//>)dnl
m4_define(</GF_TEMPLATE_DESC/>, <//>)dnl
m4_define(</GT_TEMPLATE_DESC/>, <//>)dnl
m4_define(</GIST_TEMPLATE_DESC/>, <//>)dnl
m4_define(</FUNC_TEMPLATE_DESC/>, <//>)dnl

m4_define(</TYPE_DEF/>, <//>)dnl
m4_define(</SYN_DEF/>, <//>)dnl
m4_define(</FUNC_DEF/>), <//>)dnl
m4_define(</OP_DEF/>, <//>)dnl

dnl # Used to help with Meta-states that have multiple inner states
dnl ##### M4_COLLECT_STATES ##### BEGIN
m4_define(</M4_COLLECT_STATES/>, </dnl
m4_divert_push(-1)

dnl # undefine output macros
m4_ifdef_undef(</__STATES/>)
m4_ifdef_undef(</__INPUTS/>)
m4_ifdef_undef(</__OUTPUTS/>)
m4_ifdef_undef(</__REQ_STATES/>)
m4_ifdef_undef(</__GEN_STATES/>)
m4_ifdef_undef(</__INIT/>)
m4_ifdef_undef(</__CONSTRUCTORS/>)
m4_ifdef_undef(</__REZTYPE/>) dnl # the last result type
m4_ifdef_undef(</__REZTYPES/>) dnl # a list of the result types

dnl # if this ends up being set, there are multiple result types
m4_ifdef_undef(</__DIFF_REZTYPES/>)

dnl # set up counters
m4_redefine(</_INVAL_/>, 0)
m4_redefine(</_OUTVAL_/>, 0)
m4_redefine(</_STATEVAL_/>, 0)
m4_redefine(</_CONSTVAL_/>, 0)
m4_redefine(</_INITVAL_/>, 0)

dnl # iterate through list
m4_foreach(</_G_/>, </$*/>, </
<//>_$0(_G_)
/>)

m4_define_ifndef(</__STATES/>, </__INPUTS/>, </__OUTPUTS/>, </__REQ_STATES/>, </__GEN_STATES/>, </__INIT/>, </__CONSTRUCTORS/>, </__REZTYPE/>, </__REZTYPES/>)

m4_divert_pop(-1)dnl
/>)dnl
dnl ##### M4_COLLECT_STATES ##### END

m4_define(</m4_define_ifndef/>, </dnl
<//>m4_if($#, 0, <//>, </dnl
<//>m4_if($1,,<//>, </dnl
<//><//>m4_ifndef(</$1/>, </m4_define(</$1/>)/>)dnl
<//><//>m4_define_ifndef(m4_shift($@))dnl
<//>/>)dnl
<//>/>)dnl
/>)dnl

dnl # Helper macro for M4_COLLECT_STATES
m4_define(</_M4_COLLECT_STATES/>, </dnl
m4_redefine(</__STATENAME__/>, </state_/>_STATEVAL_)
m4_autoincr(</_STATEVAL_/>)
m4_redefine(</__STATE__/>, </(/>__STATENAME__</, $1)/>)
m4_append(</__STATES/>, m4_quote(__STATE__), </</, />/>)
_M4_COLLECT_INPUTS(</$1/>, __STATENAME__)
_M4_COLLECT_OUTPUTS(</$1/>, __STATENAME__)
_M4_COLLECT_INITS(</$1/>, __STATENAME__)
_M4_COLLECT_GEN_STATES(</$1/>, __STATENAME__)
_M4_COLLECT_REQ_STATES(</$1/>, __STATENAME__)
_M4_COLLECT_CONST_STATES(</$1/>, __STATENAME__)
_M4_COLLECT_REZTYPES(</$1/>, __STATENAME__)
_M4_COLLECT_CONSTRUCTORS(</$1/>, __STATENAME__)
/>)dnl

dnl # General macro to rename the variables in a list and add them to another list.
dnl # Additionally, the renamed variables are also stored in a list with the name
dnl # <variable name>_T_<macro suffix>
dnl #
dnl # $1 = Variable Type
dnl # $2 = Variable Name
dnl # $3 = Source macro suffix (e.g., INPUT, OUTPUT, REZTYPE, etc.)
dnl # $4 = Destination list name
dnl # $5 = Name of counter
m4_define(</_M4_COLLECT_VARLIST/>, </dnl
<//>m4_ifdef_undef(</$2_T_$3/>)dnl
<//>m4_ifdef(</$1_$3/>, </dnl
<//>m4_ifval(m4_quote($1_$3), </dnl
<//>m4_foreach(</_A_/>, m4_quote($1_$3), </dnl
<//><//>m4_redefine(</__TMP__/>, m4_quote(</(/>m4_first(_A_)</_/>$5</, />m4_second(_A_)</)/>))<//>dnl
<//><//>m4_append(</$4/>, m4_quote(__TMP__), </</, />/>)<//>dnl
<//><//>m4_append(</$2_T_$3/>, m4_quote(__TMP__), </</, />/>)<//>dnl
<//><//>m4_autoincr(</$5/>)<//>dnl
<//>/>)dnl
<//>/>)dnl
<//>/>)dnl
<//>m4_ifndef(</$2_T_$3/>, </m4_define(</$2_T_$3/>)/>)dnl
/>)

dnl # All of the helper macros below have the same arguments:
dnl # $1 = The type of the state
dnl # $2 = The name of the variable used for the state

m4_define(</_M4_COLLECT_INPUTS/>, </dnl
<//>_M4_COLLECT_VARLIST(</$1/>, </$2/>, </INPUT/>, </__INPUTS/>, </_INVAL_/>)dnl
/>)

m4_define(</_M4_COLLECT_OUTPUTS/>, </dnl
<//>_M4_COLLECT_VARLIST(</$1/>, </$2/>, </OUTPUT/>, </__OUTPUTS/>, </_OUTVAL_/>)dnl
/>)

m4_define(</_M4_COLLECT_INITS/>, </dnl
<//>_M4_COLLECT_VARLIST(</$1/>, </$2/>, </INIT/>, </__INITS/>, </_INITVAL_/>)dnl
/>)

m4_define(</_M4_COLLECT_GEN_STATES/>, </dnl
<//>_M4_COLLECT_VARLIST(</$1/>, </$2/>, </CONST_GEN/>, </__GEN_STATES/>, </_CONSTVAL_/>)dnl
/>)

m4_define(</_M4_COLLECT_REQ_STATES/>, </dnl
<//>_M4_COLLECT_VARLIST(</$1/>, </$2/>, </CONST_REQ/>, </__REQ_STATES/>, </_CONSTVAL_/>)dnl
/>)

m4_define(</_M4_COLLECT_CONST_STATES/>, </dnl
m4_define(</$2_T_CONST_STATE/>, m4_quote(GLUE_LISTS(m4_quote($2_T_CONST_GEN), m4_quote($2_T_CONST_REQ))))
/>)dnl

m4_define(</_M4_COLLECT_REZTYPES/>, </dnl
<//>m4_ifdef_undef(</$2_T_REZTYPE/>)dnl
<//>m4_ifdef(</__REZTYPE/>, </dnl
<//><//>m4_if(__REZTYPE, m4_quote($1_REZTYPE), <//>, </m4_redefine(</__DIFF_REZTYPES/>, <//>)/>)dnl
/>, </dnl
<//><//>m4_define(</__REZTYPE/>, m4_quote($1_REZTYPE))dnl
<//>/>)dnl
<//>m4_append(</__REZTYPES/>, m4_quote($1_REZTYPE), </</, />/>)dnl
<//>m4_redefine(</$2_T_REZTYPE/>, m4_quote($1_REZTYPE))dnl
/>)

m4_define(</_M4_COLLECT_CONSTRUCTORS/>, </dnl
<//>m4_ifdef_undef(</$2_T_CONSTRUCTOR/>)dnl
<//>m4_ifval($2_T_CONST_STATE, </dnl
<//><//>m4_define(</$2_T_CONSTRUCTOR/>, m4_quote($2_T_CONST_STATE))
<//>/>, </dnl
<//><//>m4_define(</$2_T_CONSTRUCTOR/>, m4_quote($2_T_INIT))
<//>/>)dnl
<//>m4_ifval($2_T_CONSTRUCTOR, </dnl
<//><//>m4_append(</__CONSTRUCTORS/>, m4_quote($2_T_CONSTRUCTOR), </</, />/>)
<//>/>)
/>)dnl

dnl ##### Macros to assist the creation of container templates #####

dnl # Note: to use this macro, you must have collected a list of states
dnl # previously
dnl # $1 = class name
dnl # $2 = body of class
m4_define(</M4_CONTAINER_TEMPLATE/>, </dnl
m4_ifdef_undef(</_TMP_/>)dnl
m4_ifdef_undef(</_CS_VALS_/>)dnl

struct </$1_ConstState/> {
m4_foreach(</_S_/>, m4_quote(__STATES), </dnl
<//>m4_ifval(reval(VAR(_S_)</_T_CONST_STATE/>), </dnl
    // Generated constant states for VAR(_S_)
<//><//>m4_foreach(</_A_/>, m4_quote(reval(VAR(_S_)</_T_CONST_GEN/>)), </dnl
    const TYPE(_A_) VAR(_A_);
<//><//>m4_append(</_TMP_/>, m4_quote(VAR(_A_)</(/>ARGS(reval(VAR(_S_)</_T_INIT/>))</)/>), </</, />/>)dnl
<//><//>m4_append(</_CS_VALS_/>, m4_quote(_A_))dnl
<//><//>/>)dnl
<//>/>, </dnl
    // Constructor values for VAR(_S_)
<//><//>m4_foreach(</_A_/>, m4_quote(reval(VAR(_S_)</_T_INIT/>)), </dnl
    const TYPE(_A_) VAR(_A_);
<//><//>m4_append(</_TMP_/>, m4_quote(VAR(_A_)</(/>VAR(_A_)</)/>), </</, />/>)dnl
<//><//>m4_append(</_CS_VALS_/>, m4_quote(_A_))dnl
<//><//>/>)dnl
<//>/>)dnl
/>)dnl

    // Constructor
    MY_NAME</_ConstState/> ( TYPED_CONST_REF_ARGS(__INITS) ) m4_ifdef(</_TMP_/>, </:
        _TMP_
/>)dnl
    {}
};

class $1 {
    // Reference to generated state
    const </$1_ConstState/>& myConstState;

    // References to values in our constant state
m4_foreach(</_A_/>, m4_quote(__CONSTRUCTORS), </dnl
    const TYPE(_A_)& VAR(_A_);
/>)dnl

    // References to external constant states
m4_foreach(</_A_/>, m4_quote(__REQ_STATES), </dnl
    const TYPE(_A_)& VAR(_A_);
/>)dnl

    // Internal GFs
m4_foreach(</_S_/>, m4_quote(__STATES), </dnl
    TYPE(_S_) VAR(_S_);
/>)dnl

public:

    // Constructor
    $1 ( const </$1_ConstState/>& myConstState<//>m4_ifval(__REQ_STATES, </, TYPED_CONST_REF_ARGS(__REQ_STATES)/>) ) :
        myConstState(myConstState)
m4_foreach(</_A_/>, m4_quote(_CS_VALS_), </dnl
        , VAR(_A_) (myConstState.VAR(_A_))
/>)dnl
m4_foreach(</_A_/>, m4_quote(__REQ_STATES), </dnl
        , VAR(_A_) (VAR(_A_))
/>)dnl
m4_foreach(</_S_/>, m4_quote(__STATES), </dnl
        , VAR(_S_) ( ARGS(reval(VAR(_S_)</_T_CONSTRUCTOR/>)) )
/>)
    {}

    // Destructor
    ~$1 () {}

$2
};
/>)dnl

dnl # Put the BEGIN_LIBRARY comment in diversion 7 and END_LIBRARY in 9 so that
dnl # once m4 exits, the libraries in diversion 8 are commented out.
m4_divert_push(7)dnl
/* BEGIN_LIBRARIES
m4_divert_pop(7)dnl

m4_divert_push(9)dnl
END_LIBRARIES */
m4_divert_pop(9)dnl

m4_divert(0)dnl
