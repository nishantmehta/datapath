#ifndef _DATE_FUNCS_H_
#define _DATE_FUNCS_H_

// This file serves mostly to house the descriptions for the functions for
// the DATE type. They have been split into this file in order to ensure that
// all the types required by the functions have been defined first.

#include "base/Types/DATE.h"
#include "base/Types/INT.h"
#include "base/Types/STRING_LITERAL.h"
#include "base/Types/bool.h"

// ** Constructors **
// FUNC_DEF(</DATE/>, </(julianDay, INT)/>, </DATE/>)
// FUNC_DEF(</DATE/>, </(year, INT), (month, INT), (day, INT)/>, </DATE/>)
// FUNC_DEF(</DATE/>, </(year, INT), (yearDay, INT)/>, </DATE/>)
// FUNC_DEF(</DATE/>, </(str, STRING_LITERAL)/>, </DATE/>)

// ** Operators **
// OP_DEF(==, </(d1, DATE), (d2, DATE)/>, </bool/>)
// OP_DEF(<, </(d1, DATE), (d2, DATE)/>, </bool/>)
// OP_DEF(<=, </(d1, DATE), (d2, DATE)/>, </bool/>)
// OP_DEF(>, </(d1, DATE), (d2, DATE)/>, </bool/>)
// OP_DEF(>=, </(d1, DATE), (d2, DATE)/>, </bool/>)
// OP_DEF(+, </(d1, DATE), (dateOffset, INT)/>, </DATE/>)
// OP_DEF(-, </(d1, DATE), (dateOffset, INT)/>, </DATE/>)

// ** Functions **

// FUNC_DEF(</GetYear/>, </(d, DATE)/>, </INT/>)
inline
INT GetYear( const DATE & d ) {
    return d.GetYear();
}

// FUNC_DEF(</GetQuarter/>, </(d, DATE)/>, </INT/>)
inline
INT GetQuarter( const DATE & d ) {
    return d.GetQuarter();
}

// FUNC_DEF(</GetMonth/>, </(d, DATE)/>, </INT/>)
inline
INT GetMonth( const DATE & d ) {
    return d.GetMonth();
}

// FUNC_DEF(</GetDay/>, </(d, DATE)/>, </INT/>)
inline
INT GetDay( const DATE & d ) {
    return d.GetDay();
}

// FUNC_DEF(</GetDayOfYear/>, </(d, DATE)/>, </INT/>)
inline
INT GetDayOfYear( const DATE & d ) {
    return d.GetDayOfYear();
}

// FUNC_DEF(</GetWeekOfYear/>, </(d, DATE)/>, </INT/>)
inline
INT GetWeekOfYear( const DATE & d ) {
    return d.GetWeekOfYear();
}

// FUNC_DEF(</GetYearForWeekOfYear/>, </(d, DATE)/>, </INT/>)
inline
INT GetYearForWeekOfYear( const DATE & d ) {
    return d.GetYearForWeekOfYear();
}

// FUNC_DEF(</GetDayOfWeek/>, </(d, DATE)/>, </INT/>)
inline
INT GetDayOfWeek( const DATE & d ) {
    return d.GetDayOfWeek();
}

// FUNC_DEF(</GetJulianDay/>, </(d, DATE)/>, </INT/>)
inline
INT GetJulianDay( const DATE & d ) {
    return d.GetJulianDay();
}

#endif
