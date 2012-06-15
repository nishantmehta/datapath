//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef _GROUPBY_TR1_GLA_H_
#define _GROUPBY_TR1_GLA_H_

#include "Swap.h"
#include "DataTypes.h"
#include "Timer.h"
#include "Archive.h"

#include <iomanip>
#include <iostream>
#include <tr1/unordered_map>

using namespace std;
using namespace std::tr1;


struct Key_IPV4ADDR {
    IPV4ADDR ipv4addr;

    bool operator==(const Key_IPV4ADDR& other) const {
        return (ipv4addr == other.ipv4addr);
    }

    size_t hash_value() {
        return Hash(ipv4addr);
    }
};

struct HashKey_IPV4ADDR {
    size_t operator()(const Key_IPV4ADDR& o) const {
        Key_IPV4ADDR& newObject = const_cast<Key_IPV4ADDR&>(o);
        return newObject.hash_value();
    }
};

typedef std::tr1::unordered_map<Key_IPV4ADDR, FLOAT, HashKey_IPV4ADDR> IPDoubleMapTR1;

class GLA_GroupBy : public GLA {
private:
    // the GROUP-BY is implemented as an EfficientMap having as key the GROUP-BY
    // attribute(s) and as value the aggregate that is computed
    IPDoubleMapTR1 groupByMap;
    IPDoubleMapTR1::iterator theIterator;

public:
    GLA_GroupBy() {}
    ~GLA_GroupBy() {}

    void AddItem(IPV4ADDR& _key, FLOAT& _value) {
        // check if _key is already in the map; if yes, add _value; else, add a new
        // entry (_key, _value)
        Key_IPV4ADDR cKey; cKey.ipv4addr = _key;
        FLOAT cVal = _value;

        IPDoubleMapTR1::iterator it = groupByMap.find(cKey);
        if (it != groupByMap.end()) {
            it->second = it->second + _value;
        }
        else {
            groupByMap.insert(IPDoubleMapTR1::value_type(cKey, cVal));
        }
    }

    void AddState(GLA_GroupBy& other) {
        // combine two maps by keeping all the keys unique; for common keys, sum the
        // entries; iterate over the entries in one map; find an entry with the same
        // key in the other map; if yes, add the values; then do the reverse, but
        // ignore all the entries already existent in the map and insert only the
        // non-existent elements from the second map
        for (IPDoubleMapTR1::iterator it = groupByMap.begin(); it != groupByMap.end();
            it++) {
            Key_IPV4ADDR cKey = it->first;
            FLOAT cVal = it->second;

            IPDoubleMapTR1::iterator itt = other.groupByMap.find(cKey);
            if (itt != other.groupByMap.end()) {
                FLOAT otherVal = itt->second;
                cVal = cVal + otherVal;
            }
        }

        for (IPDoubleMapTR1::iterator it = other.groupByMap.begin();
            it != other.groupByMap.end(); it++) {
            Key_IPV4ADDR cKey = it->first;
            FLOAT cVal = it->second;

            IPDoubleMapTR1::iterator itt = groupByMap.find(cKey);
            if (itt == groupByMap.end()) {
                groupByMap.insert(IPDoubleMapTR1::value_type(cKey, cVal));
            }
        }
    }

    void Finalize() {
        theIterator = groupByMap.begin();
    }

    bool GetNext(IPV4ADDR& _key, FLOAT& _val) {
        if (theIterator == groupByMap.end()) {
            return false;
        }
        else {
            _key = theIterator->first.ipv4addr;
            _val = theIterator->second;
            theIterator++;

            return true;
        }
    }

};

#endif // _GROUPBYTR1_GLA_H_

