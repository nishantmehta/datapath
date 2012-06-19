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
#ifndef _HSTRING_H_
#define _HSTRING_H_

#include <assert.h>
#include <string.h>
#include <vector>
#include <string>
#include <cstring>
#include <tr1/unordered_map>
#include <pthread.h>
#include <inttypes.h>
#include <iostream>

#include "Constants.h"
#include "Machine.h"
#include "HashFunctions.h"
#include "Swap.h"
#include "Errors.h"

// Our 61 bit hash has 3 bit spare at MSB position, we use one of it to
// denote we have it in our global dictionary. If set, string has entry in
// global dictionary
#define MSB_BITSET_3RD 0x2000000000000000UL

// This is to align on 8 byte boundary, number returned will be multiple of 8
#define BYTE_ALIGN(x) ((x+7) & ~7)

/* In fast implementation of HString, we assume Hashes are different for 2 different
	 strings. In our operator== we just return false if hashes differ. But in slow
	 implementation, we take care of situation if our hash end up equal for 2 different
	 strings in the universe.

	 We maintain uniqueness in dictionary. If two frequent strings happen to have same
	 hash, we consider one of the string infrequent and dont add in dictionary.
*/


//#define SLOW_WITHOUT_RISK

class HStringIterator;

using namespace std;
/** This datatype is a hashed string type. The idea is to save
    both space and computation for this datatype. Internally,
		this type is represented as an hash value.

		WARNING: In this class we do no synchronization at all. We assume
		that theser operations are embeded in code that does chunk size
		synchronization on the dictionary.

		Do not use it for temporary strings like HString("abcd"), since
		this class does not maintain its own copy of string data, relies on you
*/

class HString {
public:
	typedef tr1::unordered_map<__uint64_t, string> Dictionary; // type for global dictionary

	/** type that allows LocalDictionaries to be transported around */

	class DictionaryWrapper {
	private:
		Dictionary* dict;

		// no copy constructor or assignment
		DictionaryWrapper(const DictionaryWrapper&);
		void operator=(const DictionaryWrapper&);

	public:
		DictionaryWrapper(Dictionary* _dict = NULL):dict(_dict){};

		// get access to the dictionary as a refference
		Dictionary& GetDictionary(){
			FATALIF( dict == NULL, "Trying to access an invalid dictionary");
			return *dict;
		}

		void swap(DictionaryWrapper& other){
			SWAP(dict, other.dict);
		}

		void copy(DictionaryWrapper& other){
			// we do nothing but make sure we fail if this is ever called
			dict = NULL;
		}

		~DictionaryWrapper(){
			// nothing for now.
			// TODO: inform the global pool that we are
			// done with this local dictionary

			// NOTE: under any circumstances, do not detete the dict pointe
		}

	};

private:
	// WARNING:  the implementation depends on having this specific
	// order of members. Make sure nothing gets added to the list
	//
	// BEGIN DO NOT CHANGE ORDER SECTION
	__uint64_t mHash; // to compare frequent strings

	__uint64_t mStrLen; // Length of the string stored

	const char* mStr; // to compare infrequent strings
  // END  DO NOT CHANGE ORDER SECTION

	// Global dictionary
	static Dictionary globalDictionary;

	// lock for the list of local dictionaries
	static 	pthread_mutex_t mutex; // the mutex that protects the local
																 // dictionary list

	// the list of local dictionaries
	static vector<Dictionary*> localDictionaries;

  operator const char *() const;

	/* These are private functions only to be used by HStringIterator */

	/* This will return the object length, i.e. size of all members added
	*/
	unsigned int GetObjLength() const;

	/* This will return the object length, i.e. size of all members added but it will
		 also compute the string length, hence mStrLen will have some valid value. This
		 function is used the the time of writing
	*/
	unsigned int ComputeObjLength() ;


public:

	/* This adds the string in dictionary. If hash value already exists in dictionary,
		 we don't add it again. This can happen if same string is being added in dictionary
		 again, OR different string has somehow computed to be same hash value which is
		 very rare scenario. If different strings have same hash value, we don't add newer
		 string in dictionary (virtually allowing it to be infrequent category).
	*/
	static void AddEntryInDictionary(HString& h, Dictionary& dict = globalDictionary);

	/* It will initialize the dictionary from the dictionary already saved on disk.
	*/
  static void InitializeDictionary(void);

