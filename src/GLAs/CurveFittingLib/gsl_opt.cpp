#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_min.h>
#include "gsl_opt.h"
#include <iostream>

using namespace std;

double lk_gamma(double x, void * params)
{
  //  cout << "params: " << (*(double*)params)<< endl;
  return (log(x) - gsl_sf_psi(x) - *(double*)params);
}

double lk_gamma_deriv(double x, void * params)
{
  return (1.0 / x - gsl_sf_psi_1(x));
}

void lk_gamma_fdf (double x, void *params, double *y, double *dy)
{
  *y = (log(x) - gsl_sf_psi(x) - *(double*)params);
  *dy = (1.0 / x - gsl_sf_psi_1(x));
}

double nlk_power_law(double x, void * params)
{
  //  cout << "nlk_power_law" << endl;
  struct pair {double s1, s2;} *p = (struct pair*)params;
  //  cout << x << endl;
  return (x * p->s2 + log(gsl_sf_zeta(x)) * p->s1);
}

double nlk_stretched_exponential(double x, void * params)
{
  //  cout<< "Stretched_exponential: "<<x<< endl;
  double *data = (double*)params;
  int n = data[0];
  double s1 = 0, s2 = 0, s3 = 0;
  for(int i = 1; i < n; i++)
    {
      s1 += data[i];
      s2 += data[i] * pow(i,x);
      s3 += data[i] * log(i);
    }
  return - s1 * log(s1 / s2) - s1 * log(x) - (x - 1) * s3 + s1;
}

double nlk_lognormal3(double x, void * params)
{
  double *data = (double*)params;
  int n = data[0];
  double s1 = 0, s2 = 0, s3 = 0;
  for(int i = floor(x) + 1; i < n; i++)
    {
      cout << i << ": "<<data[i]<<endl;
     s1 += data[i];
     s2 += log(i - x) * data[i];
     s3 += log(i - x) * log(i - x) * data[i];
    }
  double mu = s2 / s1;
  /*
  for(int i = floor(x) + 1; i < n; i++)
     s3 += (log(i - x) - mu) * (log(i - x) - mu) * data[i];
  double sigma2 = s3 / s1;
  */
  double sigma2= (s3 - 2.0 * mu * s2 + mu * mu * s1) / s1;
  cout << sigma2<<" ";
  data[n] = mu;
  data[n + 1] = sigma2;
  double r = s2 + s1 * log(sqrt(2.0 * M_PI * sigma2)) + s3 / 2 / sigma2 - s2 * mu / sigma2 + s2 * mu * mu / 2.0 / sigma2;
  cout << r<<"##########";
  return r;
}

double gamma_solver(double c)
{
       int status;
       int iter = 0, max_iter = 100;
       const gsl_root_fdfsolver_type *T;
       gsl_root_fdfsolver *s;
       double x0, x = 2.0;
       gsl_function_fdf FDF;
       //       struct quadratic_params params = {1.0, 0.0, -5.0};

       // turn off the error handler
       gsl_set_error_handler_off ();
     
       FDF.f = &lk_gamma;
       FDF.df = &lk_gamma_deriv;
       FDF.fdf = &lk_gamma_fdf;
       FDF.params = &c;
     
       T = gsl_root_fdfsolver_newton;
       s = gsl_root_fdfsolver_alloc (T);
       gsl_root_fdfsolver_set (s, &FDF, x);
       // for(double y=0.01;y<20;y=y+0.1)
       //	 cout << y<<"\t"<<lk_gamma(y, &c)<< endl;
       /*       
       printf ("using %s method\n", 
               gsl_root_fdfsolver_name (s));
       
       printf ("%-5s %10s %10s %10s\n",
               "iter", "root", "err", "err(est)");
       */
       do
         {
           iter++;
           status = gsl_root_fdfsolver_iterate (s);
           x0 = x;
           x = gsl_root_fdfsolver_root (s);
           status = gsl_root_test_delta (x, x0, 0, 1e-3);
	   /*  
           if (status == GSL_SUCCESS)
             printf ("Converged:\n");
     
           printf ("%5d %10.7f %+10.7f %10.7f\n",
                   iter, x, x-7.5, x - x0);
	   */       
	 }
       while (status == GSL_CONTINUE && iter < max_iter);
     
       gsl_root_fdfsolver_free (s);
       return x;//return 5;
     }

