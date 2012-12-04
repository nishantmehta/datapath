#ifndef  Z_POLY_H_
#define Z_POLY_H_

#include <math.h>
#include <stdint.h>
#include "Constants.h"
#include "Swap.h"
#include "probdb/zPoly/DFT.h"

#if 1
#ifdef NUM_EXEC_ENGINE_THREADS /* We are running in DataPath */
#warning "USING MMAP ALLOCATOR"
#include "MmapAllocator.h"
#define Z_USE_MMAP
#else
#include "probdb/zPoly/DFT.h"
#endif
#endif

// at what level to switch between the O(N^2) to the O(N log^2 N) alg
#define CROSSOVER_LEVEL 0
/** NOTE: Value 8 seems to be the best overall. It only helps by 5% for 1M and not at al at 10M */


// point below witch the vanila O(N^2) is better
#define CROSSOVER_POINT 5500 // processor dependent, set at 5500 


/* This file contains the class zPoly that is used to reconstruct the
   z-polynomial given probablilities of independent results. More
   specifically, the class can compute the coefficients of
   $\prod_{i=1}^N ( (1-p_i)+p_i z )$ for given $p_i$, the probability
   that the event $i$ happens. The coefficients of the polynomail give
   the probability distribution of the count of events that happen.

*/

class zPoly {
  static const long int initialCapacity = 1ULL<<20; // 1 million. No point in allocating less
  uint64_t NN; // the actual capacity
  uint64_t mN; // max capacity (always power of 2)

  bool resultComputed; // is the resutl computed?
  // if the result is computed, it is stored in p_norm (which is increased by 1)

  double* coefs; // the coefficients of the polynomial or intermediate versions
  // size should be 2*mN
  // if resultComputed = false then coefs[2k] should be 1.0-p_i and coefs[2k+1]=p_i
  // if resultComputed = true then coefs[0..N] should be the coefficients

  // double the size of the polynomial
  void Double(void);

  /** memory allocation and dealocatin */
  void * z_malloc(size_t size);
  void z_free(void*);

 public:
  zPoly(void);
  zPoly(const zPoly& o);

  void Swap(zPoly& other){
    SWAP_memmove(zPoly, other);
  }

  // add event to the list of events
  void AddEvent(double p);

	// merge two zPolys. This effectively multiplies the two polynomials and stores the result into this one.
	// this method always uses fftw3 to multiply since it will always be faster (polynomials are large by now)
	// numThreads indicates the level of parallelism to suggest to fftw3
	void AddState(zPoly& other, int numThreads=1);

  // compute polynomial expansion of the current polynomial
  void ComputePolynomialCoefficients(void);

	// compute polynomial expansion of (parts) of the current polynomial
	// this algorithm is O(N^2)
	// maxDegree -- maximum degree of polynomial
	// once the maxDegree is reached, a new polynomial is started
	// if maxDegree = -1, then no maximum
	// This function can be used to allow O(N^2) computations for parts of the problem 
	// It produces an input that can be used in ComputePolynomialCoefficient
	void ComputePolynomialCoeffN2(int maxDegree = -1);

	// get coefficient of z^i
	double Coef(int i);

	// sise of the distribution
	int GetSize(void){ return NN; }

	// print the content
	void Print(void);

	// test correctness by computing the sum (should be 1.0)
	bool TestCorrectness(void);

	//spreads the coefficients by inserting 0's
	void spread(int power);

	//makes sure the coefficients are computed by performing multiplication
	void ensureComputed();

  ~zPoly(void);
};

#ifndef Z_DEACTIVATE_NONINLINE
/** next two functions must be here and not be inlined */
inline
void * zPoly::z_malloc(size_t size){
#ifdef Z_USE_MMAP
	return mmap_alloc(size,-1);
#else
	return DFT_C::malloc(size);
#endif
}
inline
void zPoly::z_free(void* addr){
#ifdef Z_USE_MMAP
	return mmap_free(addr);
#else
	return DFT_C::free(addr);
#endif
}
#endif // Z_DEACTIVATE_NONINLINE

inline bool zPoly::TestCorrectness(void){
  long double sum=0.0;
  for (int i=0; i<NN; i++)
    sum+=coefs[i];
  
  return fabs(sum-1.0)<1.0e-4;
}

inline double zPoly::Coef(int i){ return coefs[i]; }

inline void zPoly::AddEvent(double p){
  if (p<0.0 || p>1.0)
    return; // do not add diformed data
  if (NN == mN)
    Double(); // not enough space
  
  coefs[2*NN] = 1.0-p;
  coefs[2*NN+1] = p;
  NN++; // one more element
}

#endif //  Z_POLY_H_
