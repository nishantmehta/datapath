#ifndef _ARMA_MATRIX_H_
#define _ARMA_MATRIX_H_

// This file is mostly to contain the descriptions of the various types and
// operations brought in by the Armadillo library.
// This particular file focuses on the Matrix datatype.
//
// This datatype requires the base library be loaded.

#include <armadillo>
#include "DOUBLE.h"
#include "FLOAT.h"

/**
 *  TYPE_DESC
 *      NAME(</MATRIX/>)
 *  END_DESC
 */
typedef Mat<DOUBLE> MATRIX;
/**
 *  TYPE_DESC
 *      NAME(</FMATRIX/>)
 *  END_DESC
 */
typedef Mat<FLOAT> FMATRIX;

#endif // _ARMA_MATRIX_H_
