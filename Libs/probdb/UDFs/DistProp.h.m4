FUNC_TEMPLATE_DESC(</Equal/>)
dnl # This is a templated function to created an Armadillo vector from a set
dnl # of values. This is a column vector, by default.
dnl #
dnl # Usage:
dnl # $1 = The name of the function
dnl # $2 = The number of elements in the vector.
dnl # $3 = The datatype of the elements (optional, default is DOUBLE)
m4_define(</Equal/>, </dnl
m4_redefine(</FUNC_NAME/>, </$1/>)dnl
m4_redefine(</DIST_TYPE/>, </$2/>)dnl

m4_pushdef(</__TEMP__/>, m4_defn(</Equal/>))
m4_undefine(</Equal/>)
/*
 * Function to create an Armadillo Column Vector of size NUM_ELEM of type ELEM_TYPE.
 *
 * FUNC_DESC
 *  NAME(FUNC_NAME)
 *  INPUTS(</(s,STATE),(a,DOUBLE)/>)
 *  RESULT_TYPE(DOUBLE)
 * END_DESC
 *
 */

inline
DOUBLE FUNC_NAME (STATE s, DOUBLE x) {
       DIST_TYPE* dist = (DIST_TYPE*) s.GetObject(M4_HASH_NAME(DIST_TYPE));
       return dist->Equal(x);
}
m4_define(</Equal/>, m4_defn(</__TEMP__/>))
m4_popdef(</__TEMP__/>)
/>)dnl
