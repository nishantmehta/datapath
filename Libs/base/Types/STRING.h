//
//  Copyright 2012 Christopher Dudley
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

#ifndef _BASE_STRING_H_
#define _BASE_STRING_H_

#include <string.h>
#include <inttypes.h>

#include "HashFunctions.h"
#include "Constants.h"
#include "Errors.h"
#include "ColumnVarIterator.h"

// Debugging
#ifdef DEBUG_STRING
#include <string>
#include <sstream>
#include <iostream>
#endif // DEBUG_STRING

/**
 * A basic string type, representing null-terminated strings.
 */

/* System description block
 *  TYPE_DESC
 *      NAME(</STRING/>)
 *      SIMPLE_TYPE(</ColumnVarIterator< STRING >/>)
 *  END_DESC
 *
 *  SYN_DEF(</VARCHAR/>, </STRING/>)
 */

class STRING {

    // the type used for the size
    typedef size_t SizeType;
    
    // The length of the string
    SizeType length;

    // The null-terminated string
    const char * str;

    // Whether or not the string is responsible for its own storage.
    bool localStorage;

    ///// Private Functions /////

    // Clears the value of the string, deallocating memory if necessary.
    void Clear();

public:

    // The maximum length for a string in the system.
    static const SizeType MaxObjectLength = 1024;
    static const SizeType HeaderLength = 1024;

    // Empty string
    STRING();

    // Construct the object on top of the given null terminated string.
    STRING( const char * _str );

    // Make a copy of the other string (deep)
    STRING( const STRING& other );

    // Copy assigment operator (shallow)
    STRING& operator = ( const STRING& other );

    // Destructor
    ~STRING();

    ///// Serialization / Deserialization operations /////

    // Serialize myself to the buffer.
    void Serialize( char * buffer ) const;

    // Deserialize myself from the buffer
    void Deserialize( char * buffer );

    // Return the size (in bytes) this object writes in Serialize and reads in Deserialize.
    int GetObjLength() const;

    ///// Utilities /////

    // Copy the data from the other string into this one (deep copy)
    void Copy( const STRING& other );

    ///// General Methods /////
    
    // Return the length of the string
    SizeType Length( void ) const;

    // Return the null-terminated string the object represents
    const char * ToString( void ) const;

    // Operators

    // Access characters in the string.
    // Performs no bounds checking
    const char & operator [] ( SizeType index );

    // Read a character at the specified index in the string.
    // Performs index sanity checking.
    const char & at( SizeType index ) const;

#ifdef DEBUG_STRING
    // DEBUGGING
    std::string DebugString(void) const {
        std::ostringstream ostream;
        ostream << "Length: " << length << " local: " << localStorage << " str: " << std::string(str);
        return ostream.str();
    }
#endif // DEBUG_STRING
};

typedef STRING VARCHAR;

inline
STRING :: STRING() : length(0), str(""), localStorage(false) {
}

inline
STRING :: STRING( const char * _str ) : length(strlen(_str)), str(_str), localStorage(false) {
}

inline
STRING :: STRING( const STRING& other ) : length(other.length), str(strdup(other.str)), localStorage(true) {
}

inline
STRING& STRING :: operator = ( const STRING& other ) {
    Clear();

    length = other.length;
    str = other.str;
    localStorage = other.localStorage;

    if( localStorage ) {
        str = strdup( other.str );
    }
}

inline
STRING :: ~STRING() {
    Clear();
}

inline
void STRING :: Clear() {
    if( localStorage ) {
        delete[] str;
        str = "";
        length = 0;
        localStorage = false;
    }
}

inline
void STRING :: Serialize( char * buffer ) const {
#ifdef DEBUG_STRING
    std::cout << "Serializing -> " << DebugString() << std::endl; 
#endif // DEBUG_STRING

    strcpy( buffer, str );
}

inline
void STRING :: Deserialize( char * buffer ) {
    Clear();

    str = buffer;
    length = strlen( str );

#ifdef DEBUG_STRING
    std::cout << "Deserialized -> " << DebugString() << std::endl;
#endif // DEBUG_STRING
}

inline
int STRING :: GetObjLength() const {
    return length + 1;
}

inline
void STRING :: Copy( const STRING& other ) {
    Clear();

    length = other.length;
    str = strdup( other.str );
    localStorage = true;
}

// Copy function
inline
void Copy( STRING& to, const STRING& from ) {
    to.Copy( from );
}

inline
STRING::SizeType STRING :: Length( void ) const {
    return length;
}

inline
const char * STRING :: ToString( void ) const {
    return str;
}

// ToString for Print
inline
int ToString( const STRING& str, char* text ) {
#ifdef DEBUG_STRING
    std::cout << "ToString -> " << str.DebugString() << std::endl;
#endif // DEBUG_STRING
    strcpy( text, str.ToString() );
    return str.Length() + 1;
}

// FromString for TextLoader
inline
void FromString( STRING& str, char * text ) {
    str = STRING(text);
#ifdef DEBUG_STRING
    std::cout << "FromString -> " << str.DebugString() << std::endl;
#endif // DEBUG_STRING
}

// Operators
inline
bool operator == (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) == 0;
}

inline
bool operator != (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) != 0;
}

inline
bool operator > (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) > 0;
}

inline
bool operator >= (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) >= 0;
}

inline
bool operator < (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) < 0;
}

inline
bool operator <= (const STRING& str1, const STRING& str2) {
    return strcmp( str1.ToString(), str2.ToString() ) <= 0;
}

inline
const char & STRING :: operator [] ( SizeType index ) {
    return str[index];
}

inline
const char & STRING :: at( SizeType index ) const {
    FATALIF( index >= length, "Attempting to access character past the end of a STRING.");

    return str[index];
}

// Hash function
inline
uint64_t Hash( const STRING& str ) {
    return HashString( str.ToString() );
}

#ifdef _HAS_CPP_11
#include <functional>
// C++11 STL-Compliant hash struct specialization

namespace std {

template <>
class hash<STRING> {
    size_t operator () (const STRING& key) const {
        return Hash( key );
    }
};

}

#endif // _HAS_CPP_11

#endif // _BASE_STRING_H_
