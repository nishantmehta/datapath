/**
 * Estimate mixture from moments (Lindsay, 2000)
 * 
 * Parameters:
 *
 * n (in): number of mixture components
 * moments (in): vector with moments of the true distributions
 * lambda (out): address where to place the dispersion parameter lambda of the mixture distribution *               (size 1)
 * mu (out): address where to place the vector with the means of the component gamma distributions  *           (size n)
 * pi (out): address where to place the vector with the mixing weights for the mixture distribution *           (size n)
 * 
 * Compile with -lgsl -lblas -llapack -larmadillo
 */

void mixture(int n, double *moments, double *lambda, double *mu, double *pi);

// Solve equation for confidence interval using GSL
double solve_confidence(int n, double lambda, double *mu, double *pi, double p);