double zeta_solver(void * p)
{
         int status;
       int iter = 0, max_iter = 100;
       const gsl_min_fminimizer_type *T;
       //          cout << "++++++++++++++++++++++++"<< endl;

       struct pair {double s1, s2;};
       //cout << ((struct pair *)p)->s1 <<"; "  <<((struct pair *)p)->s2 << endl;

       gsl_min_fminimizer *s;
       double m = 2, m_expected = M_PI;
       double a = 1.01, b = 10.0;
       gsl_function F;
       F.function = &nlk_power_law;
       F.params = p;
       
       T = gsl_min_fminimizer_goldensection;
       s = gsl_min_fminimizer_alloc (T);
       //       for(double y=1.01;y<10;y=y+0.1)
       // cout << y<<"\t"<<nlk_power_law(y,p)<< endl;

       gsl_min_fminimizer_set (s, &F, m, a, b);/*
       cout<< "~~~~~~~~~~~~~"<< endl;  
       printf ("using %s method\n",
               gsl_min_fminimizer_name (s));
     
       printf ("%5s [%9s, %9s] %9s %10s %9s\n",
               "iter", "lower", "upper", "min",
               "err", "err(est)");
     
       printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
               iter, a, b,
               m, m - m_expected, b - a);
					       
       do
         {
           iter++;
           status = gsl_min_fminimizer_iterate (s);
     
           m = gsl_min_fminimizer_x_minimum (s);
           a = gsl_min_fminimizer_x_lower (s);
           b = gsl_min_fminimizer_x_upper (s);
     
           status 
             = gsl_min_test_interval (a, b, 0.001, 0.0);
	   
           if (status == GSL_SUCCESS)
             printf ("Converged:\n");
     
           printf ("%5d [%.7f, %.7f] "
                   "%.7f %+.7f %.7f\n",
                   iter, a, b,
                   m, m - m_expected, b - a);
         }
       while (status == GSL_CONTINUE && iter < max_iter);
     
       gsl_min_fminimizer_free (s);
     
       return m;*/
         return -5;
}

double stretched_exponential_solver(void * p)
{
       int status;
       int iter = 0, max_iter = 100;
       const gsl_min_fminimizer_type *T;
       gsl_min_fminimizer *s;
       double m = 2.0, m_expected = M_PI;
       double a = 0.00001, b = 10.0;
       gsl_function F;
       F.function = &nlk_stretched_exponential;
       F.params = p;
       //   for(double y=0.01;y<b;y=y+1)
       // cout << y<<"\t"<<nlk_stretched_exponential(y,p)<< endl;

       T = gsl_min_fminimizer_brent;
       s = gsl_min_fminimizer_alloc (T);
       gsl_min_fminimizer_set (s, &F, m, a, b);
       /*
       printf ("using %s method\n",
               gsl_min_fminimizer_name (s));
     
       printf ("%5s [%9s, %9s] %9s %10s %9s\n",
               "iter", "lower", "upper", "min",
               "err", "err(est)");
     
       printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
               iter, a, b,
               m, m - m_expected, b - a);
       */
       do
         {
           iter++;
           status = gsl_min_fminimizer_iterate (s);
     
           m = gsl_min_fminimizer_x_minimum (s);
           a = gsl_min_fminimizer_x_lower (s);
           b = gsl_min_fminimizer_x_upper (s);
     
           status 
             = gsl_min_test_interval (a, b, 0.001, 0.0);
	   /*  
           if (status == GSL_SUCCESS)
             printf ("Converged:\n");
     
           printf ("%5d [%.7f, %.7f] "
                   "%.7f %+.7f %.7f\n",
                   iter, a, b,
                   m, m - m_expected, b - a);*/
         }
	 while (status == GSL_CONTINUE && iter < max_iter);
     
       gsl_min_fminimizer_free (s);
     
       return m;
}

double lognormal3_solver(void * p)
{
       int status;
       int iter = 0, max_iter = 100;
       const gsl_min_fminimizer_type *T;
       gsl_min_fminimizer *s;
       double m = 2.0, m_expected = M_PI;
       double a = 0.0, b = 10.0;
       gsl_function F;
       F.function = &nlk_lognormal3;
       F.params = p;
     
       T = gsl_min_fminimizer_brent;
       s = gsl_min_fminimizer_alloc (T);
       gsl_min_fminimizer_set (s, &F, m, a, b);
       /*
       printf ("using %s method\n",
               gsl_min_fminimizer_name (s));
     
       printf ("%5s [%9s, %9s] %9s %10s %9s\n",
               "iter", "lower", "upper", "min",
               "err", "err(est)");
     
       printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
               iter, a, b,
               m, m - m_expected, b - a);
       */
       do
         {
           iter++;
           status = gsl_min_fminimizer_iterate (s);
     
           m = gsl_min_fminimizer_x_minimum (s);
           a = gsl_min_fminimizer_x_lower (s);
           b = gsl_min_fminimizer_x_upper (s);
     
           status 
             = gsl_min_test_interval (a, b, 0.001, 0.0);
	   /*
           if (status == GSL_SUCCESS)
             printf ("Converged:\n");
     
           printf ("%5d [%.7f, %.7f] "
                   "%.7f %+.7f %.7f\n",
                   iter, a, b,
                   m, m - m_expected, b - a);*/
         }
       while (status == GSL_CONTINUE && iter < max_iter);
     
       gsl_min_fminimizer_free (s);
     
       return m;
}
