#ifndef _partbrand_F_H_
#define _partbrand_F_H_

#include "Dictionary.h"
#include "DictionaryManager.h"
#include "ColumnIteratorDict.h"

#include "base/Types/STRING_LITERAL.h"

#include <limits>
#include <string.h>

/* Description block
 * TYPE_DESC
 *  NAME(partbrand_F)
 *  SIMPLE_TYPE(ColumnIteratorDict< partbrand_F >)
 *  DICTIONARY(partbrand_F)
 * END_DESC
 */

class partbrand_F {
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
    partbrand_F ( void );

    // Constructor from strings
    partbrand_F ( STRING_LITERAL );

    // Constructor from storage type
    partbrand_F ( StorageType );

    // Copy constructor
    partbrand_F ( const partbrand_F & other );

    // Destructor
    ~partbrand_F (void);

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

    bool operator ==( const partbrand_F & ) const;
    bool operator !=( const partbrand_F & ) const;
    bool operator <( const partbrand_F & ) const;
    bool operator <=( const partbrand_F & ) const;
    bool operator >( const partbrand_F & ) const;
    bool operator >=( const partbrand_F & ) const;

    friend uint64_t Hash(const partbrand_F );
};

// Statically Initialized Members

const char * partbrand_F :: DictionaryName = "partbrand_F";

const partbrand_F :: StorageType partbrand_F :: InvalidID = Dictionary::InvalidID;

const partbrand_F :: StorageType partbrand_F :: MaxID = std::numeric_limits<StorageType>::max();

Dictionary partbrand_F :: globalDictionary = Dictionary();

/* ----- Inlined Methods ----- */

inline
partbrand_F :: partbrand_F ( void ) : myID( InvalidID ) {
}

// FUNC_DEF(partbrand_F, </(str, STRING_LITERAL)/>, partbrand_F)
inline
partbrand_F :: partbrand_F ( STRING_LITERAL str ) {
    FromString( str );
}

// Don't expose this to the user. Only the iterator needs to use this.
inline
partbrand_F :: partbrand_F ( StorageType id ) : myID( id ) {
}

inline
partbrand_F :: partbrand_F ( const partbrand_F & other ) : myID(other.myID) {
}

inline
partbrand_F :: ~partbrand_F ( void ) {
}

inline
void partbrand_F :: FromString( STRING_LITERAL str ) {
    // GlobalDictionary will return InvalidID if not found.
    myID = globalDictionary.Lookup( str );
}

inline
void partbrand_F :: FromString( STRING_LITERAL str, Dictionary& localDict ) {
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
STRING_LITERAL partbrand_F :: ToString( void ) const {
    return globalDictionary.Lookup( myID );
}

inline
bool partbrand_F :: Invalid( void ) const {
    return myID == InvalidID;
}

inline
void partbrand_F :: Translate(Dictionary::TranslationTable& trans){	
  if (trans.find(myID)!=trans.end())// found
    myID = trans[myID];
}

// OP_DEF(==, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator ==( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return myID == other.myID;
}

// OP_DEF(!=, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator !=( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return true;

    return myID != other.myID;
}

// OP_DEF(<, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator <( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) < 0;
}

// OP_DEF(<=, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator <=( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) <= 0;
}

// OP_DEF(>, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator >( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) > 0;
}

// OP_DEF(>=, </(f1, partbrand_F), (f2, partbrand_F)/>, bool)
inline
bool partbrand_F :: operator >=( const partbrand_F & other ) const {
    if( Invalid() || other.Invalid() )
        return false;

    return globalDictionary.Compare(myID, other.myID) >= 0;
}


/* ----- Inline Functions ----- */

inline
void FromString( partbrand_F & f, STRING_LITERAL str ) {
   f.FromString( str ); 
}

// This function is used by TextLoader to create Factors.
inline
void FromString( partbrand_F & f, STRING_LITERAL str, Dictionary& localDict ) {
    f.FromString( str, localDict );
}

// Used to write a partbrand_F to a buffer.
inline
int ToString( const partbrand_F& f, char * buffer ) {
    const char * str = f.ToString();
    strcpy( buffer, str );
    int len = strlen( buffer );
    return len + 1;
}

inline
uint64_t Hash( const partbrand_F x) {
    return x.myID;
}

// Eventually there will be a ToString method here exposed to the user that
// will turn the factor into a VARCHAR

// FUNC_DEF(</ToString/>, </(f, partbrand_F)/>, </STRING_LITERAL/>)
inline
STRING_LITERAL ToString( const partbrand_F & f ) {
    return f.ToString();
}

#endif//_partbrand_F_H_
