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
 * LIBS(mixture)
 * END_DESC
 */

#include <gsl/gsl_sf_gamma.h>
#include <probdb/mixture/mixture.h>
#include <gsl/gsl_cdf.h>
#include <math.h>

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

  int status;

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
    // we standardize the result to keep it numerically stable

    double sigma = pow(cm[1],0.5);
    double psigma = sigma;
    psigma *= sigma;
    psigma *= sigma;
    double tmp2 = cm[2] / psigma;
    psigma *= sigma;
    double tmp3 = (cm[3] + 3 * cm[1] * cm[1]) / psigma;
    psigma *= sigma;
    double tmp4 = (cm[4] + 10 * cm[2] * cm[1]) / psigma;
    psigma *= sigma;
    double tmp5 = (cm[5] + 15 * cm[3] * cm[1] + 10 * cm[2] * cm[2] * cm[2] + 15 * cm[1] * cm[1] * cm[1]) / psigma;

    nu[0] = 10.0;
    nu[1] = 101.0;
    nu[2] = tmp2 + 1030.0;
    nu[3] = tmp3 + 40.0 * tmp2 + 10600.0;
    nu[4] = tmp4 + 50.0 * tmp3 + 1000.0 * tmp2 + 110000.0;
    nu[5] = tmp5 + 60.0 * tmp4 + 1500.0 * tmp3 + 20000.0 * tmp2 + 1150000.0;

    cout << "Moments: ";
    for(int i=0;i<O;i++)
      cout << nu[i] << "\t";
    cout << endl;
    // find mixture distribution
    mixture(N, nu, &lambda, mu, pi, status);
    cout << "Mixture done" << endl;
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
    c_dbl cf;
    if (status)
      {
	cout << "Switching to normal approximation"<< endl;
	double sigma2 = nu[1] - nu[0] * nu[0];
	double sigma = pow(sigma2, 0.5);
	double l = gsl_cdf_gaussian_Pinv(conf / 2.0, sigma) + nu[0],
	  h = gsl_cdf_gaussian_Qinv(conf / 2.0, sigma) + nu[0];
	l = (l - 10.0) * pow(cm[1], 0.5) + cm[0];
	h = (h - 10.0) * pow(cm[1], 0.5) + cm[0];
	cf = {l, h};
      }
    else
      {
	double pp = (1.0 - conf) / 2.0;
	cout << "lambda: "<< lambda << endl;
	cout << "mu: ";
	for (int i=0; i< N;i++)
	  cout << mu[i]<< " ";
	cout << endl;
	cout << "pi: ";
	for (int i=0; i< N;i++)
	  cout << pi[i]<< " ";
	cout << endl;

	double l = solve_confidence(N, lambda, mu, pi, pp);
	double h = solve_confidence(N, lambda, mu, pi, 1.0 - pp);
	cout << "l=" << l << " h="<< h << endl; 
	l = (l - 10.0) * pow(cm[1], 0.5) + cm[0];
	h = (h - 10.0) * pow(cm[1], 0.5) + cm[0];
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

#endif // _SUMPROBMOMENTSCONFGLA_H_ 
