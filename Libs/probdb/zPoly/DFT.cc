#include "DFT.h"
#include "Timer.h"
#include <fftw3.h>
#include <iostream>

using namespace std;

#define DFT_FLAGS  FFTW_ESTIMATE

/** locking for thread safety of plan execution */
#define DFT_LOCK pthread_mutex_lock(&mutex)
#define DFT_UNLOCK pthread_mutex_unlock(&mutex)

pthread_mutex_t DFT_C::mutex = PTHREAD_MUTEX_INITIALIZER;
bool DFT_C::threadingOn = false;

void DFT_C::InitializeThreading(void){
  if (!threadingOn){
		threadingOn = true;
		fftw_init_threads();
	}
}

void DFT_C::DFT(int N, double* in, complex<double>* out, int numThreads){
	DFT_LOCK;
	if (numThreads>1){
		InitializeThreading();
	}
	if (threadingOn){
		fftw_plan_with_nthreads(numThreads);
	}

	fftw_plan p = 
	  fftw_plan_dft_r2c_1d(N, in, 
			       reinterpret_cast<fftw_complex*>(out), 
			       DFT_FLAGS);
	DFT_UNLOCK;

	Timer clock;
	fftw_execute(p);
	//	cout << "------------- DFT N=" << N << " nThreads=" << numThreads << " time=" << clock.GetTime() << endl;

	DFT_LOCK;
	fftw_destroy_plan(p);
	DFT_UNLOCK;
}

void DFT_C::IDFT(int N, complex<double>* in, double* out, int numThreads){
      	DFT_LOCK;
	if (numThreads>1){
		InitializeThreading();
	}
	if (threadingOn){
		fftw_plan_with_nthreads(numThreads);
	}

	fftw_plan p = 
	  fftw_plan_dft_c2r_1d(N, reinterpret_cast<fftw_complex*>(in), 
			       out, 
			       DFT_FLAGS);
	DFT_UNLOCK;

	Timer clock;
	fftw_execute(p);
	//	cout << "------------- DFT N=" << N << " nThreads=" << numThreads << " time=" << clock.GetTime() << endl;

	DFT_LOCK;
	fftw_destroy_plan(p);
	DFT_UNLOCK;
}

void DFT_C::DFT_multi(int k, int L, double* in, complex<double>* out){
	int n[1];
	n[0]=k;
	DFT_LOCK;
	fftw_plan p = 
	  fftw_plan_many_dft_r2c(1, n, L, in, 
				 NULL, 1, k,
				 reinterpret_cast<fftw_complex*>(out), NULL,
				 1, k, DFT_FLAGS);
	DFT_UNLOCK;
	fftw_execute(p);
	DFT_LOCK;
	fftw_destroy_plan(p);
	DFT_UNLOCK;
}

void DFT_C::IDFT_multi(int k, int L, complex<double>* in,  double* out){
	int n[1];
	n[0]=k;
	DFT_LOCK;
	fftw_plan p=fftw_plan_many_dft_c2r(1, n, L, reinterpret_cast<fftw_complex*>(in), 
																		 NULL, 1, k,
																		 out, NULL,
																		 1, k, DFT_FLAGS);
	DFT_UNLOCK;
	fftw_execute(p);
	DFT_LOCK;
	fftw_destroy_plan(p);
	DFT_UNLOCK;
}

void* DFT_C::malloc(size_t size){
	return fftw_malloc(size);
}

void DFT_C::free(void* p){
	fftw_free(p);
}
