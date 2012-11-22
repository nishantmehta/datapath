#include "Dictionary.h"
#include "Errors.h"

#include <list>

using namespace std;

///// Static Initialization /////
const Dictionary::IntType Dictionary::InvalidID = 0;

// Constructor
Dictionary :: Dictionary( void ) : modified(false), orderValid(true) {
    indexMap[InvalidID] = StringType("NULL");
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
      return "NULL";
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
    orderValid = false;

    indexMap[nextID] = s;
    reverseMap[s] = nextID;

    return nextID++;
}

Dictionary::IntType Dictionary :: Insert( const char * str, IntType maxID ) {
    FATALIF( nextID > maxID, "Error: Unable to add new value to dictionary."
        " Next ID %u greater than specified maximum ID %u.", nextID, maxID );

    StringType s(str);
    return Insert( s );
}

void Dictionary :: Integrate( Dictionary& other, TranslationTable& trans ) {
    for( IndexMap::const_iterator it = other.indexMap.begin(); it != other.indexMap.end(); ++it ) {
        IntType id = it->first;

        if( id == InvalidID )
            continue;

        StringType str = it->second;
        IntType myID = Lookup( str );
        if( myID == InvalidID ) {
            // This string isn't in my map.
            myID = Insert( str );
            trans[id] = myID;
        } else if( myID != id ) {
            // The string is in my dictionary, but the IDs are different.
            trans[id] = myID;
        }

        // Otherwise, both IDs are the same and we need no insertions or
        // translations
    }

    // Recompute sorted order
    ComputeOrder();
}

void Dictionary :: ComputeOrder( void ) {
    if( !orderValid ) {
        list<StringType> sortList;
        for( ReverseMap::const_iterator it = reverseMap.begin(); it != reverseMap.end(); ++it ) {
            StringType& str = it->first;
            sortList.push_back(str);
        }

        sortList.sort();

        IntType curIndex = sortList.size();
        for( list<StringType>::const_iterator it = sortList.rbegin(); it != sortList.rend(); ++it ) {
            StringType& str = *it;
            IntType ID = reverseMap[str];

            orderMap[ID] = curIndex--;
        }

        orderValid = true;
    }
}

Dictionary :: DiffType Dictionary :: Compare( IntType firstID, secondID ) {
    return orderMap[firstID] - orderMap[secondID];
}

