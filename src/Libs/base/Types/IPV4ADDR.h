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
#ifndef _IPV4ADDR_H_
#define _IPV4ADDR_H_

#include <stdlib.h>
#include <stdio.h>

/** This type implements an efficient IP v4 address
        Internal representation is an int (for efficiency).
*/

/** Type definition for the type IPv4
 *
 *  TYPE_DESC
 *      NAME(</IPv4/>)
 *      SIMPLE_TYPE
 *  END_DESC
 *
 *  SYN_DEF(</IPV4ADDR/>, </IPv4/>)
 */
class IPv4;

bool operator < (const IPv4 &d1, const IPv4 &d2);
bool operator <= (const IPv4 &d1, const IPv4 &d2);
bool operator > (const IPv4 &d1, const IPv4 &d2);
bool operator >= (const IPv4 &d1, const IPv4 &d2);
bool operator == (const IPv4 &d1, const IPv4 &d2);
bool operator != (const IPv4 &d1, const IPv4 &d2);

// function to extract the domain (class C)
IPv4 Domain(IPv4 x);

class IPv4 {
private:
    union addr_rep {
        unsigned int asInt;
        struct {
            unsigned char c1;
            unsigned char c2;
            unsigned char c3;
            unsigned char c4;
        } split;
    };
    addr_rep addr;

public:
    // Default constructor
    IPv4(void){}

    /* Constructor from string "xxx.xxx.xxx.xxx". The format is fixed */
    IPv4 (const char *_addr){
        FromString(_addr);
    }

    // constructor from integers
    IPv4(char c1, char c2, char c3, char c4){
        addr.split.c1 = c1;
        addr.split.c2 = c2;
        addr.split.c3 = c3;
        addr.split.c4 = c4;
        }

    void FromString(const char *_addr) {
        unsigned int c1;
        unsigned int c2;
        unsigned int c3;
        unsigned int c4;
        sscanf(_addr, "%u.%u.%u.%u", &c1, &c2, &c3, &c4);

        addr.split.c1 = c1;
        addr.split.c2 = c2;
        addr.split.c3 = c3;
        addr.split.c4 = c4;
    }

    int ToString(char* text) const{
        return 1+sprintf(text,"%u.%u.%u.%u",
                                            (unsigned int) addr.split.c1,
                                            (unsigned int) addr.split.c2,
                                            (unsigned int) addr.split.c3,
                                            (unsigned int) addr.split.c4);
    }

    void Print(void){ printf("%u.%u.%u.%u",
                                                     (unsigned int) addr.split.c1,
                                                     (unsigned int) addr.split.c2,
                                                     (unsigned int) addr.split.c3,
                                                     (unsigned int) addr.split.c4);
    }

    /* operators */
    friend bool operator < (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt<d2.addr.asInt);
    }

    friend bool operator <= (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt<=d2.addr.asInt);
    }

    friend bool operator > (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt>d2.addr.asInt);
    }

    friend bool operator >= (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt>=d2.addr.asInt);
    }

    friend bool operator == (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt==d2.addr.asInt);
    }

    friend bool operator != (const IPv4 &d1, const IPv4 &d2) {
        return (d1.addr.asInt!=d2.addr.asInt);
    }

    friend uint64_t Hash(const IPv4 d);

    friend IPv4 Domain(IPv4 x){
        IPv4 rez=x;
        rez.addr.split.c4=0;
        return rez;
    }

    static bool Between (const IPv4 &d, const IPv4 &dl, const IPv4 &dr) {
        return (d.addr.asInt >= dl.addr.asInt && d.addr.asInt <= dr.addr.asInt);
    }
};

inline void FromString(IPv4& x, const char* text){
    x.FromString(text);
}

inline int ToString(const IPv4& x, char* text){
    return x.ToString(text);
}

// hash function, just return the unsigned int inside
inline uint64_t Hash(const IPv4 d){ return d.addr.asInt; }

// compatibility with the other type definitions
typedef IPv4 IPV4ADDR;


#endif // _IPV4ADDR_H_
