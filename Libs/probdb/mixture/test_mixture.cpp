#include <iostream>
#include <math.h>
#include "mixture.h"

#define N 4
#define M (2*N)

using namespace std;
  
int main(int argc, char** argv)
{
  //Set up Gamma mixture distribution
  double lambda = 0.5;
  double mu[N] = {2.0, 3.0, 4.0, 6.0};
  double pi[N] = {0.2, 0.3, 0.4, 0.1};
  double moments[M];
  double prod = 1.0;

  // Compute moments
  // If   X ~ Gamma(1/lambda,mu) 
  // then E(X^k) = (1+lambda)(1+2*lambda)...(1+(k-1)*lambda)*mu^k
  for (int i = 0; i < M; i++)
    {
      prod *= 1.0 + (double)i * lambda;
      moments[i] = 0.0;
      for (int j = 0; j < N; j++) 
	{
	  moments[i] += pi[j] * prod * pow(mu[j], i + 1);
	}
    }

  cout << "Moments:" << endl;
  for (int i = 0; i < M; i++)
    cout << moments[i] << " ";
  cout << endl;

  //estimate mixture parameters from moments (Lindsay, 2000)
  double _lambda;
  double _mu[N];
  double _pi[N];
  mixture(N, moments, &_lambda, _mu, _pi);
  cout << "lambda: " << _lambda << endl;
  cout << "mu: ";
  for (int i = 0; i < N; i++)
    cout << mu[i] << " ";
  cout << endl;
  cout << "pi: " ;
  for (int i = 0; i < N; i++)
    cout << pi[i] << " ";
  cout << endl;
}  
