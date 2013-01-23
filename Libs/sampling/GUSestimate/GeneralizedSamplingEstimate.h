#ifndef _GeneralizedSamplingEstimate_H_
#define _GeneralizedSamplingEstimate_H_


#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <vector>
#include <tr1/unordered_map>

#include "sampling/GUSestimate/HashFct.h"

// EXACTLY ONE OF THE FOLLOWING SHOULD BE UNCOMMENTED

//#define USE_SORT
#define USE_HASH

using namespace std;

ofstream myfile("ys_readings_107.txt");
int order;
int NumberOfSetBits(int i) {
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


template<int k>
struct mytuple {
  uint64_t ch[k];
  double agg;
	
  mytuple(uint64_t _ch[k], double _agg):
    agg(_agg) { memcpy(ch, _ch, k*sizeof(uint64_t));}
  
  // selective constructor. Used to ensure that tuples form groups
  mytuple(mytuple& other, int order):agg(0.0){
    for (int i = 0; i < k; i++) {
      if ((order & (1 << i)) != 0) {
	ch[k-i-1]=other.ch[k-i-1];
      }  else {
	ch[k-i-1]=0ULL;
      }
    }
  }

public:
  friend ostream& operator<<(ostream& Ostr, const mytuple& Tup) {
    Ostr <<"( "<< Tup.ch[0] << ", "<< Tup.ch[1] << ", "<< Tup.ch[2] << "), "<< Tup.agg << endl;
    return Ostr;
  }

  // function to turn to 0 entries not in the set 
  // this will force all these items to be in the same group
  void SelectiveZero(int order){
   
  }

  uint64_t Hash() const {
    // cout << "TPL: " << *this << endl;
    uint64_t hash=HASH_INIT_MOD;
    for (int i = 0; i < k; i++) {
      uint64_t hash1 = hash;
      if ((order & (1 << i)) != 0) {
	hash = CongruentHashModified(ch[k-i-1],hash);
	//cout<<hash<<" = CongHash("<<ch[k-i-1]<<", "<<hash1<<")"<<endl;
      }  
      //cout << "HASH: (" << i << ")=" << hash << endl;
    }
    return hash;
  }

  bool operator==(const mytuple& o) const {
    for (int i = 0; i < k; i++) {
      if ((order & (1 << i)) != 0) {
	if (ch[k-i-1] != o.ch[k-i-1])
	  return false;
      }
    }
    
    return true;
  }

  bool operator<(const mytuple& o) const {
    for (int i = 0; i < k; i++) {
      if ((order & (1 << i)) != 0) {
	//attribute i is included in sorting
	if (ch[k-i -1] < o.ch[k-i-1])
	  return true;
      }
    }
    return false;
  }
};

template<int k>
struct HashKey_tuple {
  size_t operator() (const mytuple<k>& o) const {
    return o.Hash();
  }
};

class GeneralizedSamplingData {
 public:
  /*a - scaling factor for expected value
    It accounts for the outside computed GUS + 
    the bernoullli subsampling.
   */
  double a;
  double a_sub;

  /*b_t - coefficients for the variance terms
    They accounts for the outside computed GUS + 
    the bernoullli subsampling.
   */
  double *b_t;
  double *b_t_sub;

  //number of relations
  int relations_no;
  
  //data -structure that keeps the resized tuples for final computation
  std::vector< mytuple<3> > V;
  
  /*biased y_s - the raw summations obtained from the data*/
  double *coefficients;

  /*unbiased Y_s - computed from y_s*/
  double *unbiased_coefficients;
  
  /*Array indicating whether the the unbiased Y_s has been computed for a particualr s */
  int *is_unbiased_computed;
  

  double expectation_estimate;
  
  int tuples_in;
  int is_variance_stable;
  int deletes_no;
  int tuples_stable;

  

  /* General constructor for outside computed GUS */
  /* Takes the a & b_t parameters for the outside computed GUS and */
  /* combines them with the bernoulli subsampling parameters to get the */
  /* ultimate a & b_t parameters*/
  /* Note: If there has been no subsampling, then the bernoulli parameters are all 1*/
  /* and the ultimate parameters are equal to the input parameters*/

  GeneralizedSamplingData(int numR, double _a, double* _b, double p, std::vector< mytuple<3> > _V) {
    a = _a;
    a_sub = _a * pow(p, numR);
    cout<<" a after resampling= "<< a_sub<< endl;
    relations_no = numR;
    int numCoef = 1<< numR;
    b_t = new double[numCoef];
    b_t_sub = new double[numCoef];
    for (int i=0; i<numCoef; i++) {
      int size = NumberOfSetBits(i);
      b_t[i] = _b[i];
      b_t_sub[i] =_b[i] * pow(p, 2*relations_no - size);
      cout<<" b_t"<<i<<" after resampling = "<< b_t_sub[i]<< endl;
    }
    this->relations_no = relations_no;
    
    V = _V;

	coefficients = new (std::nothrow) double[1 << this->relations_no];
	if (coefficients == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	unbiased_coefficients = new (std::nothrow) double[1 << this->relations_no];
	if (unbiased_coefficients == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	is_unbiased_computed = new (std::nothrow) int[1 << this->relations_no];
	if (is_unbiased_computed == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	for (int i = 0; i < (1 << this->relations_no); i++)
	{
		coefficients[i] = 0.0;
		unbiased_coefficients[i] = 0.0;
		is_unbiased_computed[i] = 0;
	}

  }

~GeneralizedSamplingData() {
	delete [] coefficients;
	coefficients = NULL;

	delete [] unbiased_coefficients;
	unbiased_coefficients = NULL;

	delete [] is_unbiased_computed;
	is_unbiased_computed = NULL;

	relations_no = 0;
}
	

  /* computes biased Y_s for a given s - sort based implementation. Running slow */
 double ComputeOrderCoefSortBased(int _order){
   order = _order;
   cout << "Begin SORTING " << _order << endl;	  
   sort(V.begin(), V.end());
   cout << "END SORTING " << _order << endl;	  
   
   //cout << "After sorting on"<< order<< endl;
   //B.printtuples();
   
   double res = 0.0; // overall result
   if ( V.size() == 0) {
     cout << "No tuples subsampled for variance estimation" << endl;
     exit(1);
   }
   double p_res = V[0].agg; // group aggregate
   //cout << "Initial p_res = " <<p_res<< endl;
   
   for (size_t i=1; i<V.size(); i++){
     // new group?
     if (V[i-1] < V[i]){ // yes
       res+=p_res*p_res;
       p_res = V[i].agg;
     } else { // same group
       p_res += V[i].agg;
     }
     
   }
   res += p_res*p_res;
   //cout <<"biased y_"<< order << " = " << res << endl; 
   return res;
 }

  /* computes biased Y_s for a given s - hash based imlementation*/
 double ComputeOrderCoef(int _order){
   order = _order;
   double res = 0.0;
   
   cout << "Order=" << _order << endl;

   typedef std::tr1::unordered_map < mytuple<3>, double, HashKey_tuple<3> > MapType;
   MapType mymap;
   
   for (size_t i = 0; i <V.size(); i++) {
     const mytuple<3> key=V[i];

     if (mymap.find(key)==mymap.end())
       mymap.insert(make_pair(key,V[i].agg));
     else
       mymap[key] += V[i].agg;
   }
   cout<<endl;
   
   MapType::iterator it;
   for ( it=mymap.begin(); it!=mymap.end(); ++it){
     double val=it->second;
     // cout<< " + "<< val;
     res+=val*val;
   }
   cout<<endl;
  
   return res;
 }


  /* computes biased y_s for all s and stores them in coefficients*/
  void GenerateStatistics() {
    for (int S = 0; S < (1 << relations_no); S++) {
      int size = NumberOfSetBits(S);
#ifdef USE_HASH
      coefficients[S] = ComputeOrderCoef(S);
#endif

#ifdef USE_SORT
      coefficients[S] = ComputeOrderCoefSortBased(S);
#endif
      if (myfile.is_open()){
	myfile <<setprecision(15) << coefficients[S] << "\t";
	cout <<"Raw coefficients"<< setprecision(15)<< coefficients[S]<< endl;
	//myfile.close();
      }
      else {
	cout << "Unable to open file ys_readings";
      }	
    }
    return;

    if (myfile.is_open()) {
      myfile << "\n";
      myfile.close(); 
    }
    else {
      cout<< "Unable to open file ys_readings";
    }
  }
		
  /* computes C_s for a given s*/
  double ComputeCCoeff(int S) {
    double res = 0.0;
    //printf("C(%d)\n", S);
    int T = 0;
    while (T <= S) {
      //skip over T not in POWER(S)
      if ((T & ~S) != 0) {
	T += 1;
	continue;
      }		
      int ip = S & ~T;
      double coeff = 1.0;
      for (int i = 0; i < relations_no; i++) {
	// now treat bit in scan as in the big variance 
	//since these are variance terms
	if ((ip & (1 << i)) != 0)
	  coeff *= -1.0;
      }
      res += coeff * b_t[T];
      //printf("%d : %f %f\n", T, coeff, b_t[T]);
      T += 1;
    }
    return res;
  }
  
  /*computes C_s,t for given s and t*/
  double ComputeCCoeff(int S, int T) {
    double res = 0.0;
    //	printf("C(%d,%d)\n", S, T);
    int U = 0;
    while (U <= T) {
      //skip over U not in POWER(T)
      if ((U & ~T) != 0) {
	U += 1;
	continue;
      }
      //ip = T - U
      int ip = T & ~U;
      double coeff = 1.0;
      for (int i = 0; i < relations_no; i++) {
	// now treat bit in scan as in the big variance 
	//since these are variance terms
	if ((ip & (1 << i)) != 0)
	  coeff *= -1.0;
      }
      int s_union_u = U | S;
      res += coeff * b_t_sub[s_union_u];
      //printf("%d : %f %f\n", ip, coeff, b_t[s_union_u]);
      U += 1;
    }
    return res;
  }

  /* computes the unbiased Y_s for a given s*/
  double ComputeUnbiasedCoef(int S)
  {
    //check if the S coefficient is already computed
    if (is_unbiased_computed[S] != 0)
      {
	//cout << "Coefficient " << S << " was computed previously !!!\n";
	double res = unbiased_coefficients[S];
	return res;
      }
    
    //first compute the unbiased coefficients for all the super-terms of the S term
    double unbiased_rest = 0.0;
    
    int scan = S;
    while (scan < (1 << relations_no))
      {
	//enumerates all supersets of S skipping the ones that are not
	for (scan = scan + 1; (scan < (1 << relations_no)) && ((S | scan) == S); scan++);
	scan = scan | S;
	if (scan >= (1 << relations_no))
	  break;
	
	double agg_val = ComputeUnbiasedCoef(scan);
	
	//T = scan - S
	int T = scan & ~S;
	double coeff = ComputeCCoeff(S, T);
	
	unbiased_rest += agg_val * coeff;
      }
    
    //compute the coefficient in front of the S term c_{\empty,S}
    double c_S0 = ComputeCCoeff(S, 0);
    //	double c_S0 = data->ComputeCCoeff(~S & complement_mask);
	
    double unbiased_est = coefficients[S];// / gsl_pow_int(data->a, 2);
    unbiased_est -= unbiased_rest;
    
    //	cout << "Unbiased estimaxte(" << S << "): " << unbiased_est << "\n";
    
    unbiased_est /= c_S0;
    
    //	cout << "Unbiased estimate(" << S << "): " << unbiased_est << "\n";
    
    unbiased_coefficients[S] = unbiased_est;
    is_unbiased_computed[S] = 1;
    
    return unbiased_est;
  }
  
  /*computes the unbiased coefficients for all s*/
  void ComputeUnbiasedCoefs()
  {
    for (int i = 0; i < (1 << relations_no); i++) {
      is_unbiased_computed[i] = 0;
    }
    
    for (int i = 0; i < (1 << relations_no); i++) {
      unbiased_coefficients[i] = ComputeUnbiasedCoef(i);
      is_unbiased_computed[i] = 1;
      cout<< "Unbiased coefficients:" << setprecision(9) <<unbiased_coefficients[i]<<endl;
    }
  }

  /*computes the variance */
  double ComputeVariance() {
    //assume that unbised_coefficients contain most up to date estimates !!!!!!
    
    //this is the square of the expected value
    double var = -unbiased_coefficients[0];
    
    for (int j = 0; j < (1 << relations_no); j++) {
      //compute the coefficient
      double coeff = ComputeCCoeff(j) / pow(a, 2);
      
      var += unbiased_coefficients[j] * coeff;
      
      //cout << j << "\t" << data->threshold << "\t" << coeff  << "\t" << unbiased_coefficients[j] << "\t" << coefficients[j] << "\n";
    }
    
    //	cout << "var=" << var << "\tstd=" << sqrt(var) << endl;
    
    return var;
  }

  
};

#endif //_GeneralizedSamplingEstimate_H_
