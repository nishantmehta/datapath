dnl #
dnl #  Copyright 2012 Christopher Dudley
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
include(Resources-T.m4)dnl
m4_include(lists.m4)dnl
m4_divert(-1)dnl

m4_define(</D_DEFINE/>, 1)
m4_define(</D_NAME/>, 2)
m4_define(</D_PARAM/>, 3)
m4_define(</D_RESULT/>, 4)
m4_define(</D_FILE/>, 5)
m4_define(</D_END/>, 6)

m4_define(</GLA_DESC/>, </dnl
<//>m4_define(</DESC_TYPE/>, </GLA/>)
<//>m4_divert_push(D_DEFINE)dnl
<//>DEFINE GLA<//>dnl
<//>m4_divert_pop(D_DEFINE)dnl
<//>m4_divert_push(D_FILE)dnl
<//> FROM "SOURCE_FILE"<//>dnl
<//>m4_divert_pop(D_FILE)dnl
/>)dnl

m4_define(</GF_DESC/>, </dnl
<//>m4_define(</DESC_TYPE/>, </GLA/>)
<//>m4_divert_push(D_DEFINE)dnl
<//>DEFINE GF<//>dnl
<//>m4_divert_pop(D_DEFINE)dnl
<//>m4_divert_push(D_FILE)dnl
<//> FROM "SOURCE_FILE"<//>dnl
<//>m4_divert_pop(D_FILE)dnl
/>)dnl

m4_define(</FUNC_DESC/>, </dnl
<//>m4_define(</DESC_TYPE/>, </FUNC/>)
<//>m4_divert_push(D_DEFINE)dnl
<//>DEFINE FUNCTION<//>dnl
<//>m4_divert_pop(D_DEFINE)dnl
<//>m4_divert_push(D_FILE)dnl
<//> FROM "SOURCE_FILE"<//>dnl
<//>m4_divert_pop(D_FILE)dnl
/>)dnl

m4_define(</OP_DESC/>, </dnl
<//>m4_define(</DESC_TYPE/>, </OP/>)
<//>m4_divert_push(D_DEFINE)dnl
<//>DEFINE OPERATOR<//>dnl
<//>m4_divert_pop(D_DEFINE)dnl
<//>m4_divert_push(D_FILE)dnl
<//> FROM "SOURCE_FILE"<//>dnl
<//>m4_divert_pop(D_FILE)dnl
/>)dnl

m4_define(</END_DESC/>, </dnl
<//>m4_divert_push(D_END)dnl
<//>;
<//>m4_divert_pop(D_END)dnl
<//>m4_divert_push(0)dnl
<//>m4_undivert<//>dnl
<//>m4_divert_pop(0)dnl
<//>m4_undefine(</DESC_TYPE/>)
/>)dnl

m4_define(</FUNC_DEF/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE FUNCTION $1 ( TYPES(</$2/>)  ) -> $3 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</OP_DEF/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE OPERATOR "$1" ( TYPES(</$2/>) ) -> $3 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</GLA_TEMPLATE_DESC/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE TEMPLATE GLA $1 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</GF_TEMPLATE_DESC/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE TEMPLATE GF $1 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</FUNC_TEMPLATE_DESC/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE TEMPLATE FUNCTION $1 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</TYPE_DESC/>, </dnl
<//>m4_divert_push(D_DEFINE)dnl
<//>DEFINE DATATYPE<//>dnl
<//>m4_divert_pop(D_DEFINE)dnl
<//>m4_divert_push(D_FILE)dnl
<//> FROM "SOURCE_FILE"<//>dnl
<//>m4_divert_pop(D_FILE)dnl
/>)dnl

m4_define(</TYPE_DEF/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE DATATYPE $1 FROM "SOURCE_FILE";
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</SYN_DEF/>, </dnl
<//>m4_divert_push(0)dnl
<//>DEFINE $1 AS $2;
<//>m4_divert_pop(0)dnl
/>)dnl

m4_define(</NAME/>, </dnl
<//>m4_divert_push(D_NAME)dnl
<//> m4_if(DESC_TYPE, OP, "$1", $1)<//>dnl
<//>m4_divert_pop(D_NAME)dnl
/>)dnl

m4_define(</INPUTS/>, </dnl
<//>m4_divert_push(D_PARAM)dnl
<//> ( <//>dnl
<//>TYPES($@)<//>dnl
<//> )<//>dnl
<//>m4_divert_pop(D_PARAM)dnl
/>)dnl

m4_define(</OUTPUTS/>, </dnl
<//>m4_divert_push(D_RESULT)dnl
<//> -> m4_case(DESC_TYPE, </GLA/>, </( />, </GF/>, </( />)dnl
<//>TYPES($@)<//>dnl
<//>m4_case(DESC_TYPE, </GLA/>, </ )/>, </GF/>, </)/>)<//>dnl
<//>m4_divert_pop(D_RESULT)dnl
/>)dnl

m4_include(SOURCE_FILE)dnl
