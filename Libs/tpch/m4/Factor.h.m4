m4_dnl # DICT_NAME and STORAGE_TYPE should be defined before running this file.
m4_define(`FACTOR_NAME', `'DICT_NAME`')m4_dnl
m4_changecom()`'m4_dnl
#ifndef _`'FACTOR_NAME`'_H_
#define _`'FACTOR_NAME`'_H_

#include "Dictionary.h"
#include "DictionaryManager.h"
#include "ColumnIteratorDict.h"

#include "base/Types/STRING_LITERAL.h"

#include <limits>
#include <string.h>

/* Description block
 * TYPE_DESC
 *  NAME(FACTOR_NAME)
 *  SIMPLE_TYPE(ColumnIteratorDict< FACTOR_NAME >)
 *  DICTIONARY(DICT_NAME)
 * END_DESC
 */

class FACTOR_NAME {
public:
    // Type used to store ID.
    typedef STORAGE_TYPE StorageType;

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
    FACTOR_NAME ( void );

    // Constructor from strings
    FACTOR_NAME ( STRING_LITERAL );

    // Constructor from storage type
    FACTOR_NAME ( StorageType );

    // Copy constructor
    FACTOR_NAME ( const FACTOR_NAME & other );

    // Destructor
    ~FACTOR_NAME (void);

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

    bool operator ==( const FACTOR_NAME & ) const;
    bool operator !=( const FACTOR_NAME & ) const;
    bool operator <( const FACTOR_NAME & ) const;
    bool operator <=( const FACTOR_NAME & ) const;
    bool operator >( const FACTOR_NAME & ) const;
    bool operator >=( const FACTOR_NAME & ) const;

    friend uint64_t Hash(const FACTOR_NAME );
};

// Statically Initialized Members

const char * FACTOR_NAME :: DictionaryName = "DICT_NAME";

const FACTOR_NAME :: StorageType FACTOR_NAME :: InvalidID = Dictionary::InvalidID;

const FACTOR_NAME :: StorageType FACTOR_NAME :: MaxID = std::numeric_limits<StorageType>::max();

Dictionary FACTOR_NAME :: globalDictionary = Dictionary();

/* ----- Inlined Methods ----- */

inline
FACTOR_NAME :: FACTOR_NAME ( void ) : myID( InvalidID ) {
}

// FUNC_DEF(FACTOR_NAME, </(str, STRING_LITERAL)/>, FACTOR_NAME)
inline
FACTOR_NAME :: FACTOR_NAME ( STRING_LITERAL str ) {
    FromString( str );
}

// Don't expose this to the user. Only the iterator needs to use this.
inline
FACTOR_NAME :: FACTOR_NAME ( StorageType id ) : myID( id ) {
}

inline
FACTOR_NAME :: FACTOR_NAME ( const FACTOR_NAME & other ) : myID(other.myID) {
}

inline
FACTOR_NAME :: ~FACTOR_NAME ( void ) {
}

inline
void FACTOR_NAME :: FromString( STRING_LITERAL str ) {
    // GlobalDictionary will return InvalidID if not found.
    myID = globalDictionary.Lookup( str );
}

inline
void FACTOR_NAME :: FromString( STRING_LITERAL str, Dictionary& localDict ) {
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
STRING_LITERAL FACTOR_NAME :: ToString( void ) const {
    return globalDictionary.Lookup( myID );
}

inline
bool FACTOR_NAME :: Invalid( void ) const {
    return myID == InvalidID;
}

inline
void FACTOR_NAME :: Translate(Dictionary::TranslationTable& trans){	
  if (trans.find(myID)!=trans.end())// found
    myID = trans[myID];
}

// OP_DEF(==, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator ==( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID == other.myID;
}

// OP_DEF(!=, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator !=( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return true;

    return myID != other.myID;
}

// OP_DEF(<, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator <( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID < other.myID;
}

// OP_DEF(<=, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator <=( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID <= other.myID;
}

// OP_DEF(>, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator >( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID > other.myID;
}

// OP_DEF(>=, </(f1, FACTOR_NAME), (f2, FACTOR_NAME)/>, bool)
inline
bool FACTOR_NAME :: operator >=( const FACTOR_NAME & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID >= other.myID;
}


/* ----- Inline Functions ----- */

inline
void FromString( FACTOR_NAME & f, STRING_LITERAL str ) {
   f.FromString( str ); 
}

// This function is used by TextLoader to create Factors.
inline
void FromString( FACTOR_NAME & f, STRING_LITERAL str, Dictionary& localDict ) {
    f.FromString( str, localDict );
}

// Used to write a FACTOR_NAME to a buffer.
inline
int ToString( const FACTOR_NAME& f, char * buffer ) {
    const char * str = f.ToString();
    strcpy( buffer, str );
    int len = strlen( buffer );
    return len + 1;
}

inline
uint64_t Hash( const FACTOR_NAME x) {
    return x.myID;
}

// Eventually there will be a ToString method here exposed to the user that
// will turn the factor into a VARCHAR

// FUNC_DEF(</ToString/>, </(f, FACTOR_NAME)/>, </STRING_LITERAL/>)
inline
STRING_LITERAL ToString( const FACTOR_NAME & f ) {
    return f.ToString();
}

#endif//_`'FACTOR_NAME`'_H_
