#ifndef _ARMA_VECTOR_H_
#define _ARMA_VECTOR_H_

// This file is mostly to contain the descriptions of the various types and
// operations brought in by the Armadillo library.
// This particular file focuses on the Vector datatype.
//
// This datatype requires the base library be loaded.

#include <armadillo>
#include "base/Types/DOUBLE.h"
#include "base/Types/FLOAT.h"

/**
 *  TYPE_DESC
 *      NAME(</VECTOR/>)
 *  END_DESC
 */
typedef arma::Col<DOUBLE> VECTOR;
/**
 *  TYPE_DESC
 *      NAME(</FVECTOR/>)
 *  END_DESC
 */
typedef arma::Col<FLOAT> FVECTOR;

#endif // _ARMA_VECTOR_H_
