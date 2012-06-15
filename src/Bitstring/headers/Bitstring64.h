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
#ifndef _BITSTRING64_H
#define _BITSTRING64_H

#include "stdlib.h"
#include <sstream>

/** This header specifies the implementation of the Bitstring64

    The code is written in terms of types bitmapword, signedbitmapword
    and the representaion of 1 in the desired format, BITSTRING_ONE.
    These are defined in Bitsring.h
*/


/* A set that allows compact representation of non-negative integers */
class Bitstring64
{
private:
	bitmapword w;

public:
	/* Default constructor */
	Bitstring64 ();

	/* Constructor from int*/
	/* x is an index from 0 to MaxSize () - 1*/
	Bitstring64 (unsigned int x);

	// This is used in BStringIterator
	Bitstring64 (__uint64_t u, bool isCompleteWord) : w(u) {}

	/* Constructor from string. This should not be assumed to have any
		 particular form and should be the output of some ToString()
		 call */
	Bitstring64 (string str);

	/* Serialization function */
	string ToString();


	/* x is an index from 0 to MaxSize () - 1,
		the return value tells whether the operation is successful */
	bool AddMember (unsigned int x); 	/* set bit x */
	bool DeleteMember (unsigned int x);	/* reset bit x */
	bool IsMember (unsigned int x);

	/* test functions */
	bool IsSubsetOf (const Bitstring64 &b);
	bool Overlaps (const Bitstring64 &b); // do b and ourselves intersect?
	bool IsEqual (const Bitstring64 &b);
	bool operator == (const Bitstring64 &b) const;
	bool IsEmpty ();

	/* Set operations */
	void Union (const Bitstring64 &b);
	void Intersect (const Bitstring64 &b);
	void Difference (const Bitstring64 &b);

	/* manipulation function */
	void Empty ();
	Bitstring64 Clone ();
	void copy (Bitstring64 &b);
	void swap (Bitstring64 &b);
	void SetAll ();

	/* Utility functions */
	long Size ();	/* current size of set */
	static long MaxSize (); /* max capcity of this set */
	void Print (); /* print set as query name or binary */
	void PrintBinary(); // print set in binary

	string GetStr();

	/* Auxiliary functions to use Bitstirngs in a map */
	bool operator<(const Bitstring64& other) const;
	bool LessThan(const Bitstring64& other) const;

	/*----------
		* GetFirst - find and remove first member of a set.
		* This is intended as support for iterating through the members of a set.
		* Returns a singleton bitstring with the removed member.
		* Returns 0 if set is empty.
		* NB: set is destructively modified!
		*
		* The typical pattern is
		*		Bitstring64 input;
		*		Bitstring64 tmp = input.Clone ();
		*		while (!tmp.IsEmpty())
		*			Bitstring64 x = tmp.GetFirst ();
		*			process member x;
		*----------
		*/
	Bitstring64 GetFirst ();

	/** find a 0 in the bitstring and return a Bitstring64 with the 1 in
			that position. If one cannot be found, return empty bitstring */
	Bitstring64 GetNew();

	/* GetInt - returns integer representation of the bitmapword.
		The return value is meaningless for anything but a singleton set */
	unsigned int GetInt ();

	__uint64_t GetInt64 ();

	// it is fine to have a copy constructor an operator = since this is an ID
	Bitstring64& operator= (const Bitstring64 &b);
	Bitstring64 (const Bitstring64& b);
};

/* inline functions */

inline 
Bitstring64::Bitstring64(string str){
	w = atol(str.c_str());
}

inline
string Bitstring64::ToString(){
	stringstream out;
	out << w;

	return out.str();
}

inline 
void Bitstring64::SetAll () {
	for (int i = 0; i < MaxSize () - 1; i++) {
		AddMember (i);	
	}
}

inline
Bitstring64 Bitstring64::GetNew(){
	Bitstring64 rez;
	rez.w=RIGHTMOST_ONE(~w);
	// rez.w will be 0, hence the query empty if no position left
	return rez;
}

inline
bool Bitstring64::operator<(const Bitstring64& other) const{
	return (w<other.w);
}

inline
bool Bitstring64::LessThan(const Bitstring64& other) const{
	return (w<other.w);
}

