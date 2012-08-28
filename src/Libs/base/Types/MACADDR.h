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
#ifndef _MACADDR_H_
#define _MACADDR_H_

#include <iostream>
#include <Errors.h>

/* This type implements the MAC address
   Internal representation is an Int (for better efficiency)

   NOTE: This code has a bug that resuts in mistakes in the mac (loss of the top 2 digits)
   v<<4 has to come before not after filling in the value with CharToHex

 */

/** Type definition for the type macAddr
 *
 *  TYPE_DESC
 *      NAME(</macAddr/>)
 *      SIMPLE_TYPE
 *  END_DESC
 *
 *  SYN_DEF(</MACADDR/>, </macAddr/>)
 */

using namespace std;

class macAddr;

bool operator == (const macAddr &mac1, const macAddr &mac2);
bool operator != (const macAddr &mac1, const macAddr &mac2);
bool operator < (const macAddr &mac1, const macAddr &mac2);
bool operator <= (const macAddr &mac1, const macAddr &mac2);
bool operator > (const macAddr &mac1, const macAddr &mac2);
bool operator >= (const macAddr &mac1, const macAddr &mac2);

static char table_map[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

class macAddr{

 private:

  //Internal representation of the mac address
    union mac_rep{
        unsigned long long int asInt;
        struct {
            unsigned char c1:4;
            unsigned char c2:4;
            unsigned char c3:4;
            unsigned char c4:4;
            unsigned char c5:4;
            unsigned char c6:4;
            unsigned char c7:4;
            unsigned char c8:4;
            unsigned char c9:4;
            unsigned char c10:4;
            unsigned char c11:4;
            unsigned char c12:4;
        }split;
    };
    mac_rep mac;
 public:

  //default constructor
  macAddr() {
    mac.asInt = 0;
  }

  //constructor to convert string into the mac format sepcified format
  macAddr(const char* addr){
    FromString(addr);
  }

  //copy constructor
  macAddr(const macAddr &copyMe){
    mac.asInt = copyMe.mac.asInt;
  }

  //convert from string to the mac format specified above
  void FromString(const char* addr){

    int i = 0;
    long long int v=0;

#define GROUP_OF_MAC \
    if (addr[i+1] == ':' || addr[i+1] == 0){ /* short */ \
	    v <<= 4;	  \
	    v |= HexToDecimal (addr[i++]); \
	    v <<= 4; \
    } else /* long */ { 	  \
	    v |= HexToDecimal (addr[i++]); \
			v <<=4; \
			v |= HexToDecimal (addr[i++]); \
			v <<=4; \
    }	  \
    i++ /* skip : */

        GROUP_OF_MAC;
        GROUP_OF_MAC;
        GROUP_OF_MAC;
        GROUP_OF_MAC;
        GROUP_OF_MAC;
        GROUP_OF_MAC;

        mac.asInt = v;
  }

  //convert the mac address into string
  int ToString(char *addr) const{
    return 1 + sprintf(addr, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
                       table_map[mac.split.c1], table_map[mac.split.c12],
                       table_map[mac.split.c11], table_map[mac.split.c10],
                       table_map[mac.split.c9], table_map[mac.split.c8],
                       table_map[mac.split.c7], table_map[mac.split.c6],
                       table_map[mac.split.c5], table_map[mac.split.c4],
                       table_map[mac.split.c3], table_map[mac.split.c2]);
  }

  //print the mac address
  void Print(){
    char output[20];
    ToString((char*)output);
    cout<<output;
  }

  inline int HexToDecimal(char c){
      for(int i = 0; i < 16; i++)
        if(table_map[i] == c)
            return i;
      WARNING("Error: Invalid MAC Address %c", c);
      return 0;
  }

  /* operators */

  macAddr& operator=(const macAddr &copyMe){
    mac.asInt = copyMe.mac.asInt;
    return *this;
  }

  friend bool operator==(const macAddr &mac1, const macAddr &mac2){
    return (mac1.mac.asInt == mac2.mac.asInt);
  }

  friend bool operator != (const macAddr &mac1, const macAddr &mac2){
    return !(mac1.mac.asInt == mac2.mac.asInt);
  }

  friend bool operator < (const macAddr &mac1, const macAddr &mac2){
    return (mac1.mac.asInt < mac2.mac.asInt);
  }

  friend bool operator <= (const macAddr &mac1, const macAddr &mac2){
    return (mac1.mac.asInt <= mac2.mac.asInt);
  }

  friend bool operator > (const macAddr &mac1, const macAddr &mac2){
    return (mac1.mac.asInt > mac2.mac.asInt);
  }

  friend bool operator >= (const macAddr &mac1, const macAddr &mac2){
    return (mac1.mac.asInt >= mac2.mac.asInt);
  }

  friend unsigned int Hash(macAddr mac1);

};

inline int ToString(const macAddr& x, char* text){
    return x.ToString(text);
}


inline void FromString(macAddr& x, const char* text){
    x.FromString(text);
}

typedef macAddr MACADDR;

inline unsigned int Hash(macAddr mac1){
  return mac1.mac.asInt;
}

#endif
