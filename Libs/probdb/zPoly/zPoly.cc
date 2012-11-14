#include "DFT.h"
#include "Timer.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

// #define Z_DEACTIVATE_NONINLINE
#include "zPoly.h"

#define PRINT(array,start, stop) for(int i = start; i < stop; i++) cout << array[i] << " "; cout << endl; 

// Problem size for one cpu. Numer of cpus to use is size/ONEPROC_SIZE
#define ONEPROC_SIZE (2700000*8)

using namespace std;

zPoly::zPoly(void){
  mN = initialCapacity;
  NN=0;
  resultComputed = false;
  coefs = (double*)z_malloc(sizeof(double)*2*mN);
}

zPoly::zPoly(const zPoly& o){
  mN = o.mN;
  NN=o.NN;
  resultComputed = false;
  coefs = (double*)z_malloc(sizeof(double)*2*mN);
  memcpy(coefs, o.coefs, sizeof(double)*2*mN);
}


void zPoly :: Double(void){
  mN *= 2; // double the size
  double* newCoefs = (double*)z_malloc(sizeof(double)*2*mN);
  memcpy(newCoefs, coefs, sizeof(double)*mN);
  z_free(coefs);
  coefs = newCoefs;
}

void zPoly::Print(void){
	for (int i=0; i<2*NN; i++)
		cout << coefs[i] << " ";
	cout << endl;
}

zPoly :: ~zPoly(void){
  z_free(coefs);
  coefs=NULL;
}

void zPoly :: ComputePolynomialCoeffN2(int _maxDegree){
	if (_maxDegree == -1)
		_maxDegree = NN;

#ifdef DEBUG
		cout << "Phase 0: "; Print();
#endif

 	int offset = 0; // start from beginning
	int cDeg = 0; // current degree of polynomial being constructed
	// this is the relative position of the current polynomial
	while (true){
		int end = (offset+_maxDegree > NN)? NN : offset+_maxDegree;
		for (int i=offset+1; i<end; i++){
			double old = 0.0; // old value of the coef (for incremental alg)
			// coefs of the polynomial i: q_i +z p_i
			double qi = coefs[2*i]; 
			double pi = coefs[2*i+1]; 
			for (int j=offset; j<=i; j++){
				double nOld = coefs[j];
				coefs[j]=coefs[j]*qi+old*pi;
				old=nOld;

#ifdef DEBUG
				cout << "pi=" << pi << " j=" << j << " coefs[j]=" << coefs[j] << " "; 
				cout << "old=" << old << " "; Print();
#endif


			}
			coefs[i+1]=old*pi;
#ifdef DEBUG
			cout << "Phase i: " << i << " "; Print();
#endif

		}

		// zero out the rest of the entries up to 2*end
		memset(coefs+end+1, 0, sizeof(double)*(end-offset-1));
#ifdef DEBUG
		cout << "Phase 1: "; Print();
#endif

						
		if (NN <= offset+_maxDegree){
			return; // done
		} else {
			offset+=_maxDegree;
		}
	}
	resultComputed = true;
}

