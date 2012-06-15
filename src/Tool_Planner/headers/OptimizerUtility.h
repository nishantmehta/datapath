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
#ifndef OPTIMIZERUTILITY_H_INCLUDED
#define OPTIMIZERUTILITY_H_INCLUDED

namespace std {
template<>
struct less<OptimizerGroup> {

  public:
    // comparison of elements: Strict weak ordering: Strict means that pr(X, X) is false.
    bool operator() (const OptimizerGroup& g1, const OptimizerGroup& g2) const {
    // First size then
    // Lexicographical comparison in this sequence lhsRel, lhsAtt, rhsRel, rhsAtt
    // "L" - g1 < g2; "G" - g1 > g2; "U"  unsure

    string compareStr1, compareStr2, compareStr3, compareStr4;

    const OptimizerGroup* ptr1 = &g1;
    const OptimizerGroup* ptr2 = &g2;


    // Cheap trick to avoid duplicate code :)
    #define OPTIMIZER_GROUP_COMPARE(OPTIMIZER_HASH_VECTOR_1, OPTIMIZER_GRP_CMP_STR)                             \
    if((ptr1 -> OPTIMIZER_HASH_VECTOR_1).size() == (ptr2 -> OPTIMIZER_HASH_VECTOR_1).size()) {                  \
        bool isLess = false;                                                                                    \
        bool isGreater = false;                                                                                 \
        for(int i = 0; i < (ptr1 -> OPTIMIZER_HASH_VECTOR_1).size(); i++) {                                     \
            if((ptr1 -> OPTIMIZER_HASH_VECTOR_1)[i].compare((ptr2 -> OPTIMIZER_HASH_VECTOR_1)[i]) < 0) {        \
                isLess = true;                                                                                  \
                break;                                                                                          \
            }                                                                                                   \
            else if((ptr1 -> OPTIMIZER_HASH_VECTOR_1)[i].compare((ptr2 -> OPTIMIZER_HASH_VECTOR_1)[i]) > 0) {   \
                isGreater = true;                                                                               \
                break;                                                                                          \
            }                                                                                                   \
        }                                                                                                       \
        if(isLess) {                                                                                            \
            OPTIMIZER_GRP_CMP_STR = "L";                                                                        \
        }                                                                                                       \
        else if(isGreater) {                                                                                    \
            OPTIMIZER_GRP_CMP_STR = "G";                                                                        \
        }                                                                                                       \
        else {                                                                                                  \
            OPTIMIZER_GRP_CMP_STR = "U";                                                                        \
        }                                                                                                       \
    }                                                                                                           \
    else if((ptr1 -> OPTIMIZER_HASH_VECTOR_1).size() < (ptr2 -> OPTIMIZER_HASH_VECTOR_1).size()) {              \
        OPTIMIZER_GRP_CMP_STR = "L";                                                                            \
    }                                                                                                           \
    else {                                                                                                      \
        OPTIMIZER_GRP_CMP_STR = "G";                                                                            \
    }                                                                                                           \

    OPTIMIZER_GROUP_COMPARE(_lhsHashRel, compareStr1)
    OPTIMIZER_GROUP_COMPARE(_lhsHashAtt, compareStr2)
    OPTIMIZER_GROUP_COMPARE(_rhsHashRel, compareStr3)
    OPTIMIZER_GROUP_COMPARE(_rhsHashAtt, compareStr4)

    #undef OPTIMIZER_GROUP_COMPARE

    const string compareStr = compareStr1 + compareStr2 + compareStr3 + compareStr4;

    assert(compareStr.length() == 4);
    assert(compareStr[0] == 'L' || compareStr[0] == 'G' || compareStr[0] == 'U');
    assert(compareStr[1] == 'L' || compareStr[1] == 'G' || compareStr[1] == 'U');
    assert(compareStr[2] == 'L' || compareStr[2] == 'G' || compareStr[2] == 'U');
    assert(compareStr[3] == 'L' || compareStr[3] == 'G' || compareStr[3] == 'U');


    if(compareStr[0] == 'L') {
        return true;
    }
    else if(compareStr[0] == 'G') {
        return false;
    }
    else {
        if(compareStr[1] == 'L') {
            return true;
        }
        else if(compareStr[1] == 'G') {
            return false;
        }
        else {
            if(compareStr[2] == 'L') {
                return true;
            }
            else if(compareStr[2] == 'G') {
                return false;
            }
            else {
                    if(compareStr[3] == 'L') {
                        return true;
                    }
                    else if(compareStr[3] == 'G') {
                        return false;
                    }
                    // IF "UUUU" =>> Equal Group return false (since strict weak ordering, => greater than or equal to)
                    return false;
            }
        }
    }

    }

};

/*
template<>
struct less<PathNetworkCost> {

  public:
    // comparison of elements: Strict weak ordering: Strict means that pr(X, X) is false.
    bool operator() (const PathNetworkCost& c1, const PathNetworkCost& c2) const {
        //TODO: Multi-objective optimization
        cout << "TODO: Multi-objective optimization\n";
        if(c1._dataFlowCost < c2._dataFlowCost) {
            return true;
        }
        else {
            return false;
        }
    }
};
*/
}

#endif // OPTIMIZERUTILITY_H_INCLUDED
