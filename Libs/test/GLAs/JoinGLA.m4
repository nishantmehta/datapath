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
dnl # $1=GLA_NAME   name of the class generated
dnl # $2=MAP_NAME   name of the class containing the RHS map
dnl # $3=KEY_ATTS   the join attributes
dnl # $4=LHS_ATTS   extra attributes from the LHS
dnl # $5=RHS_ATTS   extra attributes from the RHS

GLA_TEMPLATE_DESC(</JoinGLA/>)

m4_define(</JoinGLA/>, </dnl
m4_divert_push(-1)

m4_redefine(</GLA_NAME/>, </$1/>)
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
 *  GLA_DESC
 *      NAME(GLA_NAME)
 *      INPUTS(MY_INPUT)
 *      OUTPUTS(MY_OUTPUT)
 *      RESULT_TYPE(MY_REZTYPE)
 *
 *      REQ_CONST_STATES(MY_CONST_STATES)
 *  END_DESC
 */

class GLA_NAME {
    const MAP_NAME & map;

    struct Tuple {
m4_foreach(</_A_/>, </MY_OUTPUT/>, </dnl
        TYPE(_A_) VAR(_A_);
/>)dnl

        Tuple(TYPED_CONST_REF_ARGS(MY_OUTPUT)) :
m4_define(</_FIRST_/>, <//>)dnl
m4_foreach(</_A_/>, </MY_OUTPUT/>, </dnl
            m4_ifdef(</_FIRST_/>, </m4_undefine(</_FIRST_/>)/>, </, />)<//>dnl
VAR(_A_) (VAR(_A_))
/>)dnl
        {}

        Tuple() {}
    };

    typedef vector<Tuple> ResultVector;
    ResultVector result;

    typedef ResultVector::iterator ResultIterator;
    ResultIterator curIt;
    ResultIterator endIt;

public:

    GLA_NAME ( const MAP_NAME & map ) : map(map) {
    }

    void AddItem( TYPED_CONST_REF_ARGS(MY_INPUT) ) {
        MAP_NAME::Iterator it = map.LookUp(ARGS(KEY_ATTS));

m4_foreach(</_A_/>, </RHS_ATTS/>, </dnl
        TYPE(_A_) VAR(_A_);
/>)dnl

        while( it.GetNext(ARGS(RHS_ATTS)) ) {
            Tuple t(ARGS(MY_OUTPUT) );
            result.push_back(t);
        }
    }

    void AddState( GLA_NAME& other ) {
        ResultIterator it = other.result.begin();
        ResultIterator endIt = other.result.end();

        while( it != endIt ) {
            result.push_back(*it);
            ++it;
        }
    }

    void Finalize() {
        curIt = result.begin();
        endIt = result.end();
    }

    bool GetNextResult( TYPED_REF_ARGS(MY_OUTPUT) ) {
        if( curIt != endIt ) {
m4_foreach(</_A_/>, </MY_OUTPUT/>, </dnl
            VAR(_A_) = curIt->VAR(_A_);
/>)dnl

            ++curIt;
            return true;
        }
        else {
            return false;
        }
    }

};
/>)dnl
