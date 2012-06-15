/*
 * MLE.cpp
 *
 *  Created on: Nov 21, 2011
 *      Author: atodor
 */

#include "MLE.h"
#include "gsl_opt.h"
#include <math.h>
#include <gsl/gsl_sf.h>
#include <iostream>

double log_fact(double k)
{
  double r = 0.0;
  for (int i = 2; i <= k; i++)
    r += log(i);
  return r;
}

double mle_Poisson_est(const vector<double>& data)
{
	double lambda = 0;
	double n = 0;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		lambda += i * data[i];
		n += data[i];
	}
	lambda /= n;
	return lambda;
}

double mle_Poisson_val(const vector<double>& data, double *lambda)
{
  double s1 = 0.0, s2 = 0.0, s3 = 0.0;
  for (unsigned int i = 0; i < data.size(); i++)
	{
	  s1 += data[i];
	  s2 += i * data[i];
	  s3 += data[i] * log_fact(i);
	}
  *lambda = s2 / s1;
  return log(*lambda)*s2 - *lambda * s1 - s3;
}

double mle_exponential_est(const vector<double>& data)
{
  return 1.0 / mle_Poisson_est(data);
}

double mle_exponential_val(const vector<double>& data, double *lambda)
{
  double s1 = 0.0, s2 = 0.0;
  for (unsigned int i = 0; i < data.size(); i++)
  {
	s2 += i * data[i];
 	s1 += data[i];
  }
  *lambda = s1 / s2;
  return log(*lambda) * s1 - *lambda * s2;;
}

void mle_gamma_est(const vector<double>& data, double *k, double *theta)
{
  double s1 = 0.0, s2 = 0.0, s3 = 0.0;
  for (unsigned int i = 0; i < data.size(); i++)
  {
     s1 += data[i];
     s2 += i * data[i];
     s3 += log(i) * data[i];
  }
  double c = log(s2) - log(s1) - s3 / s1;
  *k = gamma_solver(c);
  *theta = s2 / (*k * s1);
}

double mle_gamma_val(const vector<double>& data, double *k, double *theta)
{
  double s1 = 0.0, s2 = 0.0, s3 = 0.0;
  for (unsigned int i = 1; i < data.size(); i++)
  {
     s1 += data[i];
     s2 += i * data[i];
     s3 += log(i) * data[i];
  }
  //  double s1 = s2 = s3 = 0.0:
  double c = log(s2) - log(s1) - s3 / s1;
  *k = gamma_solver(c);
  *theta = s2 / (*k * s1);
  return (*k - 1) * s3 - 1.0 / *theta * s2 - *k * log(*theta) * s1 - log(gsl_sf_gamma(*k)) * s1;
  //  return (*k - 1) * s3 - *k * s1 - *k * log(s2 / (*k * s1)) - s1 * log(gsl_sf_gamma(*k));
  //return 5;
}

double mle_power_law_est(const vector<double>& data, int xmin)
{
  struct {double s1, s2;} p = {0.0, 0.0};
  for (unsigned int i = xmin; i < data.size(); i++)
  {
     p.s1 += data[i];
     p.s2 += log(i) * data[i];
  }
  return zeta_solver(&p);
}

double mle_power_law_val(const vector<double>& data, int xmin, double *gamma)
{
  struct {double s1, s2;} p = {0.0, 0.0};
  for (unsigned int i = xmin; i < data.size(); i++)
  {
     p.s1 += data[i];
     p.s2 += log(2 * i) * data[i];
  }
  //  cout << p.s1 << ", " << p.s2 << endl;
  *gamma = 1.0 + p.s1 / p.s2;//zeta_solver(&p);
  return -nlk_power_law(*gamma, &p);
}

double mle_power_law_cont_val(const vector<double>& data, int xmin, double *gamma)
{
  struct {double s1, s2;} p = {0.0, 0.0};
  for (unsigned int i = xmin; i < data.size(); i++)
    // if (data[i] >= 1.0)
  {
     p.s1 += data[i];
     p.s2 += log(i) * data[i];
  }
  //  cout<<"Power law: "<< p.s1 << ", " << p.s2 << endl;
  *gamma = p.s1 / p.s2 + 1;
  //  cout << "Gamma: "<< *gamma << endl;
  double lk =  log(*gamma - 1) * p.s1 - *gamma * p.s2;
  //  cout << "Log Likelihood: "<<lk<< endl;
  return lk;
}

double mle_lognormal_val(const vector<double>& data, double *mu, double *sigma2)
{
  double s1 = 0.0, s2 = 0.0, s3 = 0.0;
  for (unsigned int i = 1; i < data.size(); i++)
  {
    //    cout << i<< ": "<< data[i]<< endl;
     s1 += data[i];
     s2 += log(i) * data[i];
     s3 += log(i) * log(i) * data[i];
  }
  *mu = s2 / s1;
  *sigma2= (s3 - 2.0 * *mu * s2 + *mu * *mu * s1) / s1;
  //  cout << "s1="<< s1 << " s2=" << s2 << " s3=" << s3 <<  endl;
  
  return 
- s1 * log(sqrt(2.0 * M_PI * *sigma2)) 
- s1* *mu * *mu / 2.0 / *sigma2 * s2
- s2 
+ s2 * *mu / *sigma2 
- s3 / 2.0 / *sigma2;
  /*
  double r = 
-s2 
- s1 * log(sqrt(2.0 * M_PI * *sigma2)) 
- s3 / 2 / *sigma2 
+ s2 * *mu / *sigma2 
- s1 * *mu * *mu / 2.0 / *sigma2;
  //  cout << r<<" ";
  return r;
  */
}

double mle_stretched_exponential_val(const vector<double>& data, double *lambda, double *beta)
{
  int n = data.size();
  double p[n+1];
  p[0] = n - 1;
  for (unsigned int i = 1; i < data.size(); i++)
    p[i] = data[i];
  *beta = stretched_exponential_solver(p);
  double s1 = 0.0, s2 = 0.0;
  for (unsigned int i = 0; i < data.size(); i++)
  {
     s1 += data[i];
     s2 += pow(i, *beta) * data[i];
  }
  *lambda = s1 / s2;
  return - nlk_stretched_exponential(*lambda, p);
}

double mle_lognormal3_val(const vector<double>& data, double *m, double *sigma2, double *theta)
{
  int n = data.size();
  double p[n+1];
  p[0] = n;
  for (unsigned int i = 0; i < data.size(); i++)
    p[i+1] = data[i];
  *theta = lognormal3_solver(p);
  *m = exp(p[n]); //mu_fcn(*theta, p);
  *sigma2 = p[n + 1]; //sigma2_fcn(*theta, *mu, p);
  return - nlk_lognormal3(*theta, p);
}
