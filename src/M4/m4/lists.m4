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

dnl # macro to list arguments with types
dnl # $1=list of the form (var, type)
dnl # result is type, ...
m4_define(</TYPES/>,</dnl
m4_ifdef(</__TEMP__/>, </m4_undefine(</__TEMP__/>)/>)dnl
m4_foreach(</_A_/>,</$*/>,</dnl
m4_append(</__TEMP__/>, TYPE(_A_),</</, />/>)dnl
/>)dnl
m4_ifdef(</__TEMP__/>, __TEMP__)<//>dnl
/>)dnl
m4_divert(0)dnl
