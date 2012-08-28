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
dnl # DEFAULT VALUE DEFINITIONS
dnl # -------------------------

dnl # default value for the maximum line size
m4_ifndef(</MAX_LINE_SIZE/>, m4_define(</MAX_LINE_SIZE/>, 10000))dnl

dnl # HELPING MACROS
dnl # --------------


dnl # macro to define a variable to hold the fix part of a column
dnl # for the variable size data, this is the offset table
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_VAR/>, </$1<//>Data/>)

dnl # macro to define a variable to hold the variable part of a column
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_CONTENT/>, </$1<//>Content/>)

dnl # macro to define a variable to indicate where the next value of a variable column is written
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_CURR/>, </$1<//>Curr/>)


dnl # this macro defines the column/storage for a single column
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_DECLARE_COLUMN/>, </dnl
  dnl #if column is of type NONE do nothing		  
<//>m4_case(M4_ATT_TYPE($1), NONE,<//>dnl # column of type NONE so do nothing              , 
<//><//>, </dnl # this is the default behavior 
  MMappedStorage $1<//>Storage;
  Column $1<//>Column( $1<//>Storage );
  M4_ITERATOR_TYPE(M4_ATT_TYPE($1)) $1<//>Iterator ( dnl #
<//> $1<//>Column M4_ITERATOR_EXTRA_ARG(M4_ATT_TYPE($1)) );  
<//>/>)dnl
/>)dnl

dnl # macro to declare the correct iterator type
dnl # $1=column type
m4_define(</M4_ITERATOR_TYPE/>, </dnl
<//>m4_ifdef(</DT_ITERATOR_$1/>, </m4_defn(</DT_ITERATOR_$1/>)/>, </m4_fatal(</No known iterator for type $1/>)/>)<//>dnl
/>)

dnl # macro to put extra arguments for iterators
dnl # $1: column type
m4_define(</M4_ITERATOR_EXTRA_ARG/>, </dnl
dnl # if column is of type HString we use HStringIterator otherwise
dnl # we use ColumnIterator<type>
<//>m4_case($1,HString,</, localDictionary/>,<//>) dnl
/>)


dnl # macro to declare the correct basic type
dnl # $1=column type
m4_define(</M4_BASIC_TYPE/>, </dnl
dnl # if column is of type HString we use HStringIterator otherwise
dnl # we use ColumnIterator<type>
<//>m4_case($1,VARCHAR,</HString/>,</$1/>) dnl
/>)
m4_divert(0)dnl