	/* Merge all local dictionaries into gloabl dictionary. First check if member is
		 not already in global dictionary before adding. Also can perform string check
		 to ensure if there is collision between two different strings who have got
		 same hash vaule somehow.
	*/
	static void MergeAllLocalIntoGlobalDictionary();

	/** Creating a local dictionary. This local dictionary is
	 * incorporated into the global dictionary by SaveDictionary() The
	 * method is thread safe

	 * The dictionary is returned in where
	 */
	static void CreateLocalDictionary(DictionaryWrapper& where);

	/* It will save the dictionary to the disk.
	*/
  static void SaveDictionary(void);

	/* Below constructor is used to create HString and it will also check
		 if the given string is in the dictionary or not. If in the dictionary
		 it will set the bit high indicating I am in the dictionary. And in that
		 case mStr will be NULL. But if not in dictionary, mStr will just point
		 to the string which is passed. In any case, mHash will contain correct
		 computed hash value wheather we are in dictionary or not.
	*/
  HString(const char* aux);

	/* Use only this ctor when populating data received from column, dont compute
		 hash then, just beleive hash and string pair is correct. Hence this ctor
		 just serves as a wrapper around the data received from column and is used
		 only by HStringIterator. This is made public to help the dictionary initialization.
	*/
	HString(__uint64_t h, __uint64_t l, const char* aux) : mHash(h), mStrLen(l), mStr(aux) {
    }

  // default constructor
  HString(): mHash(MSB_BITSET_3RD), mStrLen(-1), mStr(NULL) {}

  /** Function to look the string in a local dictionary. Bit is set if
	 * in */
	void LookUpInLocalDictionary(Dictionary& localDictionary);

	// assignment operator
  void operator=(const char *input);

/*** This operator== need to perform as fast as possible. Because it will be called for
		 millions of string comparisions. Here is the strategy what is being followed:

		 We compare 61 bit hash value first. Because if it does not match, we are sure that
		 strings do not match and just return false. It is very important to do perform this
		 check before checking if this string is in dictionary or not, because that way we
		 can avoid string comparisions if string happen to be infrequent string.

		 If 61 bit hash value matches, then we need to check extra one bit. If that matches
		 too, signifying both strings are in dictionary, then we are done, return true.

		 If extra bit do not match, then we need to do string comparision. Although our
		 hash is universal, but there are remote chances that hash values may be same for
		 two different strings. In dictionary we are ensuring that all entries are unique.
		 Hence if both strings are in dictionary and their hash value matches too, we are
		 sure they are same strings. But if any one or both of the strings are not in
		 dictionary once their hash value matches, we can not gurantee their uniqueness,
		 hence string comparisions at worst has to be done.

		 We tell the compiler at every step our likely branch, because we need to optimize
		 time for frequent string comparisions.

		 SLOW_WITHOUT_RISK
		 Another slightly slower implementation is also provided, which takes care of
		 situation if hash collides for 2 different strings. If any of two string is not
		 in dictionary, we do string comparision.
*/
  bool operator==(const HString& input);
  bool operator==(const char* input);

	/* return  a reference to hash value. Returns a reference so that hash value can be
		 modified if it is found in dictionary (inside AddInDictionary operation)
	*/
	__uint64_t& GetHashValue();

	/* return string length. This will make sense only after you call ComputeObjLen()
		 otherwise mStrLen is invalid. We dont maintain it unless it is required.
		 HStringIterator Insert() function uses it that way, overall this is used for
		 writing purpose only.
	*/
	__uint64_t GetStrLength() const;

	/* GetString() will return mStr, and not dictionary string. Because we ensure this is
			called only from HStringIterator Insert function if string is not in dictionary
	*/
	const char* GetString() const;
	// This returns pointer if present in dictionary or locally. Never returns null
	const char* GetStr() const;

	/* If this string exists in dictionary*/
	bool IsInDictionary() const;

  /* destructor, never virtual here. If you make it virtual then all alignment will
		 go haywire, because class will allocate space for __vptr
	*/
  ~HString(void){ }

	void FromString (const char* str);

	void* OptimizedSerialize(HString& hstr, void* buffer);

	HString Deserialize(void* buffer);

	int GetSize () const;

	friend class HStringIterator;
	friend class PatternMatcher;

    /* Functions below added by Christopher Dudley
     *
     * These comparison functions are necessary to implement ordering by
     * strings, which many TPC-H queries require.
     *
     * The implementation is currently slow (using strcmp), so there is room
     * for improvement, or possibly even another solution entirely.
     */
    bool operator >( const HString & input ) const;
    bool operator <( const HString & input ) const;
    bool operator >=( const HString & input ) const;
    bool operator <=( const HString & input ) const;

