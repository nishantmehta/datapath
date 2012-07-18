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
m4_define(</TYPE/>,</m4_second($1)/>)dnl
m4_define(</VAR/>,</m4_first($1)/>)dnl

dnl # macro to glue two argument lists into one safely
dnl # $1: first list
dnl # $2: second list
m4_define(</GLUE_LISTS/>,</dnl
m4_if(</$2/>, <//>, </$1/>, </dnl
m4_if(</$1/>, <//>, </$2/>, </dnl
m4_ifdef(</__TEMP__/>, </m4_undefine(</__TEMP__/>)/>)dnl
m4_ifval(</$1/>,</dnl
m4_foreach(</_A_/>,</$1/>,</dnl
m4_append(</__TEMP__/>, _A_<//>, </</, />/>)dnl
/>)dnl
/>)dnl
m4_ifval(</$2/>,</dnl
m4_foreach(</_A_/>,</$2/>,</dnl
m4_append(</__TEMP__/>, _A_<//>, </</, />/>)dnl
/>)dnl
/>)dnl
m4_ifval(</$3/>,</dnl
m4_foreach(</_A_/>,</$3/>,</dnl
m4_append(</__TEMP__/>, _A_<//>, </</, />/>)dnl
/>)dnl
/>)dnl
m4_ifdef(</__TEMP__/>, __TEMP__)<//>dnl
/>)dnl
/>)dnl
/>)dnl

dnl # macro to list arguments with types
dnl # $1=list of the form (var, type)
dnl # result is type var, ...
m4_define(</TYPED_ARGS/>,</dnl
m4_ifdef(</__TEMP__/>, </m4_undefine(</__TEMP__/>)/>)dnl
m4_foreach(</_A_/>,</$*/>,</dnl
m4_append(</__TEMP__/>, TYPE(_A_) VAR(_A_),</</, />/>)dnl
/>)dnl
m4_ifdef(</__TEMP__/>, __TEMP__)<//>dnl
/>)dnl


dnl # macro to list arguments with types
dnl # $1=list of the form (var, type)
dnl # result is type& var, ...
m4_define(</TYPED_REF_ARGS/>,</dnl
m4_ifdef(</__TEMP__/>, </m4_undefine(</__TEMP__/>)/>)dnl
m4_foreach(</_A_/>,</$*/>,</dnl
m4_append(</__TEMP__/>, TYPE(_A_)& VAR(_A_),</</, />/>)dnl
/>)dnl
m4_ifdef(</__TEMP__/>, __TEMP__)<//>dnl
/>)dnl


dnl # macro to list arguments with types
dnl # $1=list of the form (var, type)
dnl # result is var, ...
m4_define(</ARGS/>,</dnl
m4_ifdef(</__TEMP__/>, </m4_undefine(</__TEMP__/>)/>)dnl
m4_foreach(</_A_/>,</$*/>,</dnl
m4_append(</__TEMP__/>, VAR(_A_),</</, />/>)dnl
/>)dnl
m4_ifdef(</__TEMP__/>, __TEMP__)<//>dnl
/>)dnl


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

dnl # Macros to make it easier to specify meta-information about GLAs.

m4_define(</NAME/>, </dnl
<//></$0($@)/>dnl
<//>m4_redefine(</__META_GLA_NAME/>, </$1/>)dnl
/>)dnl

m4_define(</INPUTS/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 1, </dnl
<//><//>m4_define(__META_GLA_NAME</_INPUT/>, </$1/>)dnl
<//>/>, </dnl
<//><//>m4_define($1</_INPUT/>, </$2/>)dnl
<//>/>)dnl
/>)dnl

m4_define(</OUTPUTS/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 1, </dnl
<//><//>m4_define(__META_GLA_NAME</_OUTPUT/>, </$1/>)dnl
<//>/>, </dnl
<//><//>m4_define($1</_OUTPUT/>, </$2/>)dnl
<//>/>)dnl
/>)dnl

m4_define(</RESULT_TYPE/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 1, </dnl
<//><//>m4_define(</GLA_REZTYPE_/>__META_GLA_NAME, </$1/>)dnl
<//>/>, </dnl
<//><//>m4_define(</GLA_REZTYPE_/>$1, </$2/>)dnl
<//>/>)dnl
/>)dnl

m4_define(</CONSTRUCTOR/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 1, </dnl
<//><//>m4_define(__META_GLA_NAME</_INIT/>, </$1/>)dnl
<//>/>, </dnl
<//><//>m4_define($1</_INIT/>, </$2/>)dnl
<//>/>)dnl
/>)dnl

dnl # Macros to make it easier to add options to GLAs
m4_define(</OPT_CHUNK_BOUNDARY/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 0, </dnl
<//><//>m4_define(</GLA_CHUNKBOUNDARY_/>__META_GLA_NAME, <//>)dnl
<//>/>, </dnl
<//><//>m4_define(</GLA_CHUNKBOUNDARY_/>$1, <//>)dnl
<//>/>)dnl
/>)dnl

m4_define(</OPT_ITERABLE/>, </dnl
<//></$0($@)/>dnl
<//>m4_if($#, 0, </dnl
<//><//>m4_define(</GLA_ITERABLE_/>__META_GLA_NAME, <//>)dnl
<//>/>, </dnl
<//><//>m4_define(</GLA_ITERABLE_/>$1, <//>)dnl
<//>/>)dnl
/>)dnl

dnl # Macro for defining what libraries a GLA requires to be linked
m4_define(</LIBS/>, </dnl
<//></$0($@)/>dnl
<//>m4_divert_push(8)dnl
<//>m4_foreach(</_LIB_/>, </$@/>, </dnl
<//><//>_LIB_
/>)dnl
<//>m4_divert_pop(8)dnl
/>)

dnl # Put the BEGIN_LIBRARY comment in diversion 7 and END_LIBRARY in 9 so that
dnl # once m4 exits, the libraries in diversion 8 are commented out.
m4_divert_push(7)dnl
/* BEGIN_LIBRARIES
m4_divert_pop(7)dnl

m4_divert_push(9)dnl
END_LIBRARIES */
m4_divert_pop(9)dnl
m4_divert(0)dnl
