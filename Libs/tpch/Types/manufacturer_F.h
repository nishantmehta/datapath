#ifndef _manufacturer_F_H_
#define _manufacturer_F_H_

#include "Dictionary.h"
#include "DictionaryManager.h"
#include "ColumnIteratorDict.h"

#include "base/Types/STRING_LITERAL.h"

#include <limits>
#include <string.h>

/* Description block
 * TYPE_DESC
 *  NAME(manufacturer_F)
 *  SIMPLE_TYPE(ColumnIteratorDict< manufacturer_F >)
 *  DICTIONARY(manufacturer_F)
 * END_DESC
 */

class manufacturer_F {
public:
    // Type used to store ID.
    typedef unsigned char StorageType;

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
    manufacturer_F ( void );

    // Constructor from strings
    manufacturer_F ( STRING_LITERAL );

    // Constructor from storage type
    manufacturer_F ( StorageType );

    // Copy constructor
    manufacturer_F ( const manufacturer_F & other );

    // Destructor
    ~manufacturer_F (void);

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

    bool operator ==( const manufacturer_F & ) const;
    bool operator !=( const manufacturer_F & ) const;
    bool operator <( const manufacturer_F & ) const;
    bool operator <=( const manufacturer_F & ) const;
    bool operator >( const manufacturer_F & ) const;
    bool operator >=( const manufacturer_F & ) const;

    friend uint64_t Hash(const manufacturer_F );
};

// Statically Initialized Members

const char * manufacturer_F :: DictionaryName = "manufacturer_F";

const manufacturer_F :: StorageType manufacturer_F :: InvalidID = Dictionary::InvalidID;

const manufacturer_F :: StorageType manufacturer_F :: MaxID = std::numeric_limits<StorageType>::max();

Dictionary manufacturer_F :: globalDictionary = Dictionary();

/* ----- Inlined Methods ----- */

inline
manufacturer_F :: manufacturer_F ( void ) : myID( InvalidID ) {
}

// FUNC_DEF(manufacturer_F, </(str, STRING_LITERAL)/>, manufacturer_F)
inline
manufacturer_F :: manufacturer_F ( STRING_LITERAL str ) {
    FromString( str );
}

// Don't expose this to the user. Only the iterator needs to use this.
inline
manufacturer_F :: manufacturer_F ( StorageType id ) : myID( id ) {
}

inline
manufacturer_F :: manufacturer_F ( const manufacturer_F & other ) : myID(other.myID) {
}

inline
manufacturer_F :: ~manufacturer_F ( void ) {
}

inline
void manufacturer_F :: FromString( STRING_LITERAL str ) {
    // GlobalDictionary will return InvalidID if not found.
    myID = globalDictionary.Lookup( str );
}

inline
void manufacturer_F :: FromString( STRING_LITERAL str, Dictionary& localDict ) {
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
STRING_LITERAL manufacturer_F :: ToString( void ) const {
    return globalDictionary.Lookup( myID );
}

inline
bool manufacturer_F :: Invalid( void ) const {
    return myID == InvalidID;
}

inline
void manufacturer_F :: Translate(Dictionary::TranslationTable& trans){	
  if (trans.find(myID)!=trans.end())// found
    myID = trans[myID];
}

// OP_DEF(==, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator ==( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID == other.myID;
}

// OP_DEF(!=, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator !=( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return true;

    return myID != other.myID;
}

// OP_DEF(<, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator <( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID < other.myID;
}

// OP_DEF(<=, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator <=( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID <= other.myID;
}

// OP_DEF(>, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator >( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID > other.myID;
}

// OP_DEF(>=, </(f1, manufacturer_F), (f2, manufacturer_F)/>, bool)
inline
bool manufacturer_F :: operator >=( const manufacturer_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID >= other.myID;
}


/* ----- Inline Functions ----- */

inline
void FromString( manufacturer_F & f, STRING_LITERAL str ) {
   f.FromString( str ); 
}

// This function is used by TextLoader to create Factors.
inline
void FromString( manufacturer_F & f, STRING_LITERAL str, Dictionary& localDict ) {
    f.FromString( str, localDict );
}

// Used to write a manufacturer_F to a buffer.
inline
int ToString( const manufacturer_F& f, char * buffer ) {
    const char * str = f.ToString();
    strcpy( buffer, str );
    int len = strlen( buffer );
    return len + 1;
}

inline
uint64_t Hash( const manufacturer_F x) {
    return x.myID;
}

// Eventually there will be a ToString method here expoded to the user that
// will turn the factor into a VARCHAR
// FUNC_DEF(</ToString/>, </(f, manufacturer_F)/>, </STRING_LITERAL/>)
inline
STRING_LITERAL ToString( const manufacturer_F & f ) {
    return f.ToString();
}

#endif//_manufacturer_F_H_
