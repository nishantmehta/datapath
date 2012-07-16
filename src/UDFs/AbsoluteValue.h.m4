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
