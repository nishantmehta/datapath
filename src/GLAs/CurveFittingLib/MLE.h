/*
 * MLE.h
 *
 *  Created on: Nov 21, 2011
 *      Author: atodor
 */

#include <vector>

#ifndef MLE_H_
#define MLE_H_

using namespace std;

double mle_Poisson_val(const vector<double>& data, double *lambda);
double mle_exponential_val(const vector<double>& data, double *lambda);
double mle_gamma_val(const vector<double>& data, double *k, double *theta);
double mle_power_law_val(const vector<double>& data, int xmin, double *gamma);
double mle_power_law_cont_val(const vector<double>& data, int xmin, double *gamma);
double mle_lognormal_val(const vector<double>& data, double *mu, double *sigma2);
double mle_stretched_exponential_val(const vector<double>& data, double *lambda, double *beta);

#endif /* MLE_H_ */