void zPoly :: ComputePolynomialCoefficients(void){
  // the algorithm consists in recursive multiplicaiton of larger and arger polynomials 
  // starting with the first deggree polynomials at  the bottom
  
  // Strategy: log_2 N iterations
  // in iteration k, we multiply pairs of 2 polynomials of degree k to get polynomial of degree 2*k
  // In iteration k, the number of polynomials gets halfed, the size of poynomials gets doubled
  
  // to multiply 2 polynomials of degree k, we compute the DFT of size 2K, perform the multiplication of the 
  // DFTs and then compute the inverse.
  // We exploit the fact that the lead coefficient is always 1.0 except possibly for the last polynomial

  // We avoid using a 2*k+1 space for the DFT (which produces lots of
  // large prime factors) by oberving that IDFT(DFT(2*k)) is correct
  // except that the first coefficient is larger by 1 and the leading
  // coef is missing.
  // This way, the DFT sizes are always a multiple of 2 with perfect performance

	if (NN<CROSSOVER_POINT){
		ComputePolynomialCoeffN2();
		return;
	}

  // the complex coefficients of the DFT. The array gets reused from one stage to another
  complex<double>* dCoefs = (complex<double>*) z_malloc( sizeof(complex<double>)*mN*2 );

	// compute power of 2 immediatelly >=N
	// there are faster ways to do this for sure but we need this rarely.
	int pN=1;
	while(pN<NN)
		pN <<= 1;
	
	// zero pad coefs between N and its next power of 2
	memset((void*)(coefs+2*NN), 0, sizeof(double)*2*(pN-NN));
		
	// perform the multiplication for polynomials smaller than 1<<CROSSOVER_LEVEL
	if (CROSSOVER_LEVEL>0)
		ComputePolynomialCoeffN2(1<<CROSSOVER_LEVEL);

	// I the last polynomial smaller => no correction
  long int L = NN>>CROSSOVER_LEVEL; // number of polynomials

  for (int level=CROSSOVER_LEVEL; (1ULL<<level)<=pN; level++){
    int hL = L>>1; // hL = L/2
    bool odd = (L>(hL<<1)); // we have one element left at the end
		int K = 1<<(level); // the value of K
#ifdef DEBUG
		cout << "Phase 0: "; Print();
#endif

    // Phase 1: compute 2*(L/2) DFTs of size 2k from coefs to dCoefs
		// the odd element at the end does not need to get multiplied yet
		DFT_C::DFT_multi(2*K, hL<<1, coefs, dCoefs);

#ifdef DEBUG
		cout << "Phase 1: "; 
		for (int i=0; i<2*NN; i++)
			cout << dCoefs[i] << "|";
		cout << endl;
#endif

		// precompute the inverse so that we speed up the computation
		// computation with long double so we loose as little precision as possible
		double invNK=1.0/(2*K);

    // Phase 2: multiply DFTs of two adjacent polynomials into the first(the odd)
		// the second part needs to be zeroed since the FFT of it zeros the 
		// corresponding part in coef.
		// This looks dumb but we cannot get the fftw3 to jump over parts and we need 
		// to zero the part of coef anyway
    for (int i=0; i<hL; i++) { // for each polynomial in rez
			// compute c2k=ak*bk so we can make the correction
			double c2k = coefs[4*K*i+K]*coefs[4*K*i+3*K];
			for (int j=0; j<2*K; j++) { // for each element
				dCoefs[4*K*i+j] *= dCoefs[4*K*i + 2*K+j]*invNK;
				dCoefs[4*K*i + 2*K+j] = c2k; // set the second par to c2k
      }
    }

#ifdef DEBUG
		cout << "Phase 2: "; 
		for (int i=0; i<2*K*(L+1); i++)
			cout << dCoefs[i] << "|";
		cout << endl;
#endif    

    // Phase 3: compute IDFT for the odd polynomials
		DFT_C::IDFT_multi(2*K, hL<<1, dCoefs, coefs);

#ifdef DEBUG
		cout << "Phase 3: "; 
		Print();
#endif    

    // Phase 4: correct least significant coefficient and set even coef to [a_L b_L,0...0] 
		// the correction substracts a_L b_L
		// this works even if the polynomial is of smaller degree since a_L or b_L is 0
		// and that results in 0 correction
		// The last element if odd did not get involved so it needs no correction
		int range=hL;
    for (int i=0; i<range; i++){ // for each new polynomial
      // correct the first bit in both halves
      coefs[4*K*i+2*K]/=(2*K);
      coefs[4*K*i]-=coefs[4*K*i+2*K];
    }

#ifdef DEBUG      
		cout << "Phase 4: "; 
		Print();
#endif

		if (odd){ // left over element at the end
			hL++; //in the next round remember it
		}

    L = hL; // new L
 	}
  resultComputed = true;
}


