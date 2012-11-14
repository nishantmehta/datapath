#ifndef _SUMPROBNORMALGLA_H_ 
#define _SUMPROBNORMALGLA_H_ 

#include <gsl/gsl_cdf.h>

/** Info for the meta-GLAs
 * GLA_DESC
 *
 * NAME(</SumProbNormalGLA/>)
 * INPUTS(</(v, INT),(p, DOUBLE)/>)
 * RESULT_TYPE(</state/>)
 *
 * END_DESC
 */

class SumProbNormalGLA {
  double mu;
  double sigma2;

  typedef struct { double a,b; } c_dbl;
 public:
  SumProbNormalGLA(void) { mu = 0.0; sigma2 = 0.0;}

  //update cumulants of the true distribution 
  //with the new data point
  void AddItem(DOUBLE x, DOUBLE p){
    if (p<0.0 || p>1.0)
      return;
    mu += p*x;
    sigma2 += x*x * p*(1.0 - p);
  }

  void AddState(SumProbKolmogorovGLA& o) { mu += o.mu; sigma2 += o.sigma2; }
  
  void Finalize(void){
  }

  float Equal(float a) {
    return GreaterEq(a - 0.5) - GreaterEq(a + 0.5);
  }

  float Greater(float a) {
    return greater(a);
  }

  float GreaterEq(float a) {
    return 0.5 - 0.5 * erf((a - mu) / pow(2.0 * sigma2, 0.5));   
  }

  c_dbl ConfidenceInterval(float conf){
    double sigma = pow(sigma2, 0.5);
    double l = gsl_cdf_gaussian_Pinv(c / 2.0, sigma) + mu,
      h = gsl_cdf_gaussian_Qinv(c / 2.0, sigma) + mu;
    c_dbl cf = {l, h};
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

#endif // _SUMPROBKOLMOGOROVGLA_H_ 
