#ifndef _status_F_H_
#define _status_F_H_

#include "Dictionary.h"
#include "DictionaryManager.h"
#include "ColumnIteratorDict.h"

#include "base/Types/STRING_LITERAL.h"

#include <limits>
#include <string.h>

/* Description block
 * TYPE_DESC
 *  NAME(status_F)
 *  SIMPLE_TYPE(ColumnIteratorDict< status_F >)
 *  DICTIONARY(status_F)
 * END_DESC
 */

class status_F {
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
    status_F ( void );

    // Constructor from strings
    status_F ( STRING_LITERAL );

    // Constructor from storage type
    status_F ( StorageType );

    // Copy constructor
    status_F ( const status_F & other );

    // Destructor
    ~status_F (void);

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

    bool operator ==( const status_F & ) const;
    bool operator !=( const status_F & ) const;
    bool operator <( const status_F & ) const;
    bool operator <=( const status_F & ) const;
    bool operator >( const status_F & ) const;
    bool operator >=( const status_F & ) const;

    friend uint64_t Hash(const status_F );
};

// Statically Initialized Members

const char * status_F :: DictionaryName = "status_F";

const status_F :: StorageType status_F :: InvalidID = Dictionary::InvalidID;

const status_F :: StorageType status_F :: MaxID = std::numeric_limits<StorageType>::max();

Dictionary status_F :: globalDictionary = Dictionary();

/* ----- Inlined Methods ----- */

inline
status_F :: status_F ( void ) : myID( InvalidID ) {
}

// FUNC_DEF(status_F, </(str, STRING_LITERAL)/>, status_F)
inline
status_F :: status_F ( STRING_LITERAL str ) {
    FromString( str );
}

// Don't expose this to the user. Only the iterator needs to use this.
inline
status_F :: status_F ( StorageType id ) : myID( id ) {
}

inline
status_F :: status_F ( const status_F & other ) : myID(other.myID) {
}

inline
status_F :: ~status_F ( void ) {
}

inline
void status_F :: FromString( STRING_LITERAL str ) {
    // GlobalDictionary will return InvalidID if not found.
    myID = globalDictionary.Lookup( str );
}

inline
void status_F :: FromString( STRING_LITERAL str, Dictionary& localDict ) {
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
STRING_LITERAL status_F :: ToString( void ) const {
    return globalDictionary.Lookup( myID );
}

inline
bool status_F :: Invalid( void ) const {
    return myID == InvalidID;
}

inline
void status_F :: Translate(Dictionary::TranslationTable& trans){	
  if (trans.find(myID)!=trans.end())// found
    myID = trans[myID];
}

// OP_DEF(==, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator ==( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID == other.myID;
}

// OP_DEF(!=, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator !=( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return true;

    return myID != other.myID;
}

// OP_DEF(<, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator <( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) < 0;
}

// OP_DEF(<=, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator <=( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) <= 0;
}

// OP_DEF(>, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator >( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) > 0;
}

// OP_DEF(>=, </(f1, status_F), (f2, status_F)/>, bool)
inline
bool status_F :: operator >=( const status_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) >= 0;
}


/* ----- Inline Functions ----- */

inline
void FromString( status_F & f, STRING_LITERAL str ) {
   f.FromString( str ); 
}

// This function is used by TextLoader to create Factors.
inline
void FromString( status_F & f, STRING_LITERAL str, Dictionary& localDict ) {
    f.FromString( str, localDict );
}

// Used to write a status_F to a buffer.
inline
int ToString( const status_F& f, char * buffer ) {
    const char * str = f.ToString();
    strcpy( buffer, str );
    int len = strlen( buffer );
    return len + 1;
}

inline
uint64_t Hash( const status_F x) {
    return x.myID;
}

// Eventually there will be a ToString method here exposed to the user that
// will turn the factor into a VARCHAR

// FUNC_DEF(</ToString/>, </(f, status_F)/>, </STRING_LITERAL/>)
inline
STRING_LITERAL ToString( const status_F & f ) {
    return f.ToString();
}

#endif//_status_F_H_
