#ifndef _BERNOULLI_SAMPLING_H_
#define _BERNOULLI_SAMPLING_H_

#include "sampling/GUSestimate/HashFct.h"
#include "base/Types/BIGINT.h"
#include "base/Types/DOUBLE.h"


/* Function to apply Bernoulli Sampling 

   Arguments:
     p: selection probability
     b: seed for CongruentHash
     id: id of tuple to select
*/
// FUNC_DEF(</BernoulliSampling/>,</(p, DOUBLE), (b, BIGINT), (id, BIGINT)/>,</bool/>)
inline 
bool BernoulliSampling(const DOUBLE& p, const BIGINT& b, const BIGINT& id){
  uint64_t hash = CongruentHashModified(id, b);
  return hash <= p*0xFFFFFFFFFFFFFFFFULL;
}


#endif // _BERNOULLI_SAMPLING_H_
