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
dnl # This GLA acts as the LHS of an asymmetric hash join. This would be better
dnl # implemented as a generalized filter, but until those are implemented, this
dnl # is used to test the state-passing mechanism.

dnl # Usage
dnl # $1=GF_NAME   name of the class generated
dnl # $2=MAP_NAME   name of the class containing the RHS map
dnl # $3=KEY_ATTS   the join attributes
dnl # $4=LHS_ATTS   extra attributes from the LHS
dnl # $5=RHS_ATTS   extra attributes from the RHS

GF_TEMPLATE_DESC(</JoinLHS/>)

m4_define(</JoinLHS/>, </dnl
m4_divert_push(-1)

m4_redefine(</GF_NAME/>, </$1/>)
m4_redefine(</MAP_NAME/>, </$2/>)
m4_redefine(</KEY_ATTS/>, </$3/>)
m4_redefine(</LHS_ATTS/>, </$4/>)
m4_redefine(</RHS_ATTS/>, </$5/>)

m4_redefine(</MY_REZTYPE/>, </multi/>)

m4_redefine(</MY_INPUT/>, m4_quote(GLUE_LISTS(</$3/>, </$4/>)))
m4_redefine(</MY_OUTPUT/>, m4_quote(GLUE_LISTS(</$3/>, </$4/>, </$5/>)))
m4_redefine(</MY_CONST_STATES/>, </(map, $2)/>)

m4_divert_pop(-1)dnl
/** Information for meta-GLAs
 *  GF_DESC
 *      NAME(GF_NAME)
 *      INPUTS(MY_INPUT)
 *      OUTPUTS(MY_OUTPUT)
 *      RESULT_TYPE(MY_REZTYPE)
 *
 *      REQ_CONST_STATES(MY_CONST_STATES)
 *  END_DESC
 */

class GF_NAME {
    const MAP_NAME & map;

    // Cached join attributes
m4_foreach(</_A_/>, </KEY_ATTS/>, </dnl
    TYPE(_A_) VAR(_A_);
/>)dnl
    // Cached LHS attributes
m4_foreach(</_A_/>, </LHS_ATTS/>, </dnl
    TYPE(_A_) VAR(_A_);
/>)dnl

    // Current iterator
    MAP_NAME :: Iterator curIt;

public:

    GF_NAME ( const MAP_NAME & map ) : map(map) {
    }

    void ProcessTuple( TYPED_CONST_REF_ARGS(MY_INPUT) ) {
m4_foreach(</_A_/>, </MY_INPUT/>, </
        this->VAR(_A_) = VAR(_A_);
/>)dnl

        curIt = map.LookUp(ARGS(KEY_ATTS));
    }

    bool GetNextResult( TYPED_REF_ARGS(MY_OUTPUT) ) {
m4_foreach(</_A_/>, </MY_INPUT/>, </dnl
        VAR(_A_) = this->VAR(_A_);
/>)

        return curIt.GetNext(ARGS(RHS_ATTS));
    }
};
/>)dnl
