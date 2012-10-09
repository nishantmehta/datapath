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

m4_define(</__NAME/>, </dnl
<//></$0($@)/>dnl
<//>m4_redefine(</__META_NAME/>, </$1/>)dnl
/>)dnl

m4_define(</__INPUTS/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_INPUT/>, </$*/>)dnl
/>)dnl

m4_define(</__OUTPUTS/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_OUTPUT/>, </$*/>)dnl
/>)dnl

m4_define(</__RESULT_TYPE/>, </dnl
<//></$0($@)/>dnl
<//><//>m4_define(__META_TYPE</_REZTYPE_/>__META_NAME, </$1/>)dnl
<//><//>m4_define(__META_NAME</_REZTYPE/>, </$1/>)dnl
/>)dnl

m4_define(</__CONSTRUCTOR/>, </dnl
<//></$0(</$*/>)/>dnl
<//><//>m4_define(__META_NAME</_INIT/>, </$*/>)dnl
/>)dnl

dnl # Macros to make it easier to add options to GLAs
m4_define(</__OPT_CHUNK_BOUNDARY/>, </dnl
<//></$0/>dnl
<//><//>m4_define(__META_TYPE</_CHUNKBOUNDARY_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_CHUNKBOUNDARY/>, <//>)dnl
/>)dnl

m4_define(</__OPT_ITERABLE/>, </dnl
<//></$0/>dnl
<//><//>m4_define(</GLA_ITERABLE_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_CONST_GEN/>, </(myConstState, />__META_NAME</_ConstState)/>)dnl
/>)dnl

m4_define(</__REQ_CONST_STATES/>, </dnl
<//></$0(</$*/>)/>dnl
<//>m4_define(__META_NAME</_CONST_REC/>, </$*/>)dnl
/>)dnl

dnl # Designates that the GLA needs to have FinalizeState() called before being
dnl # directly used.
m4_define(</__OPT_FINALIZE_AS_STATE/>, </dnl
<//></$0/>dnl
<//><//>m4_define(</GLA_FINALIZE_AS_STATE_/>__META_NAME, <//>)dnl
<//><//>m4_define(__META_NAME</_FINALIZE_AS_STATE/>, <//>)dnl
/>)dnl

dnl # GIST-specific options

dnl # type of the local scheduler
m4_define(</__LOCAL_SCHEDULER_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_LS_TYPE/>, </$1/>)dnl
/>)dnl

dnl # type of the GLA used by the GIST
m4_define(</__GLA_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_GLA_TYPE/>, </$1/>)dnl
/>)dnl

m4_define(</__TASK_TYPE/>, </dnl
<//></$0(</$1/>)/>dnl
<//>m4_define(__META_NAME</_TASK_TYPE/>, </$1/>)dnl
/>)dnl

dnl # Options for type descriptions.
m4_define(</__SIMPLE_TYPE/>, </dnl
<//></$0/>dnl
<//>m4_if(__META_TYPE, </DATATYPE/>, <//>, </m4_fatal(</Can only declare types as simple!/>[__META_TYPE])/>)dnl
<//>m4_define(</DT_TYPE_/>__META_NAME, </DT_SIMPLE/>)dnl
<//>m4_define(</DT_ITERATOR_/>__META_NAME, </ColumnIterator< />__META_NAME</ >/>)dnl
/>)dnl

m4_define(</__COMPLEX_TYPE/>, </dnl
<//></$0($@)/>dnl
<//>m4_if(__META_TYPE, </DATATYPE/>, <//>, </m4_fatal(</Can only declare types as complex!/>)/>)dnl
<//>m4_define(</DT_TYPE_/>__META_NAME, </DT_COMPLEX/>)dnl
<//>m4_define(</DT_ITERATOR_/>__META_NAME, </$1/>)dnl
/>)dnl

dnl # Macro for defining what libraries a GLA requires to be linked
m4_define(</__LIBS/>, </dnl
<//></$0($@)/>dnl
<//>m4_divert_push(8)dnl
<//>m4_foreach(</_LIB_/>, </$@/>, </dnl
<//><//>_LIB_
/>)dnl
<//>m4_divert_pop(8)dnl
/>)

dnl # here we define which options each type supports

m4_define(</__GLA_DESC_OPTIONS/>, </NAME, INPUTS, OUTPUTS, RESULT_TYPE, CONSTRUCTOR, OPT_CHUNK_BOUNDARY, OPT_ITERABLE, REQ_CONST_STATES, OPT_FINALIZE_AS_STATE, LIBS/>)

m4_define(</__GF_DESC_OPTIONS/>, </NAME, INPUTS, CONSTRUCTOR, REQ_CONST_STATES, LIBS/>)

m4_define(</__GT_DESC_OPTIONS/>, </NAME, INPUTS, OUTPUTS, RESULT_TYPE, CONSTRUCTOR, REQ_CONST_STATES, LIBS/>)

m4_define(</__GIST_DESC_OPTIONS/>, </NAME, OUTPUTS, RESULT_TYPE, CONSTRUCTOR, REQ_CONST_STATES, LOCAL_SCHEDULER_TYPE, GLA_TYPE, TASK_TYPE, OPT_FINALIZE_AS_STATE, LIBS/>)

m4_define(</__FUNC_DESC_OPTIONS/>, </NAME, INPUTS, RESULT_TYPE, LIBS/>)

m4_define(</__DATATYPE_DESC_OPTIONS/>, </NAME, SIMPLE_TYPE, COMPLEX_TYPE, LIBS/>)

dnl # used to set and unset options
m4_define(</__SET_OPTION/>, </dnl
<//>m4_pushdef(</$1/>, m4_defn(</__$1/>))dnl
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
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__SET_GF_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__UNSET_GF_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)dnl

m4_define(</__SET_GT_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</INPUTS/>)dnl
<//>__SET_OPTION(</OUTPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
<//>__SET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__SET_OPTION(</LIBS/>)dnl
/>)

m4_define(</__UNSET_GT_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</INPUTS/>)dnl
<//>__UNSET_OPTION(</OUTPUTS/>)dnl
<//>__UNSET_OPTION(</RESULT_TYPE/>)dnl
<//>__UNSET_OPTION(</CONSTRUCTOR/>)dnl
<//>__UNSET_OPTION(</REQ_CONST_STATES/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
/>)

m4_define(</__SET_GIST_OPTIONS/>, </dnl
<//>__SET_OPTION(</NAME/>)dnl
<//>__SET_OPTION(</OUTPUTS/>)dnl
<//>__SET_OPTION(</RESULT_TYPE/>)dnl
<//>__SET_OPTION(</CONSTRUCTOR/>)dnl
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
/>)dnl

m4_define(</__UNSET_DATATYPE_OPTIONS/>, </dnl
<//>__UNSET_OPTION(</NAME/>)dnl
<//>__UNSET_OPTION(</SIMPLE_TYPE/>)dnl
<//>__UNSET_OPTION(</COMPLEX_TYPE/>)dnl
<//>__UNSET_OPTION(</LIBS/>)dnl
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


dnl # Put the BEGIN_LIBRARY comment in diversion 7 and END_LIBRARY in 9 so that
dnl # once m4 exits, the libraries in diversion 8 are commented out.
m4_divert_push(7)dnl
/* BEGIN_LIBRARIES
m4_divert_pop(7)dnl

m4_divert_push(9)dnl
END_LIBRARIES */
m4_divert_pop(9)dnl

m4_divert(0)dnl
