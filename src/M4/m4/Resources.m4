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
divert(-1)
changequote()
changequote([, ])

include(m4sugar/m4sugar.m4)

	
### Resources.m4 -- contains helper functions used by other
### DataPath macros in the code generator.
### -------------------------------------------------------

### _arg1 macro -- returns the first element, use with parenthesized
### lists. Used by both foreach and first.
m4_define([_arg1], [$1])

### _arg2 macro -- returns the second element from a parenthesized
### list. Used by second.
m4_define([_arg2], [$2])

### _arg2 macro -- returns the third element from a parenthesized
### list. Used by third.
m4_define([_arg3], [$3])

### _arg2 macro -- returns the third element from a parenthesized
### list. Used by third.
m4_define([_arg4], [$4])

### _arg2 macro -- returns the third element from a parenthesized
### list. Used by third.
m4_define([_arg5], [$5])



### TUPLE ELEMENT EXTRACTION METHODS
### for each of the following macros, a tuple is assumed as arguments
### tuples look like: (a,b,c)

### first macro -- returns the first element from a parenthesized
### list, forcing the M4 expression evaluation
m4_define(m4_first, [_arg1]$1)

### second macro -- similar to the previous one, but returns the
### second element.
m4_define(m4_second, [_arg2]$1)

### third macro -- similar to the previous one, but returns the
### second third.
m4_define(m4_third, [_arg3]$1)

### third macro -- similar to the previous one, but returns the
### second third.
m4_define(m4_forth, [_arg4]$1)

### third macro -- similar to the previous one, but returns the
### second third.
m4_define(m4_fifth, [_arg5]$1)

m4_define(m4_args, $*)

### reval macro -- forces evaluation of an M4 statement
m4_define(reval, $1)
m4_divert(0)dnl
