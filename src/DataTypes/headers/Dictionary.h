#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <string>
#include <tr1/unordered_map>

// This class represents a dictionary that maps integer values to strings.
class Dictionary {
public:
    typedef int             DiffType; // return value of compare function
    typedef unsigned int    IntType; // int is enough, no need for int64
    typedef std::string     StringType;
    typedef std::tr1::unordered_map<IntType, IntType>    TranslationTable;

private:
    typedef std::tr1::unordered_map<IntType, StringType> IndexMap;
    typedef std::tr1::unordered_map<StringType, IntType> ReverseMap;

    // Mapping from ID to String
    IndexMap indexMap;
    // Mapping from String to ID
    ReverseMap reverseMap;
    // Mapping from ID to index in sorted order
    TranslationTable orderMap;

    // Next ID to be given
    IntType nextID;

    // Whether or not the dictionary has been modified since loading.
    bool modified;

    // Whether or not orderMap is valid
    bool orderValid;

public:
    // Constant for an Invalid ID
    static const IntType InvalidID;

    // Constructor
    Dictionary( void );

    // Destructor
    ~Dictionary( void );

    // Look up an ID and get the String
    const char * Lookup( IntType id ) const;

    // Look up a String and get the ID
    IntType Lookup( const char * str ) const;

    // Insert a value into the Dictionary and return the ID.
    // Throw an error if the new ID is larger than maxID
    IntType Insert( const char * str, IntType maxID );

    // Integrate another dictionary into this one, and produce a translation
    // table for any values whose ID has changed.
    void Integrate( Dictionary& other, TranslationTable& trans );

    // load/save dictionary from SQL
    // name specifies which dictionary to load/save
    void Load(const char* name);
    void Save(const char* name);

    // Compare two factors lexicographically.
    // The return value will be as follows:
    //  first > second  : retVal > 0
    //  first = second  : retVal = 0
    //  first < second  : retVal < 0
    DiffType Compare( IntType firstID, IntType secondID );

private:
    // Helper method for reverse lookups
    IntType Lookup( StringType& str ) const;

    // Helper method for inserting strings
    IntType Insert( StringType& str );

    // Helper method to compute the sorted order.
    void ComputeOrder( void );
};

#endif//_DICTIONARY_H_