inline
Bitstring64::Bitstring64 () {
	w = 0;
}

inline
Bitstring64::Bitstring64 (unsigned int x) {
	w = 0;
	AddMember (x);
}

inline
bool Bitstring64::AddMember (unsigned int index) {
	if (index >= BITS_PER_WORD)
		return false;
	unsigned int bitnum = BITNUM(index);
	w |= (BITSTRING_ONE << bitnum);
	return true;
}

inline
bool Bitstring64::DeleteMember (unsigned int index) {
	w &= ~(BITSTRING_ONE << BITNUM(index));
	return true;
}

inline
bool Bitstring64::IsMember (unsigned int index) {
	if ((w & (BITSTRING_ONE << BITNUM(index))) != 0)
		return true;
	return false;
}

inline
bool Bitstring64::IsSubsetOf (const Bitstring64 &b) {
	if ((w & ~(b.w)) != 0)
		return false;
	return true;
}

inline
bool Bitstring64::Overlaps (const Bitstring64 &b) {
	return (w & (b.w));
}


inline
bool Bitstring64::IsEqual (const Bitstring64 &b) {
	return (w == b.w);
}

inline
bool Bitstring64::operator == (const Bitstring64 &b) const{
	return (w == b.w);
}

inline
Bitstring64& Bitstring64::operator= (const Bitstring64 &b) {
	if (&b == this) return *this;
	w = b.w;
	return *this;
}

inline
Bitstring64::Bitstring64(const Bitstring64 &b) {
	w = b.w;
}

inline
void Bitstring64::Union (const Bitstring64 &b) {
	w |= (b.w);
}

inline
void Bitstring64::Intersect (const Bitstring64 &b) {
	w &= (b.w);
}

inline
void Bitstring64::Difference (const Bitstring64 &b) {
	w &= ~b.w;
}

inline
Bitstring64 Bitstring64::Clone () {
	Bitstring64 res;
	res.w = w;
	return res;
}

inline
void Bitstring64::copy(Bitstring64 &b) {
	w = b.w;
}

inline
void Bitstring64::swap (Bitstring64 &b) {
	SWAP (w, b.w);
}

inline
bool Bitstring64::IsEmpty () {
	if (w == 0)
		return true;
	return false;
}

inline
long Bitstring64::Size () {
	int res = 0;
	bitmapword x = w;
	while (x != 0) {
		res += NumberOfOnes[x & 255UL];
		x >>= 8;
	}
	return res;
}

inline
long Bitstring64::MaxSize () {
	return BITS_PER_WORD;
}

inline void Bitstring64::Print () {
	int first = 1;
	cout << "{";
	for (unsigned int i = 0; i < BITS_PER_WORD; i++) {
		if (IsMember (i)) {
			if (!first)
				cout << ", ";
			first = 1;
			cout << i;
		}
	}
	cout << "}";
}

inline
void Bitstring64::PrintBinary() {
	bitmapword mask = BITSTRING_ONE;
	mask = mask << (BITS_PER_WORD - 1);
	for (unsigned int i = 1; i <= BITS_PER_WORD; i++) {
		if ((w & mask) == 0)
			cerr << 0;
		else
			cerr << 1;
		mask = mask >> 1;
		if (i % 8 == 0)
			cerr << " ";
	}
	cerr << endl;
}

inline string Bitstring64::GetStr () {
  int first = 1;
  string str;
  str += "{";
  for (unsigned int i = 0; i < BITS_PER_WORD; i++) {
    if (IsMember (i)) {
      if (!first) {
        str += ", ";
      }
      first = 1;
      stringstream s;
      s << i;
      str += s.str();
    }
  }
  str += "}";
  return str;
}


inline
void Bitstring64::Empty(){
	w = 0;
}

inline
Bitstring64 Bitstring64::GetFirst () {
	Bitstring64 rez;
	rez.w = RIGHTMOST_ONE (w);
	w = w & (~rez.w);
	return rez;
}

inline
unsigned int Bitstring64::GetInt () {
  // TODO: What is this. Is this funcion even used?
	return BITNUM((w >> 1));
}

inline
__uint64_t Bitstring64::GetInt64 () {
	return (__uint64_t)w;
}

#endif
