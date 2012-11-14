#ifndef _test_F_H_
#define _test_F_H_

#include "Dictionary.h"
#include "DictionaryManager.h"
#include "ColumnIteratorDict.h"

#include "base/Types/STRING_LITERAL.h"

#include <limits>
#include <string.h>

/* Description block
 * TYPE_DESC
 *  NAME(test_F)
 *  COMPLEX_TYPE(ColumnIteratorDict< test_F >)
 *  DICTIONARY(test_F)
 * END_DESC
 */

class test_F {
public:
    // Type used to store ID.
    typedef unsigned short StorageType;

    static const char* DictionaryName __attribute__((weak));

    static const StorageType InvalidID __attribute__((weak));

    static const StorageType MaxID __attribute__((weak));

    static Dictionary globalDictionary __attribute__((weak));

public:
    
    /* ----- Members ----- */

    // The ID of this Factor.
    StorageType myID;

    /* ----- Constructors / Destructors ----- */
    // Default constructor
    test_F ( void );

    // Constructor from strings
    test_F ( STRING_LITERAL );

    // Constructor from storage type
    test_F ( StorageType );

    // Destructor
    ~test_F (void);

    /* ----- Methods ----- */
    // Standard FromString method
    void FromString( STRING_LITERAL );
    
    // FromString method used when building the dictionaries
    void FromString( STRING_LITERAL, Dictionary& );

    // Looks up the Factor in the global dictionary
    STRING_LITERAL ToString( void ) const;

    // Returns the ID of the Factor
    StorageType GetID( void ) const;

    // Whether or not a Factor is valid
    bool Invalid( void ) const;

    // Translate the content
    void Translate(Dictionary::TranslationTable& trans);

    /* ----- Operators ----- */

    bool operator ==( const test_F & ) const;
    bool operator !=( const test_F & ) const;
};

// Statically Initialized Members

const char * test_F :: DictionaryName = "test_F";

const test_F :: StorageType test_F :: InvalidID = Dictionary::InvalidID;

const test_F :: StorageType test_F :: MaxID = std::numeric_limits<StorageType>::max();

Dictionary test_F :: globalDictionary = Dictionary();

/* ----- Inlined Methods ----- */

inline
test_F :: test_F ( void ) : myID( InvalidID ) {
}

// FUNC_DEF(test_F, </(str, STRING_LITERAL)/>, test_F)
inline
test_F :: test_F ( STRING_LITERAL str ) {
    FromString( str );
}

// Don't expose this to the user. Only the iterator needs to use this.
inline
test_F :: test_F ( StorageType id ) : myID( id ) {
}

inline
test_F :: ~test_F ( void ) {
}

inline
void test_F :: FromString( STRING_LITERAL str ) {
    // GlobalDictionary will return InvalidID if not found.
    myID = globalDictionary.Lookup( str );
}

inline
void test_F :: FromString( STRING_LITERAL str, Dictionary& localDict ) {
    // First check if we are in the local dictionary
    myID = localDict.Lookup( str );
    if( myID != InvalidID )
        return;

    // Next check if we are in the global dictionary
    myID = globalDictionary.Lookup( str );
    if( myID != InvalidID )
        return;

    // Add a new entry to the local dictionary.
    // The dictionary should throw an error if the new ID is greater than
    // MaxID.
    myID = localDict.Insert( str, MaxID );
}

inline
STRING_LITERAL test_F :: ToString( void ) const {
    return globalDictionary.Lookup( myID );
}

inline
bool test_F :: Invalid( void ) const {
    return myID == InvalidID;
}

inline
void test_F :: Translate(Dictionary::TranslationTable& trans){	
  if (trans.find(myID)!=trans.end())// found
    myID = trans[myID];
}

// OP_DEF(==, </(f1, test_F), (f2, test_F)/>, bool)
inline
bool test_F :: operator ==( const test_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID == other.myID;
}

// OP_DEF(!=, </(f1, test_F), (f2, test_F)/>, bool)
inline
bool test_F :: operator !=( const test_F & other ) const {
    if( Invalid() || other.Invalid() )
        return true;

    return myID != other.myID;
}

/* ----- Inline Functions ----- */

inline
void FromString( test_F & f, STRING_LITERAL str ) {
   f.FromString( str ); 
}

// This function is used by TextLoader to create Factors.
inline
void FromString( test_F & f, STRING_LITERAL str, Dictionary& localDict ) {
    f.FromString( str, localDict );
}

// Used to write a test_F to a buffer.
inline
int ToString( const test_F& f, char * buffer ) {
    const char * str = f.ToString();
    strcpy( buffer, str );
    int len = strlen( buffer );
    return len + 1;
}

// Eventually there will be a ToString method here expoded to the user that
// will turn the factor into a VARCHAR

#endif//_test_F_H_