    // Add a Hash() function to bring it into line with all of the other
    // datatypes. Just returns mHash
    friend uint64_t Hash( HString val );
};

typedef  HString VARCHAR;
typedef  HString STRING;



///////////// Functions definitions ///////////////////////////////

inline void HString::CreateLocalDictionary(	HString::DictionaryWrapper& where){
	Dictionary* localDict = new Dictionary();
	pthread_mutex_lock(&mutex);
	localDictionaries.push_back(localDict);
	pthread_mutex_unlock(&mutex);

	DictionaryWrapper mine(localDict);
	mine.swap(where);
}

inline void FromString (HString& obj, const char* str) {
	obj.FromString(str);
}

inline void HString::LookUpInLocalDictionary(Dictionary& localDictionary){
	Dictionary::const_iterator it = localDictionary.find(mHash | MSB_BITSET_3RD);
	if (it != localDictionary.end()) {
		// 3rd MSB bit of hash tells if we are in dictionary
		mHash |= MSB_BITSET_3RD;
	}
}

inline void HString::FromString(const char* aux) {
	// Compute the hash
	mHash = HashString(aux);
	// Check if its in dictionary or not to set appropriate bit
	Dictionary::const_iterator it = globalDictionary.find(mHash | MSB_BITSET_3RD);
	if (it != globalDictionary.end()) {
		// 3rd MSB bit of hash tells if we are in dictionary
		mHash |= MSB_BITSET_3RD;
	} else { // If not in dictionary
		// Dont make a copy, just point to it
		mStr = aux;
		/* We don't maintain mStrLen here because it is computed by explicit function when needed
			 ComputeObjLen(), which is used in HStringIterator Insert function. If we try to compute
			 length here, it may not be needed for every case who uses this constructor. mStrLen is
			 only required to be written when column is written.
		*/
	}
}

/* Please read the description from the header class above*/
inline HString::HString(const char* aux) {
	FromString(aux);
}

inline HString::operator const char *() const {

	if (mHash & MSB_BITSET_3RD)
		return (globalDictionary[mHash]).c_str(); // No risk since this is private (to be used internally)
	else
		return mStr;
}

inline const char* HString::GetStr() const {

	if (mHash & MSB_BITSET_3RD) {
        WARNINGIF( globalDictionary.find( mHash ) == globalDictionary.end(),
                "Entry should be in the dictionary. mHash: %ld", mHash);
		return (globalDictionary[mHash]).c_str(); // No risk since this is private (to be used internally)
	}
	else {
		return mStr;
	}
}

inline void HString::operator=(const char *input) {

	mHash = HashString(input);
	mStr = input;
}

#ifndef SLOW_WITHOUT_RISK
/* Please read the description from header above*/
inline bool HString::operator==(const HString& input) {

	// First check is for 61 bit hash value matching. Below check could also have
	// been done as (((mHash ^ input.mHash) << 3) == 0). See which one is faster.
	if (__builtin_expect((((mHash ^ input.mHash) & ~MSB_BITSET_3RD) == 0), 1)) {
		// Second check is to see if both strings are in dictionary or not
		if (__builtin_expect((mHash & input.mHash & MSB_BITSET_3RD), 1)) {
			return true;
		} else {
			// This is slow path, has to do string comparision, HString will convert
			// itself into const char* since const char* is overloaded
			return (strcmp(*this, input) == 0);
		}
	}
	return false;
}

#else
/* Please read the description from header above*/
inline bool HString::operator==(const HString& input) {

	// If any of the strings not in dictionary, just do string comparision
	if (__builtin_expect((mHash & input.mHash & MSB_BITSET_3RD), 1)) {
		// If both strings are in dictionary, just match their hash values,
		// because we already ensured that dictionary contains unique entries
		if (__builtin_expect((mHash == input.mHash), 1)) {
			return true;
		}
	} else {
		// This is slow path, has to do string comparision, HString will convert
		// itself into const char* since const char* is overloaded
		return (strcmp(*this, input) == 0);
	}
	return false;
}
#endif

inline bool HString::operator==(const char* input) {
	return operator==(HString(input));
}

inline __uint64_t& HString::GetHashValue() {
	return mHash;
}

inline int ToString(HString& hstr, char* buffer) {
    buffer[0] = '"';
	strcpy(buffer+1, hstr.GetStr());
    int len = strlen( buffer );
    buffer[len] = '"';
    buffer[len+1] = '\0';
	return len + 2;
}

