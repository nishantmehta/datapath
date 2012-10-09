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
FUNC_TEMPLATE_DESC(</AbsoluteValue/>)
m4_define(</AbsoluteValue/>, </dnl
m4_redefine(</FUNC_NAME/>, </$1/>)dnl
m4_redefine(</VALUE/>, </$2/>)dnl
</#/>include <stdlib.h>
</#/>include <math.h>

inline
TYPE(VALUE) FUNC_NAME<//>(TYPE(VALUE) VAR(VALUE)) {
    return abs(VAR(VALUE));
}
/>)dnl
