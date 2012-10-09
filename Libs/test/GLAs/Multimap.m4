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
dnl # This GLA builds a multimap of tuples.

dnl # Usage
dnl # $1=GLA_NAME   name of the class generated
dnl # $2=KEY_ATTS  The attributes taking part in the comparison.
dnl # $3=VAL_ATTS   Additional attributes carried through

GLA_TEMPLATE_DESC(</Multimap/>)

m4_define(</Multimap/>, </dnl
m4_divert_push(-1) dnl # Just initialization stuff, don't have to output
m4_redefine(</GLA_NAME/>, </$1/>)dnl
m4_redefine(</KEY_ATTS/>, </$2/>)dnl
m4_redefine(</VAL_ATTS/>, </$3/>)dnl

m4_redefine(</MY_REZTYPE/>, </state/>)

m4_redefine(</MY_INPUT/>, m4_quote(GLUE_LISTS(</$2/>, </$3/>)))
m4_redefine(</MY_OUTPUT/>, </(self, STATE)/>)
m4_divert_pop(-1)dnl
/** Information for meta-GLAs
 *  GLA_DESC
 *      NAME(GLA_NAME)
 *      INPUTS(MY_INPUT)
 *      OUTPUTS(MY_OUTPUT)
 *      RESULT_TYPE(MY_REZTYPE)
 *
 *      OPT_FINALIZE_AS_STATE
 *  END_DESC
 */

#include <vector>
#include "HashFunctions.h"

#define INIT_SIZE 100000
#define USE_MCT

using namespace std;

#ifdef USE_TR1
#include <tr1/unordered_map>
using namespace std::tr1;
#define DEFINED_MAP std::tr1::unordered_map
#endif // USE_TR1

#ifdef USE_MCT
#include <mct/hash-map.hpp>
using namespace mct;
#define DEFINED_MAP mct::closed_hash_map
#endif // USE_MCT

struct Key_<//>GLA_NAME {

dnl # member variables
m4_foreach(</_A_/>,</KEY_ATTS/>,</dnl
    TYPE(_A_) VAR(_A_);
/>)dnl

    Key_<//>GLA_NAME (TYPED_CONST_REF_ARGS(KEY_ATTS)) :
m4_undefine_full(</_TMP_/>)dnl
m4_foreach(</_A_/>, </KEY_ATTS/>, </dnl
        m4_ifndef(</_TMP_/>, </m4_define(</_TMP_/>)/>, </, />)dnl
VAR(_A_)</(/> VAR(_A_) </)/>
/>)dnl
    { }

    bool operator==(const Key_<//>GLA_NAME& o) const {
        return (true<//>dnl
m4_foreach(</_A_/>,</KEY_ATTS/>,</dnl
 && VAR(_A_) == o.VAR(_A_)<//>dnl
/>)dnl
);
    }

    size_t hash_value() const {
        uint64_t hash= H_b;
m4_foreach(</_A_/>,</KEY_ATTS/>,</dnl
        hash = CongruentHash(Hash(VAR(_A_)), hash);
/>)dnl
        return (size_t) hash;
    }
};

struct Hashkey_<//>GLA_NAME {
    size_t operator()(const Key_<//>GLA_NAME & o) const {
        return o.hash_value();
    };
};

struct Value_<//>GLA_NAME {
dnl # member variables
m4_foreach(</_A_/>,</VAL_ATTS/>,</dnl
    TYPE(_A_) VAR(_A_);
/>)dnl

    Value_<//>GLA_NAME (TYPED_CONST_REF_ARGS(VAL_ATTS)) :
m4_undefine_full(</_TMP_/>)dnl
m4_foreach(</_A_/>, </VAL_ATTS/>, </dnl
        m4_ifndef(</_TMP_/>, </m4_define(</_TMP_/>)/>, </, />)dnl
VAR(_A_)</(/> VAR(_A_) </)/>
/>)dnl
    { }
};

class GLA_NAME {
public:
    typedef Key_<//>GLA_NAME KeyType;
    typedef Value_<//>GLA_NAME ValueType;
    typedef vector<ValueType> VectorType;
    typedef Hashkey_<//>GLA_NAME HashType;

    typedef DEFINED_MAP<KeyType, VectorType, HashType> MapType;

private:
    MapType myMap;

public:
    GLA_NAME<//>() : myMap( INIT_SIZE ) {}

    ~GLA_NAME</()/> {}

    void AddItem(TYPED_CONST_REF_ARGS(MY_INPUT)) {
        // check if key is already in the map; if yes, add the value.
        // Otherwise, add a new entry to the map.
        KeyType key(ARGS(KEY_ATTS));
        ValueType val(ARGS(VAL_ATTS));

        MapType::iterator it = myMap.find(key);
        if( it == myMap.end() ) {
            VectorType nVec;
            myMap.insert(MapType::value_type(key, nVec));
            it = myMap.find(key); // reposition
        }
        it->second.push_back(val);
    }

    void AddState(GLA_NAME& other) {
        MapType::iterator it = other.myMap.begin();

        while( it != other.myMap.end() ) {
            const KeyType& okey = it->first;
            const VectorType& oval = it->second;

            MapType::iterator itt = myMap.find(okey);
            if( itt != myMap.end()) {
                // Found the group
                VectorType& myVec = itt->second;
                myVec.insert(myVec.end(), oval.begin(), oval.end());
            }
            else {
                // add the other group to this hash
                myMap.insert(MapType::value_type(okey, oval));
            }

            it = other.myMap.erase(it);
        }
    }

    void FinalizeState() {
        myMap.rehash(0);
    }

    class Iterator {
        VectorType::const_iterator curIt;
        VectorType::const_iterator endIt;

    public:
        Iterator( VectorType::const_iterator& curIt, VectorType::const_iterator& endIt ) :
            curIt(curIt), endIt(endIt)
        { }

        Iterator() {}

        ~Iterator() {}

        bool GetNext(TYPED_REF_ARGS(VAL_ATTS)) {
            if( curIt != endIt ) {
m4_foreach(</_A_/>, </VAL_ATTS/>, </dnl
                VAR(_A_) = curIt->VAR(_A_);
/>)dnl

                ++curIt;
                return true;
            }

            return false;
        }
    };

    Iterator LookUp(TYPED_CONST_REF_ARGS(KEY_ATTS)) const {
        const KeyType key(ARGS(KEY_ATTS));
        MapType::const_iterator it = myMap.find(key);

        if( it != myMap.end() ) {
            VectorType::const_iterator begin = it->second.begin();
            VectorType::const_iterator end = it->second.end();
            return Iterator( begin, end );
        }
        else{
            return Iterator();
        }
    }
};
/>)dnl
