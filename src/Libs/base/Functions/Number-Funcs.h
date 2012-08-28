#ifndef _NUMBER_FUNCS_H_
#define _NUMBER_FUNCS_H_

// Function descriptions for the built-in numeric types.

#include "base/Types/INT.h"
#include "base/Types/BIGINT.h"
#include "base/Types/FLOAT.h"
#include "base/Types/DOUBLE.h"
#include "base/Types/bool.h"
#include "HashFunctions.h"

// ***** Constructors *****
// FUNC_DEF(</DOUBLE/>, </(val, FLOAT)/>, </DOUBLE/>)
// FUNC_DEF(</DOUBLE/>, </(val, BIGINT)/>, </DOUBLE/>)
// FUNC_DEF(</DOUBLE/>, </(val, INT)/>, </DOUBLE/>)
// FUNC_DEF(</FLOAT/>, </(val, DOUBLE)/>, </FLOAT/>)
// FUNC_DEF(</FLOAT/>, </(val, BIGINT)/>, </FLOAT/>)
// FUNC_DEF(</FLOAT/>, </(val, INT)/>, </FLOAT/>)
// FUNC_DEF(</BIGINT/>, </(val, DOUBLE)/>, </BIGINT/>)
// FUNC_DEF(</BIGINT/>, </(val, FLOAT)/>, </BIGINT/>)
// FUNC_DEF(</BIGINT/>, </(val, INT)/>, </BIGINT/>)
// FUNC_DEF(</INT/>, </(val, DOUBLE)/>, </INT/>)
// FUNC_DEF(</INT/>, </(val, FLOAT)/>, </INT/>)
// FUNC_DEF(</INT/>, </(val, BIGINT)/>, </INT/>)

// ***** Operator == *****
// OP_DEF(==, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(==, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(==, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(==, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(==, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(==, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(==, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(==, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(==, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(==, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(==, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(==, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(==, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(==, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(==, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(==, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator != *****
// OP_DEF(!=, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(!=, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(!=, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(!=, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(!=, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(!=, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(!=, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(!=, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(!=, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(!=, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(!=, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(!=, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(!=, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(!=, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(!=, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(!=, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator > *****
// OP_DEF(>, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(>, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(>, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(>, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(>, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(>, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator < *****
// OP_DEF(<, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(<, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(<, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(<, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(<, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(<, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator >= *****
// OP_DEF(>=, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>=, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(>=, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(>=, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(>=, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>=, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>=, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>=, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(>=, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>=, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>=, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>=, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(>=, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(>=, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(>=, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(>=, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator <= *****
// OP_DEF(<=, </(x, DOUBLE), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<=, </(x, DOUBLE), (y, FLOAT)/>, </bool/>)
// OP_DEF(<=, </(x, DOUBLE), (y, BIGINT)/>, </bool/>)
// OP_DEF(<=, </(x, DOUBLE), (y, INT)/>, </bool/>)
// OP_DEF(<=, </(x, FLOAT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<=, </(x, FLOAT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<=, </(x, FLOAT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<=, </(x, FLOAT), (y, INT)/>, </bool/>)
// OP_DEF(<=, </(x, BIGINT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<=, </(x, BIGINT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<=, </(x, BIGINT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<=, </(x, BIGINT), (y, INT)/>, </bool/>)
// OP_DEF(<=, </(x, INT), (y, DOUBLE)/>, </bool/>)
// OP_DEF(<=, </(x, INT), (y, FLOAT)/>, </bool/>)
// OP_DEF(<=, </(x, INT), (y, BIGINT)/>, </bool/>)
// OP_DEF(<=, </(x, INT), (y, INT)/>, </bool/>)

// ***** Operator + *****
// OP_DEF(+, </(x, DOUBLE), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(+, </(x, DOUBLE), (y, FLOAT)/>, </DOUBLE/>)
// OP_DEF(+, </(x, DOUBLE), (y, BIGINT)/>, </DOUBLE/>)
// OP_DEF(+, </(x, DOUBLE), (y, INT)/>, </DOUBLE/>)
// OP_DEF(+, </(x, FLOAT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(+, </(x, FLOAT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(+, </(x, FLOAT), (y, BIGINT)/>, </FLOAT/>)
// OP_DEF(+, </(x, FLOAT), (y, INT)/>, </FLOAT/>)
// OP_DEF(+, </(x, BIGINT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(+, </(x, BIGINT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(+, </(x, BIGINT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(+, </(x, BIGINT), (y, INT)/>, </BIGINT/>)
// OP_DEF(+, </(x, INT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(+, </(x, INT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(+, </(x, INT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(+, </(x, INT), (y, INT)/>, </INT/>)

