#ifndef _HSTRING_FUNCS_H_
#define _HSTRING_FUNCS_H_

#include "base/Types/HString.h"
#include "base/Types/STRING_LITERAL.h"
#include "base/Types/bool.h"
#include "base/Types/BIGINT.h"

// ***** Constructor *****
// FUNC_DEF(</HString/>, </(aux, STRING_LITERAL)/>, </HString/>)

// ***** Operators *****
// OP_DEF(==, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(!=, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(>, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(<, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(>=, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(<=, </(s1, HString), (s2, HString)/>, </bool/>)
// OP_DEF(+, </(s1, HString), (s2, HString)/>, </HString/>)

// ***** Hash Function *****
// FUNC_DEF(</Hash/>, </(s, HString)/>, </BIGINT/>)

#endif // _HSTRING_FUNCS_H_
