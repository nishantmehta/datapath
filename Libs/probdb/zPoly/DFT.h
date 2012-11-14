#ifndef _DFT_H_
#define _DFT_H_

#include <complex>
#include <pthread.h>

using namespace std;

/** This file contains the class that builds and executes fftw3 plans.
    Since plans are almost never reusable for us, we construct a plan,
    use it and they distroy it.
    
    The main issuees to deal with are the thread safety for
    construction/destruction of plans and some syntax sugar.

    TODO: add locking so we can run this in parallel

*/

class DFT_C {
  // static lock
	static pthread_mutex_t mutex;
	static bool threadingOn; // is multi-threading on?
	
	// method to initialize multi-threading (if not already on)
	static void InitializeThreading(void);

 public:

  
  /** All the functins below have the following characteristics:

      NN: size of fft in units (not bytes)

  */

  /** Direct fft from real to complex */
	static void DFT(int NN, double* in, complex<double>* out, int numThreads/*=1*/);

  /** Inverse fft from complex to real -- complex partially distroyed */
	static void IDFT(int NN, complex<double>* in, double* out, int numThreads/*=1*/);

  /** Direct fft for a vector of inputs 

      Inputs and outputs are asumed to be contighous in memory and have the same size
      k: size of each element (in units)
      L: number of elements

      Note 1: the size of in should be at least k*L*sizeof(double)
      Note 2: it is hightly desirable for k to be a power of 2
   */
  static void DFT_multi(int k, int L, double* in, complex<double>* out);

  /* Inverse of the above.

   skip allows jumping over some elements in input and output. skip=1
   skips odd entries
  */
  static void IDFT_multi(int k, int L, complex<double>* in,  double* out);

	/** function to allocate memory that is FFT friendly */
	static void* malloc(size_t size);

	static void free(void* ptr);
};


#endif // _DFT_H_
