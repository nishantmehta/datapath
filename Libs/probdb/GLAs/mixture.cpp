#include <iostream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include "armadillo"

#define DEBUG 0

using namespace arma;
using namespace std;

typedef struct 
{
  int dim; 
  double *moments;
} param_struct;

typedef struct 
{
  int n;
  double lambda; 
  double *mu;
  double *pi;
  double p;
} conf_struct;

//error handler
void handler (const char * reason, const char * file, int line, int gsl_errno)
{
  cout << reason << endl;
}

// Construct pseudo-moment matrix
mat construct_matrix(int dim, double lambda, double *moments)
{
  mat a(dim,dim);
  int k = 0;
  double num = 1.0;
  double start_num = 1.0;

#if DEBUG
   cout << "Construct Matrix lambda=" << lambda << endl;
#endif

  // first element
  a(0,0) = 1.0;

  // first row
  for (int j = 1; j < dim; j++)
    {
      num *= 1.0 + ((double)j - 1.0) * lambda;
      a(0,j) = moments[j - 1] / num;
    }

  // rest of the rows
  for (int i = 1; i < dim; i++)
    {
      // first column
      start_num *= 1.0 + ((double)i - 1.0) * lambda;
      a(i,0) = moments[i - 1] / start_num;
      num = start_num;
      // rest of columns 
      for (int j = 1; j < dim; j++)
	{
	  num *= 1.0 + ((double)i + (double)j - 1.0) * lambda;
	  a(i,j) = moments[i + j - 1] / num;

#if DEBUG
	  // cout << i << " " << j << " " << moments[i + j - 1] << " " << num << " " << a(i,j) << endl;
#endif

	}
    }

#if DEBUG
  //a.print();
#endif

  return a;
}

// Equation to solve for finding lambda
// det Delta(lambda) = 0
double det_fn (double lambda, void *params)
{
  double *moments = ((param_struct*)params)->moments;
  double p = ((param_struct*)params)->dim;
  mat a = construct_matrix(p, lambda, moments);
  return det(a);
}

// CDF of gamma mixture
double gamma_cdf (double x, void *params)
{
  conf_struct *conf_params = (conf_struct*)params;
  int n = conf_params->n;
  double lambda = conf_params->lambda;
  double *mu = conf_params->mu;
  double *pi = conf_params->pi;
  double p = conf_params->p;
  double r = 0.0;
  for (int i = 0; i < n; i++)
    r += pi[i] * gsl_cdf_gamma_P (x, 1.0 / lambda, lambda * mu[i]);
  return r - p;
}

// PDF of gamma mixture
double gamma_pdf (double x, void *params)
{
  conf_struct *conf_params = (conf_struct*)params;
  int n = conf_params->n;
  double lambda = conf_params->lambda;
  double *mu = conf_params->mu;
  double *pi = conf_params->pi;
  double p = conf_params->p;
  double r = 0.0;
  for (int i = 0; i < n; i++)
    r += pi[i] * gsl_ran_gamma_pdf (x, 1.0 / lambda, lambda * mu[i]);
  return r;
}

// FDF of gamma mixture
void gamma_fdf (double x, void *params, double *y, double *dy)
{
  conf_struct *conf_params = (conf_struct*)params;
  int n = conf_params->n;
  double lambda = conf_params->lambda;
  double *mu = conf_params->mu;
  double *pi = conf_params->pi;
  double p = conf_params->p;
  double r = 0.0, s = 0.0;
  for (int i = 0; i < n; i++)
    {
      r += pi[i] * gsl_cdf_gamma_P (x, 1.0 / lambda, lambda * mu[i]);
      s += pi[i] * gsl_ran_gamma_pdf (x, 1.0 / lambda, lambda * mu[i]);
    }
  *y = r - p;
  *dy = s;
}

// Solve equation det Delta(lambda) = 0 numerically using GSL
double solve(int dim, double *moments, double limit, int &_status)
{
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  double r = 0;
  double x_lo = 0.0, x_hi = limit;
  gsl_function F;
  param_struct params = {dim, moments};
  F.function = det_fn;
  F.params = &params;
  
  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
    
  int k = 0;
  while (det_fn(x_lo,&params) * det_fn(x_hi,&params) > 0)
  {
    x_lo = 0.0;
    x_hi = ((double)k + 1.0) * limit;
    k++;
    cout << "Enlarging interval" << endl;
    if (k == 100)
      {
	cout << "Error: interval too large";
	return limit;
      }
  }
  
  gsl_set_error_handler (handler);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);
  //cout << "x_lo: " << x_lo << " x_hi: " << x_hi << endl;
  do
    {
      iter++;
      status = gsl_root_fsolver_iterate (s);
      r = gsl_root_fsolver_root (s);
      x_lo = gsl_root_fsolver_x_lower (s);
      x_hi = gsl_root_fsolver_x_upper (s);
      status = gsl_root_test_interval (x_lo, x_hi, 0, 0.001);
    }
  while (status == GSL_CONTINUE && iter < max_iter);
  
  gsl_root_fsolver_free (s);
  _status = status;
  return r;
}

