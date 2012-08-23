#ifndef _ARMA_VECTOR_H_
#define _ARMA_VECTOR_H_

// This file is mostly to contain the descriptions of the various types and
// operations brought in by the Armadillo library.
// This particular file focuses on the Vector datatype.
//
// This datatype requires the base library be loaded.

#include <armadillo>
#include "DOUBLE.h"
#include "FLOAT.h"

// TYPE_DESC(</VECTOR/>)
typedef Col<DOUBLE> VECTOR;
// TYPE_DESC(</FVECTOR/>)
typedef Col<FLOAT> FVECTOR;

#endif // _ARMA_VECTOR_H_
