#ifndef _SUMPROBMOMENTSGLA_H_ 
#define _SUMPROBMOMENTSGLA_H_ 


/** Info for the meta-GLAs
 * GLA_DESC
 *
 * NAME(</SumProbMomentsGLA/>)
 * INPUTS(</(v, INT), (p, DOUBLE)/>)
 * RESULT_TYPE(</state/>)
 *
 * END_DESC
 */

#include <gsl/gsl_sf_gamma.h>
#include <mixture.h>

#define N 3
#define O (2*N)


class SumProbMomentsGLA {
  // cumulants of the true distribution
  long double cm[O];
  // moments of the true distribution 
  double nu[O]; 

  double lambda;
  double mu[N];
  double pi[N];

  int status;

  typedef struct { double a,b; } c_dbl;
 public:
  SumProbMomentsGLA(void) { for(int i = 0; i < O; i++) cm[i] = 0.0; sumDet = 0.0; n = 0;}

  //update cumulants of the true distribution 
  //with the new data point
  void AddItem(DOUBLE x, DOUBLE p){
    if (p<0.0 || p>1.0)
      return;
    cm[0] += p*x;
    cm[1] += x*x * p*(1.0-p);
    cm[2] += x*x*x * p*(1.0-p)*(1.0-2.0*p);
    cm[3] += x*x*x*x * p*(1.0-p)*(1.0-6.0*p*(1.0-p));
    cm[4] += x*x*x*x*x * p*(1.0-p)*(1.0-2.0*p)*(1.0-12.0*p*(1.0-p));
    cm[5] += x*x*x*x*x*x * p*(1.0-p)*( 1.0+p*(1.0-p)*(120.0*p*(1.0-p)-30.0));
    sumDet += x;
    n++;
  }

  void AddState(SumProbMomentsGLA& o) { for (int i = 0; i < O; i++) cm[i] += o.cm[i]; sumDet += o.sumDet;}
  
  void Finalize(void){

    //compute moments of true distribution from cumulants
    nu[0] = cm[0];
    nu[1] = cm[1] + cm[0]*cm[0];
    nu[2] = cm[2] + 3*cm[1]*cm[0] + cm[0]*cm[0]*cm[0];
    nu[3] = cm[3] + 4*cm[2]*cm[0] + 3*cm[1]*cm[1] + 6*cm[1]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0];
    nu[4] = cm[4] + 5*cm[3]*cm[0] + 10*cm[2]*cm[1] + 10*cm[2]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[0] + 10*cm[1]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0];
    nu[5] = cm[5] + 6*cm[4]*cm[0] + 15*cm[3]*cm[1] + 15*cm[3]*cm[0]*cm[0] + 10*cm[2]*cm[2]*cm[2] + 60*cm[2]*cm[1]*cm[0] + 20*cm[2]*cm[0]*cm[0]*cm[0] + 15*cm[1]*cm[1]*cm[1] + 45*cm[1]*cm[1]*cm[0]*cm[0] + 15*cm[1]*cm[0]*cm[0]*cm[0]*cm[0] + cm[0]*cm[0]*cm[0]*cm[0]*cm[0]*cm[0];

    // find mixture distribution
    mixture(N, nu, &lambda, mu, pi, status);
  }

  float Equal(float a) {
    return GreaterEq(a - 0.5) - GreaterEq(a + 0.5);
  }

  float Greater(float a) {
    return greater(a);
  }

  float GreaterEq(float a) {
    double p = 0.0; 
    for (int i = 0; i < N; i++)
      p += pi[i] * gsl_sf_gamma_inc_Q(1.0 / lambda, a / (lambda * mu[i]));
    return p;   
  }

  c_dbl ConfidenceInterval(float conf){
    c_dbl cf;
    if (status)
      {
	double sigma = pow(sigma2, 0.5);
	double l = gsl_cdf_gaussian_Pinv(c / 2.0, sigma) + mu,
	  h = gsl_cdf_gaussian_Qinv(c / 2.0, sigma) + mu;
	cf = {l, h};
      }
    else
      {
	double pp = (1.0 - conf) / 2.0;
	double l = solve_confidence(N, lambda, mu, pi, pp);
	double h = solve_confidence(N, lambda, mu, pi, 1.0 - pp);
	cf = {l, h};
      }
    return cf;
  }

 void GetResult(DOUBLE &a, DOUBLE &b){
   double conf = 0.95;  
   Finalize(); // not automatically called
   c_dbl cf = ConfidenceInterval (conf);
   a = cf.a;
   b = cf.b;
 }

};

#endif // _SUMPROBMOMENTSGLA_H_ 
