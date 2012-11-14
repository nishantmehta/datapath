#ifndef _SUMPROBKOLMOGOROVDEPTHGLA_H_ 
#define _SUMPROBKOLMOGOROVDEPTHGLA_H_ 

#include <math.h>
 #include <gsl/gsl_rng.h>
#include "CountProbGLA.h"
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

#define O 6
#define B 10 //half of the support of distribution in standard deviations
#define N 1000 // number of bins for distribution

#define STIRLING   \
  c[0][0][0] = -1; \
  c[1][0][0] = -2; \
  c[1][0][1] = -1; \
  c[1][1][1] = 2;  \
  c[2][0][0] = -3; \
  c[2][0][1] = -3; \
  c[2][0][2] = -1; \
  c[2][1][1] = 6;  \
  c[2][1][2] = 6;   \
  c[2][2][2] = -6;  \
  c[3][0][0] = -4;  \
  c[3][0][1] = -6;  \
  c[3][0][2] = -4;  \
  c[3][0][3] = -1;  \
  c[3][1][1] = 12;  \
  c[3][1][2] = 24;  \
  c[3][1][3] = 14;  \
  c[3][2][2] = -24; \
  c[3][2][3] = -36; \
  c[3][3][3] = 24;  \
  c[4][0][0] = -5;  \
  c[4][0][1] = -10; \
  c[4][0][2] = -10; \
  c[4][0][3] = -5;  \
  c[4][0][4] = -1;  \
  c[4][1][1] = 20;  \
  c[4][1][2] = 60;  \
  c[4][1][3] = 70;  \
  c[4][1][4] = 30;  \
  c[4][2][2] = -60; \
  c[4][2][3] = -180;\
  c[4][2][4] = -150;\
  c[4][3][3] = 120; \
  c[4][3][4] = 240; \
  c[4][4][4] = -120;\
  c[5][0][0] = -6;  \
  c[5][0][1] = -15; \
  c[5][0][2] = -20; \
  c[5][0][3] = -15; \
  c[5][0][4] = -6;  \
  c[5][0][5] = -1;  \
  c[5][1][1] = 30;  \
  c[5][1][2] = 120; \
  c[5][1][3] = 210; \
  c[5][1][4] = 180; \
  c[5][1][5] = 62;  \
  c[5][2][2] = -120;\
  c[5][2][3] = -540;\
  c[5][2][4] = -900;\
  c[5][2][5] = -540;\
  c[5][3][3] = 360; \
  c[5][3][4] = 1440;\
  c[5][3][5] = 1560;\
  c[5][4][4] = -720;\
  c[5][4][5] = -1800;\
  c[5][5][5] = 720;

class SumProbKolmogorovDepthGLA {

  // cumulants of the true distribution
  long double cm[O];
  // moments of the true distribution 
  double nu[O]; 
  // moments of the initial approximation
  double bm[O];   
  //approximating distribution
  double p[N]; 
  //differences
  double d[O + 1][N]; 
  double a[O];
  double x[N], b[N];
  double sumDet;

  double inverf(double x)
  {
    return pow(0.5, 0.5) * gsl_cdf_ugaussian_Pinv(0.5 + 0.5 * x);
  }

 public:
  SumProbKolmogorovDepthGLA(void) { for(int i = 0; i < O; i++) cm[i] = 0.0; sumDet = 0.0; }

  //update cumulants of the true distribution 
  //with the new data point
  void AddItem(float p, float x){
    cm[0] += p*x;
    cm[1] += x*x * p*(1.0-p);
    cm[2] += x*x*x * p*(1.0-p)*(1.0-2.0*p);
    cm[3] += x*x*x*x * p*(1.0-p)*(1.0-6.0*p*(1.0-p));
    cm[4] += x*x*x*x*x * p*(1.0-p)*(1.0-2.0*p)*(1.0-12.0*p*(1.0-p));
    cm[5] += x*x*x*x*x*x * p*(1.0-p)*( 1.0+p*(1.0-p)*(120.0*p*(1.0-p)-30.0));
    sumDet += x;
  }

  void AddState(SumProbKolmogorovDepthGLA& o) { for (int i = 0; i < O; i++) cm[i] += o.cm[i]; sumDet += o.sumDet;}
  
