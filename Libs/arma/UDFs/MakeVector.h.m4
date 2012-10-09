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

FUNC_TEMPLATE_DESC(</MakeVector/>)
dnl # This is a templated function to created an Armadillo vector from a set
dnl # of values. This is a column vector, by default.
dnl #
dnl # Usage:
dnl # $1 = The name of the function
dnl # $2 = The number of elements in the vector.
dnl # $3 = The datatype of the elements (optional, default is DOUBLE)
m4_define(</MakeVector/>, </dnl
m4_redefine(</FUNC_NAME/>, </$1/>)dnl
m4_redefine(</NUM_ELEM/>, </$2/>)dnl
m4_redefine(</ELEM_TYPE/>, m4_ifval(</$3/>, VAR($3), </DOUBLE/>))dnl
m4_redefine(</MAX_INDEX/>, m4_eval(NUM_ELEM - 1))dnl
m4_undefine(</MY_INPUT/>)dnl
m4_forloop(</_I_/>, 0, MAX_INDEX, </dnl
<//>m4_append(</MY_INPUT/>, </(/>x_<//>_I_</,/> ELEM_TYPE</)/>, </</, />/>)dnl
/>)dnl
dnl
m4_if( m4_eval(NUM_ELEM < 1), 1, </m4_fatal(Cannot create vector with NUM_ELEM elements!)/>)dnl
</#/>include <armadillo>

/*
 * Function to create an Armadillo Column Vector of size NUM_ELEM of type ELEM_TYPE.
 *
 * FUNC_DESC
 *  NAME(FUNC_NAME)
 *  INPUTS(MY_INPUT)
 *  RESULT_TYPE(VECTOR)
 * END_DESC
 *
 * LIBS(armadillo)
 */

inline
arma::Col<ELEM_TYPE> FUNC_NAME<//>( dnl
const ELEM_TYPE & x_0<//>dnl
m4_forloop(</_I_/>, 1, MAX_INDEX, </, />const ELEM_TYPE & </x_<//>_I_/>)dnl
 ) {
    arma::Col<ELEM_TYPE>::fixed<NUM_ELEM> v;

m4_forloop(</_I_/>, 0, MAX_INDEX, </dnl
    v[_I_] = x_<//>_I_;
/>)dnl

    return v;
}
/>)dnl