inline const char* HString::GetString() const {
	return mStr;
}

inline __uint64_t HString::GetStrLength() const {
	return mStrLen;
}

inline int HString::GetSize () const {
	return GetObjLength();
}

inline unsigned int HString::GetObjLength() const {
	if (mHash & MSB_BITSET_3RD)
		return sizeof(__uint64_t); // we dont have any associated string with us, we are in dictionary
	else {
		int x = BYTE_ALIGN(mStrLen);
		return sizeof(__uint64_t) * 2 + x;
	}
}

inline unsigned int HString::ComputeObjLength() {
	if (mHash & MSB_BITSET_3RD)
		return sizeof(__uint64_t); // we dont have any associated string with us, we are in dictionary
	else {
		mStrLen = strlen(mStr) + 1;
		int x = BYTE_ALIGN(mStrLen);
		return sizeof(__uint64_t) * 2 + x;
	}
}

inline bool HString::IsInDictionary() const {
	if (mHash & MSB_BITSET_3RD)
		return true;
	return false;
}

inline void* HString::OptimizedSerialize(HString& hstr, void* buffer) {
	if (mHash & MSB_BITSET_3RD) {
		return (void*)&mHash;
	} else {
		*((__uint64_t*)buffer) = mHash;
		*((__uint64_t*)buffer + 1) = mStrLen;
		strcpy(((char*)buffer) + 16, mStr);
		return buffer;
	}
}

inline HString HString::Deserialize(void* buffer) {
	return HString(*(__uint64_t*)buffer, *((__uint64_t*)buffer + 1), ((char*)buffer)+16);
}

/* This function is used by HStringIterator to add entries in dictionary. It sets 62nd bit
	 high indicating the given string is in dictionary. It modifies the hash value received
	 from user. It copies the string to be stored in dictionary.
*/
inline void HString::AddEntryInDictionary(HString& h, Dictionary& dictionary) {
	/* set the 3rd bit from MSB to signify we are in dictionary, we already know that this
		 bit is unset since we made that check in HStringIterator before calling this function
	*/
    //cerr << "Adding entry to dictionary: string = " << h.mStr << " hash = " << h.mHash << endl;
	__uint64_t& hashVal = h.GetHashValue();
	hashVal |= MSB_BITSET_3RD;
	Dictionary::const_iterator it = dictionary.find(hashVal);
	/* It is most likely branch because we know in almost all the cases we will not find the
		 string in dictionary once establishing the fact in HStringIterator that it is not
		 present in dictionary. But if we find it in the dictionary, that would mean a case
		 of collision where two hash values match. It could be the case that we trap ourself
		 in such situation passing same string twice, but this should be ensured at HStringIterator
		 level that we don't pass same string to be added twice (using IsInDictionary()).
	*/
	if (__builtin_expect((it == dictionary.end()), 1)){
 		char* myCopy = strdup(h.GetString());
		dictionary[hashVal] = myCopy;
	}
	else {
		/* It should better be same string. Otherwise collision for different FREQUENT strings.
			 For debug purpose, just try this assert.
		*/
		//	assert(strcmp(h.GetString(), (it->second).c_str()) == 0);

		/* If strings are different, dont set the dictionary bit for newer FREQUENT string, let
			 this string be counted in infrequent strings. In rare cases this may happen. But doing
			 this we are ensuring uniqueness in dictionary.
		*/
		if (strcmp(h.GetString(), (it->second).c_str()) != 0)
			hashVal &= ~MSB_BITSET_3RD;
	}
}

/* Functions below added by Christopher Dudley
 * These comparison functions are slow compared to == because they required
 * actual string comparison operations. Maybe in the future more optimized
 * routines will be implemented.
 *
 * (In particular, these functions were implemented to support ordering by
 * strings, which is done quite often in the TPC-H queries)
 */

inline bool HString :: operator >( const HString& input ) const {
    return (strcmp(*this, input) > 0);
}

inline bool HString :: operator <( const HString & input ) const {
    return (strcmp(*this, input) < 0);
}

inline bool HString :: operator >=( const HString & input ) const {
    return (strcmp(*this, input) >= 0);
}

inline bool HString :: operator <=( const HString & input ) const {
    return (strcmp(*this, input) <= 0);
}

// Hash function for use by GLAs and such.
inline uint64_t Hash( HString val ) {
    return val.mHash & ~MSB_BITSET_3RD;
}

#endif // _HSTRING_H_