  void Finalize(void){
    int c[O][O][O];
    STIRLING

    //compute moments of true distribution
    nu[0] = cm[0];
    nu[1] = cm[1] + cm[0]*cm[0];
    nu[2] = cm[2] + 3*cm[1]*cm[0] + cm[0]*cm[0]*cm[0];
    nu[3] = cm[3] + 4*cm[2]*cm[0] + 3*cm[1]*cm[1] + 6*cm[1]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0];
    nu[4] = cm[4] + 5*cm[3]*cm[0] + 10*cm[2]*cm[1] + 10*cm[2]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[0] + 10*cm[1]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0];
    nu[5] = cm[5] + 6*cm[4]*cm[0] + 15*cm[3]*cm[1] + 15*cm[3]*cm[0]*cm[0] + 10*cm[2]*cm[2]*cm[2] + 60*cm[2]*cm[1]*cm[0] + 20*cm[2]*cm[0]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[1] + 45*cm[1]*cm[1]*cm[0]*cm[0] + 15*cm[1]*cm[0]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0]*cm[0];


    //compute initial approximation
    double sigma = pow(cm[1], 0.5);
    double cdf = 0.0, newcdf = 0.0;
    for(int i = 0; i < N - 1; i++){
      b[i] = pow(2.0 * sigma, 0.5) * inverf(2.0 * i / N - 1.0) + cm[0];
      x[i] = pow(2.0 * sigma, 0.5) * inverf(2.0 * i / N - 1.0 / (2.0 * N) - 1.0) + cm[0];
      newcdf = 0.5 * ( 1.0 + erf((b[i] - cm[0]) /  pow(2.0 * cm[1], 0.5)));
      p[i] = newcdf - cdf;
      cdf = newcdf;
    }
    p[N - 1] = 1.0 - cdf;
    x[N - 1] = pow(2.0 * sigma, 0.5) * inverf(2.0 * (N - 1) / N + 1.0 / (2.0 * N) - 1.0) + cm[0];
    // compute moments of the initial approximation 
    bm[0] = cm[0];
    bm[1] = cm[1] + cm[0]*cm[0];
    bm[2] = 3*cm[1]*cm[0] + cm[0]*cm[0]*cm[0];
    bm[3] = 3*cm[1]*cm[1] + 6*cm[1]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0];
    bm[4] = 15*cm[1]*cm[1]*cm[0] + 10*cm[1]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0];
    bm[5] = 15*cm[1]*cm[1]*cm[1] + 45*cm[1]*cm[1]*cm[0]*cm[0] + 15*cm[1]*cm[0]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0]*cm[0];
 

  for (int i = 0; i < N - 1; i++) d[0][i] = p[i];

  int fact = 1;
  for (int k = 1; k <= O; k++){
    
    // compute differences
    for (int i = 1; i < N; i++) 
      d[k][i] = d[k-1][i] - d[k-1][i-1]; 

      //compute k-th moment of current approximating distribution
    double mu = bm[k];
    for (int j = 0; j < k; j++) {
      double s = 0;
      for (int i = 0; i < N; i++)
	s += pow(x[i], k) * d[k][i];
      mu += a[j] * s;  
    }

    // compute $a_k$ 
    fact *= (k+1);
    a[k] = (nu[k] - mu) / (double)fact * (k%2?1:-1);

    //update the distribution
    for (int i = 0; i < N; i++) p[i] += a[k]*d[k][i];
  }
  }

  float Equal(float a) {
    int i = 0;
    double sigma = pow(cm[1], 0.5);
    while (b[i] < a) i++;
    if (i == 0)
      return p[i] / b[0];
    if (i == N - 2)
      return p[i] / (sumDet - b[N - 2]);  
    return p[i] / (b[i + 1] - b[i]);
  }

  float Greater(float a) {
    int i = 0; double r = 0.0;
    double sigma = pow(cm[1], 0.5);
    while (b[i] < a) { i++; r += p[i]; }
    if (i == 0)
      return p[i] / b[0];
    if (i == N - 2)
      return r + p[i] / (sumDet - b[N - 2]);  
    return r + p[i] / (b[i + 1] - b[i] - 1.0);
  }

  float GreaterEq(float a) {
    int i = 0; double r = 0.0;
    double sigma = pow(cm[1], 0.5);
    while (b[i] < a) { i++; r += p[i]; }
    if (i == 0)
      return p[i] / b[0];
    if (i == N - 2)
      return r + p[i] / (sumDet - b[N - 2]);  
    return r + p[i] / (b[i + 1] - b[i]);
  }

  CountProbGLA::c_int ConfidenceInterval(float conf){
  double pp = (1.0 - conf) / 2.0, a = 0.0;
  //cout << "conf="<<conf<<" pp=" << pp<< endl;
  int l = 0, i = 0;
  while (a < pp){
    a += p[i];
    i++;
  }
  l = i;
  int h = N - 2;
  while (a < 1.0 - pp){
    a += p[i];
    i++;
  }
  h = i;
  CountProbGLA::c_int cf = {x[l], x[h]};
  return cf;
}

 void GetResult(int &a, int &b){
   double conf = 0.95;  
   Finalize(); // not automatically called
   CountProbGLA::c_int cf = ConfidenceInterval (conf);
   a = cf.a;
   b = cf.b;
 }

};

#endif // _SUMPROBKOLMOGOROVDEPTHGLA_H_ 