// Solve equation for confidence interval using GSL
double solve_confidence(int n, double lambda, double *mu, double *pi, double p)
{
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fdfsolver_type *T;
  gsl_root_fdfsolver *s;
  double x0, x = mu[0];
  gsl_function_fdf FDF;

  conf_struct conf = {n, lambda, mu, pi, p};

  FDF.f = &gamma_cdf;
  FDF.df = &gamma_pdf;
  FDF.fdf = &gamma_fdf;
  FDF.params = &conf;

  T = gsl_root_fdfsolver_newton;
  s = gsl_root_fdfsolver_alloc (T);
  gsl_set_error_handler (handler);

  gsl_root_fdfsolver_set (s, &FDF, x);
  
  do
    {
      iter++;
      status = gsl_root_fdfsolver_iterate (s);
      x0 = x;
      x = gsl_root_fdfsolver_root (s);
      status = gsl_root_test_delta (x, x0, 0, 1e-3);
    }
  while (status == GSL_CONTINUE && iter < max_iter);
  
  gsl_root_fdfsolver_free (s);
  return x;
}

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

void mixture(int n, double *moments, double *lambda, double *mu, double *pi, int &status)
{
   double m1 = moments[0], m2 = moments[1], m3 = moments[2];
   //double lambda1, lambda2, lambda3;

   //Find lambda

   *lambda = m2 / m1 / m1 - 1.0;

   for (int k = 1; k < n; k++)
     {
#if DEBUG
       cout << "lambda: " << *lambda << endl;
       mat mat1 = construct_matrix(k + 2, 0, moments);
       cout << "Det(0): " << det(mat1) << endl;
       mat mat2 = construct_matrix(k + 2, *lambda, moments);
       cout << "Det(lambda): " << det(mat2) << endl;
       mat2.print();
#endif

       *lambda = solve(k + 2, moments, *lambda, status);
     }   
#if DEBUG
   cout << "lambda3: " << *lambda << endl;
#endif

   //Find component means mu

   mat delta = construct_matrix(n + 1, *lambda, moments);

#if DEBUG
   delta.print();
#endif

   mat delta_inv = delta.i();

#if DEBUG
   delta_inv.print();
#endif

   //Use analytic solution for small polynomials
   if (n <= 3)
     {
       double c1 = delta_inv(3,2)/delta_inv(3,3), 
	 c2 = delta_inv(3,1)/delta_inv(3,3), 
	 c3 = delta_inv(3,0)/delta_inv(3,3);
       int n_roots = gsl_poly_solve_cubic (c1, c2, c3, mu, mu + 1, mu + 2);
       if(n_roots < 3)
	 {
	   cout << "WARNING: complex roots: ";
	   gsl_complex z0, z1, z2;
	   gsl_poly_complex_solve_cubic (c1, c2, c3, &z0, &z1, &z2);
	   *mu = GSL_REAL(z0);
	   *(mu + 1) = GSL_REAL(z1);
	   *(mu + 2) = GSL_REAL(z2);
	   cout << GSL_REAL(z0) << " "  << GSL_IMAG(z0) << endl 
		<< GSL_REAL(z1) << " "  << GSL_IMAG(z1) << endl 
		<< GSL_REAL(z2) << " "  << GSL_IMAG(z2) << endl;
	   cout << "Using real part" << endl;
	 }
     }
   else
     {
       double c[n + 1]; // coefficients
       double s[2 * n]; // solutions
       for (int i = 0; i < n + 1; i++)
	 c[i] = delta_inv(n, i);
       gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc (n + 1);
       gsl_poly_complex_solve (c, n + 1, w, s);
       for (int i = 0; i < n; i++)
	 {
	   *(mu + i) = s[2 * i];
	   if (fabs(s[2 * i + 1]) > 0.1)
	     {
	       cout << "WARNING: complex root: " << s[2 * i] << " + " << s[2 * i + 1] << "i" << endl;
	       cout << "Using real part" << endl;		   
	     }     
	 }
       gsl_poly_complex_workspace_free (w);
     }
   
   //Find mixing weights pi

   mat a(n,n);
   for (int i = 0; i < n; i++)
     for(int j = 0; j < n; j++)
       a(i,j) = pow(mu[j], i);
   colvec b(n);
   b(0) = 1;
   b(1) = m1;
   double num = 1.0;
   for (int i=2;i<n;i++)
     {
       num *= 1.0 + (i - 1)* *lambda;
       b(i) = moments[i - 1] / num;
     }
   colvec p = solve(a,b);
   for (int i = 0; i < n; i++)
     pi[i] = p(i);
}

  