// ***** Operator - *****
// OP_DEF(-, </(x, DOUBLE), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(-, </(x, DOUBLE), (y, FLOAT)/>, </DOUBLE/>)
// OP_DEF(-, </(x, DOUBLE), (y, BIGINT)/>, </DOUBLE/>)
// OP_DEF(-, </(x, DOUBLE), (y, INT)/>, </DOUBLE/>)
// OP_DEF(-, </(x, FLOAT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(-, </(x, FLOAT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(-, </(x, FLOAT), (y, BIGINT)/>, </FLOAT/>)
// OP_DEF(-, </(x, FLOAT), (y, INT)/>, </FLOAT/>)
// OP_DEF(-, </(x, BIGINT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(-, </(x, BIGINT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(-, </(x, BIGINT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(-, </(x, BIGINT), (y, INT)/>, </BIGINT/>)
// OP_DEF(-, </(x, INT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(-, </(x, INT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(-, </(x, INT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(-, </(x, INT), (y, INT)/>, </INT/>)

// ***** Operator * *****
// OP_DEF(*, </(x, DOUBLE), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(*, </(x, DOUBLE), (y, FLOAT)/>, </DOUBLE/>)
// OP_DEF(*, </(x, DOUBLE), (y, BIGINT)/>, </DOUBLE/>)
// OP_DEF(*, </(x, DOUBLE), (y, INT)/>, </DOUBLE/>)
// OP_DEF(*, </(x, FLOAT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(*, </(x, FLOAT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(*, </(x, FLOAT), (y, BIGINT)/>, </FLOAT/>)
// OP_DEF(*, </(x, FLOAT), (y, INT)/>, </FLOAT/>)
// OP_DEF(*, </(x, BIGINT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(*, </(x, BIGINT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(*, </(x, BIGINT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(*, </(x, BIGINT), (y, INT)/>, </BIGINT/>)
// OP_DEF(*, </(x, INT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(*, </(x, INT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(*, </(x, INT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(*, </(x, INT), (y, INT)/>, </INT/>)

// ***** Operator / *****
// OP_DEF(/, </(x, DOUBLE), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(/, </(x, DOUBLE), (y, FLOAT)/>, </DOUBLE/>)
// OP_DEF(/, </(x, DOUBLE), (y, BIGINT)/>, </DOUBLE/>)
// OP_DEF(/, </(x, DOUBLE), (y, INT)/>, </DOUBLE/>)
// OP_DEF(/, </(x, FLOAT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(/, </(x, FLOAT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(/, </(x, FLOAT), (y, BIGINT)/>, </FLOAT/>)
// OP_DEF(/, </(x, FLOAT), (y, INT)/>, </FLOAT/>)
// OP_DEF(/, </(x, BIGINT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(/, </(x, BIGINT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(/, </(x, BIGINT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(/, </(x, BIGINT), (y, INT)/>, </BIGINT/>)
// OP_DEF(/, </(x, INT), (y, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(/, </(x, INT), (y, FLOAT)/>, </FLOAT/>)
// OP_DEF(/, </(x, INT), (y, BIGINT)/>, </BIGINT/>)
// OP_DEF(/, </(x, INT), (y, INT)/>, </INT/>)

// ***** Operator Unary + *****
// OP_DEF(+, </(x, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(+, </(x, FLOAT)/>, </FLOAT/>)
// OP_DEF(+, </(x, BIGINT)/>, </BIGINT/>)
// OP_DEF(+, </(x, INT)/>, </INT/>)

// ***** Operator Unary - *****
// OP_DEF(-, </(x, DOUBLE)/>, </DOUBLE/>)
// OP_DEF(-, </(x, FLOAT)/>, </FLOAT/>)
// OP_DEF(-, </(x, BIGINT)/>, </BIGINT/>)
// OP_DEF(-, </(x, INT)/>, </INT/>)

// ***** Hash functions *****
// FUNC_DEF(</Hash/>, </(val, DOUBLE)/>, </BIGINT/>)
// FUNC_DEF(</Hash/>, </(val, FLOAT)/>, </BIGINT/>)
// FUNC_DEF(</Hash/>, </(val, BIGINT)/>, </BIGINT/>)
// FUNC_DEF(</Hash/>, </(val, INT)/>, </BIGINT/>)

// Function to hash a new value into an existing hash
// FUNC_DEF(</Hash/>, </(newVal, BIGINT), (hash, BIGINT)/>, </BIGINT/>)
inline
BIGINT Hash( const BIGINT newVal, const BIGINT hash ) {
    return CongruentHash( newVal, hash );
}

#endif // _NUMBER_FUNCS_H_
