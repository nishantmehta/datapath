#include "Dictionary.h"

#include "Errors.h"

///// Static Initialization /////
const Dictionary::IntType Dictionary::InvalidID = 0;

// Constructor
Dictionary :: Dictionary( void ) : modified(false) {
    indexMap[InvalidID] = StringType("");
    nextID = InvalidID + 1;
}

// Destructor
Dictionary :: ~Dictionary( void ) {
}

const char * Dictionary :: Lookup( IntType id ) const {
    IndexMap::const_iterator it = indexMap.find( id );
    if( it != indexMap.end() )
        return it->second.c_str();
    else {
      return "Null";
      //        it = indexMap.find( InvalidID );
      //        return it->second.c_str();
    }
}

Dictionary::IntType Dictionary :: Lookup( StringType& s ) const {
    ReverseMap::const_iterator it = reverseMap.find( s );
    if( it != reverseMap.end() )
        return it->second;
    else
        return InvalidID;
}

Dictionary::IntType Dictionary :: Lookup( const char * str ) const {
    StringType s(str);
    return Lookup( s );
}

Dictionary::IntType Dictionary :: Insert( StringType& s ) {
    modified = true;

    indexMap[nextID] = s;
    reverseMap[s] = nextID;

    return nextID++;
}

Dictionary::IntType Dictionary :: Insert( const char * str, IntType maxID ) {
    FATALIF( nextID > maxID, "Error: Unable to add new value to dictionary."
        " Next ID %llu greater than specified maximum ID %llu.", nextID, maxID );

    StringType s(str);
    return Insert( s );
}

void Dictionary :: Integrate( Dictionary& other, TranslationTable& trans ) {
    for( IndexMap::const_iterator it = other.indexMap.begin(); it != other.indexMap.end(); ++it ) {
        IntType id = it->first;
        StringType str = it->second;
        IntType myID = Lookup( str );
        if( myID == InvalidID ) {
            // This string isn't in my map.
            myID = Insert( str );
            trans[id] = myID;
            modified = true;
        } else if( myID != id ) {
            // The string is in my dictionary, but the IDs are different.
            trans[id] = myID;
        }

        // Otherwise, both IDs are the same and we need no insertions or
        // translations
    }
}

