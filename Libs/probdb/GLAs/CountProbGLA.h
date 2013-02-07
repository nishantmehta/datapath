#ifndef _COUNTPROBGLA_H_
#define _COUNTPROBGLA_H_

/** Info for the meta-GLAs
 * GLA_DESC
 *
 * NAME(</CountProbGLA/>)
 * INPUTS(</(p, FLOAT)/>)
 * OUTPUTS(</(l,INT),(h,INT)/>)
 * RESULT_TYPE(</single/>)
 *
 * LIBS(zPoly)
 * END_DESC
*/

#include "probdb/zPoly/zPoly.h"
#include <iostream>
#include <iomanip>
// #include "base/Types/STATE.h"

class CountProbGLA{
  long int NN;
  zPoly pgf;


 public:

  typedef struct {int a,b;} c_int;

  // constructor, builds empty object
  CountProbGLA(){NN = 0; }

  // incorporate one datapoint into PGF
  // p: probability,x: value
  void AddItem(float p, float x=1.0){
    pgf.AddEvent(p); NN++;
  }

  // merge info or other PGFs into current
  void AddState(CountProbGLA& other){
    pgf.AddState(other.pgf);
  }

  // expand PGF
  void Finalize(){ pgf.ComputePolynomialCoefficients(); }

  void spread(int power){ pgf.spread(power); }

  // compute P[X>a]
  float Greater(float a){
    double r = 0.0;
    for (int i = a; i < NN; i++)
      r += pgf.Coef(i);
    return r;
  }

  // compute P[X>=a]
  float GreaterEq(float a){return Greater(a) + Equal(a);}

  // compute P[X = Y]
  // PGF2 possibly different type
  template<typename PGF2>
    float EqualTo(PGF2& other){
    double r = 0.0;
    for (int i = 0; i <= NN; i++)
      r += pgf.Coef(i) * other.Equal(i);
    return r;
  }

  // compute P[X = a]
  float Equal(double a){return pgf.Coef(a);}

  // compute P[X>Y]
  template<typename PGF2>
    float Greater(PGF2& other){
    double r = 0.0;
    for (int i = 0; i < other.NN; i++)
      r += other.pgf.Coef(i) * Greater(i);
    return r;
  }

  // compute P[X>=Y]
  template<typename PGF2>
    float GreaterEq(PGF2& other){return Greater(other) + Equal(other);}

  // compute confidence interval [l,h]
  // such that P[l<X<h] = conf
  c_int ConfidenceInterval(double conf){
    pgf.ensureComputed();
    std::cout << "Sum of coefficients: " << pgf.SumOfCoefs() << std::endl;
    double p = (1.0 - conf) / 2.0, a = 0.0;
    int l = 0;
    while (a < p){
      a += pgf.Coef(l);
      l++;
    }
    int h = NN-1;
    while (a < p){
      a += pgf.Coef(h);
      h--;
    }
    c_int cf={l, h};
    return cf;
  }

  void GetResult(int &a, int &b){
    double conf= 0.95;
    c_int cf = ConfidenceInterval (conf);
    a=cf.a;
    b=cf.b;
  }
  /*
    class Count{
    zPoly pgf;
    public:
    //AtLeastOne(void){ prod=1.0; }
    void AddItem(DOUBLE p){ pgf.AddItem(p); }
    void AddState(Count& o){ pgf.AddState(o.pgf); }
    void Finalize(){ pgf.Finalize(); }
    void GetResult(PGF& p){ p=pgf; }
    };
  */
};
#endif  // _COUNTPROBGLA_H_
