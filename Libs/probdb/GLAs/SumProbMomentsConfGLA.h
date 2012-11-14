#ifndef _SUMPROBMOMENTSCONFGLA_H_ 
#define _SUMPROBMOMENTSCONFGLA_H_ 


/** Info for the meta-GLAs
 * GLA_DESC
 *
 * NAME(</SumProbMomentsConfGLA/>)
 * INPUTS(</(v, INT), (p, DOUBLE)/>)
 * OUTPUTS(</(l,DOUBLE),(h,DOUBLE)/>)
 * RESULT_TYPE(</single/>)
 *
 * LIBS(mixture,gsl,gslcblas,lapack,armadillo)
 * END_DESC
 */

#include <gsl/gsl_sf_gamma.h>
#include <probdb/mixture/mixture.h>

#define N 3
#define O (2*N)


class SumProbMomentsConfGLA {
  // cumulants of the true distribution
  long double cm[O];
  // moments of the true distribution 
  double nu[O]; 

  double lambda;
  double mu[N];
  double pi[N];
  long double sumProb; // sum of all probabilities for normalization
  uint64_t n;
  long double sumDet;

  typedef struct { double a,b; } c_dbl;
 public:
  SumProbMomentsConfGLA(void) { for(int i = 0; i < O; i++) cm[i] = 0.0; sumProb=0.0; sumDet = 0.0; n = 0;}

  //update cumulants of the true distribution 
  //with the new data point
  void AddItem(DOUBLE x, DOUBLE p){
    if (p<0.0 || p>1.0)
      return;
    sumProb+=p;
    cm[0] += p*x;
    cm[1] += x*x * p*(1.0-p);
    cm[2] += x*x*x * p*(1.0-p)*(1.0-2.0*p);
    cm[3] += x*x*x*x * p*(1.0-p)*(1.0-6.0*p*(1.0-p));
    cm[4] += x*x*x*x*x * p*(1.0-p)*(1.0-2.0*p)*(1.0-12.0*p*(1.0-p));
    cm[5] += x*x*x*x*x*x * p*(1.0-p)*( 1.0+p*(1.0-p)*(120.0*p*(1.0-p)-30.0));
    sumDet += x;
    n++;
  }

  void AddState(SumProbMomentsConfGLA& o) { for (int i = 0; i < O; i++) cm[i] += o.cm[i]; sumProb+=o.sumProb; sumDet += o.sumDet;}
  
  void Finalize(void){

    //compute moments of true distribution from cumulants
    // we scale the result to keep it numerically stable
    nu[0] = cm[0]/sumProb;
    nu[1] = (cm[1] + cm[0]*cm[0])/(sumProb*sumProb);
    nu[2] = (cm[2] + 3*cm[1]*cm[0] + cm[0]*cm[0]*cm[0])/(sumProb*sumProb*sumProb);
    nu[3] = (cm[3] + 4*cm[2]*cm[0] + 3*cm[1]*cm[1] + 6*cm[1]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0])/(sumProb*sumProb*sumProb*sumProb);
    nu[4] = (cm[4] + 5*cm[3]*cm[0] + 10*cm[2]*cm[1] + 10*cm[2]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[0] + 10*cm[1]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0])/
      (sumProb*sumProb*sumProb*sumProb*sumProb);
    nu[5] = (cm[5] + 6*cm[4]*cm[0] + 15*cm[3]*cm[1] + 15*cm[3]*cm[0]*cm[0] + 10*cm[2]*cm[2]*cm[2] + 60*cm[2]*cm[1]*cm[0] + 20*cm[2]*cm[0]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[1] + 45*cm[1]*cm[1]*cm[0]*cm[0] + 15*cm[1]*cm[0]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0]*cm[0])/
      (sumProb*sumProb*sumProb*sumProb*sumProb*sumProb);

    // find mixture distribution
    //  mixture(N, nu, &lambda, mu, pi);
  }

  float Equal(float a) {
    return GreaterEq(a - 0.5) - GreaterEq(a + 0.5);
  }

  float GreaterEq(float a) {
    double p = 0.0; 
    for (int i = 0; i < N; i++)
      p += pi[i] * gsl_sf_gamma_inc_Q(1.0 / lambda, (a/sumProb) / (lambda * mu[i]));
    return p;   
  }

  c_dbl ConfidenceInterval(float conf){
    double pp = (1.0 - conf) / 2.0;
    double l = sumProb*solve_confidence(N, lambda, mu, pi, pp);
    double h = sumProb*solve_confidence(N, lambda, mu, pi, 1.0 - pp);
    c_dbl cf = {l, h};
    return cf;
}

  void GetResult(DOUBLE &a, DOUBLE &b){
   double conf = 0.95;  
   Finalize(); // not automatically called
   // c_dbl cf = ConfidenceInterval (conf);
   a = 1.0;//cf.a;
   b = 2.0;//cf.b;
 }

};

#endif // _SUMPROBMOMENTSCONFGLA_H_ 