void zPoly::AddState(zPoly& other, int numThreads){

  ensureComputed();
	// figure out the size of the result polynomial and allocate space for the direct ffts

	int newN = NN+other.NN; // fix

	// transform size better be a power of 2 otherwise FFTW3 is really slow
	int tSize=1;
	while (tSize<(newN+1))
		tSize<<=1;

	numThreads = newN/ONEPROC_SIZE;
	if (numThreads<1)
		numThreads = 1;

	cout << "zPoly: merging N1="<<NN << " with N2=" << other.NN << " into N=" 
	     << newN << " using === " << numThreads << " CPUs" << endl;
	// copute the direct ffts in the new space
	// NOTE: old ffts are not usable (wrong space
	// set coefs and dCoefs for *this and other to size N

	Timer clock;

	double* newCoefs = (double*)z_malloc(sizeof(double)*tSize);
	bzero(newCoefs, sizeof(double)*tSize);// zero out the memory
	memcpy(newCoefs, coefs, sizeof(double)*(NN+1));

	z_free(coefs);
	coefs = newCoefs;
	NN=newN; 

#ifdef PROFILE_FFT
	cout << "Phase 1 took " << clock.GetTime() << endl;
	clock.Restart();
#endif

	newCoefs = (double*)z_malloc(sizeof(double)*tSize);
	bzero(newCoefs, sizeof(double)*tSize);// zero out the memory
	memcpy(newCoefs, other.coefs, sizeof(double)*(other.NN+1));

	z_free(other.coefs);
	other.coefs = newCoefs;
	other.NN=newN;

#ifdef PROFILE_FFT
	cout << "Phase 2 took " << clock.GetTime() << endl;
	clock.Restart();
#endif 
	// z_free(dCoefs);
	// z_free(other.dCoefs);
	complex<double>* dCoefs = (complex<double>*) z_malloc( sizeof(complex<double>)*tSize );
	complex<double>* other_dCoefs = (complex<double>*) z_malloc( sizeof(complex<double>)*tSize );
	// FFT of *this
	DFT_C::DFT(tSize,coefs, dCoefs, numThreads);

#ifdef PROFILE_FFT
	cout << "Phase 3 took " << clock.GetTime() << endl;
	clock.Restart();
#endif
	// FFT of other
	DFT_C::DFT(tSize,other.coefs, other_dCoefs, numThreads);

#ifdef PROFILE_FFT
	cout << "Phase 4 took " << clock.GetTime() << endl;
	clock.Restart();
#endif
	// multiply DFTs and correct size
	double invN1 = 1.0/((double)(tSize));
	for (int j=0; j<tSize; j++) { // for each element
		dCoefs[j] *= other_dCoefs[j]*invN1;
	}

#ifdef PROFILE_FFT
	cout << "Phase 5 took " << clock.GetTime() << endl;
	clock.Restart();
#endif
	// compute inverse fft into the polynomial of this zPoly.
	DFT_C::IDFT(tSize,dCoefs,coefs, numThreads);

#ifdef PROFILE_FFT
	cout << "Phase 6 took " << clock.GetTime() << endl;
	clock.Restart();
#endif

#if 0
	long double invN1 = 1.0/((long double)(tSize));

	for (int j=0; j<tSize; j++)  // for each element
		coefs[j] *=invN1;
	// now coefs contains the multiplied polynomials

	cout << "Phase 7 took " << clock.GetTime() << endl;
	clock.Restart();
#endif

	// free up the space for all intermediate results
	z_free(dCoefs);
	z_free(other_dCoefs);
}

//makes sure the coefficients are computed by performing multiplication
void zPoly::ensureComputed()
{
  if (!resultComputed)
    ComputePolynomialCoefficients();
}

//spreads the coefficients by inserting 0's
void zPoly::spread(int power)
{
  ensureComputed();
  double* newCoefs = (double*)z_malloc(sizeof(double) * NN * power);
  bzero(newCoefs, sizeof(double) * NN * power);// zero out the memory
  for (int i = 0; i < NN; i++)
    newCoefs[power * i] = coefs[i];
  z_free(coefs);
  coefs = newCoefs;
  NN *= power;
}
