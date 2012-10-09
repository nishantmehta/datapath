#ifndef _MACADDR_FUNCS_H_
#define _MACADDR_FUNCS_H_

#include "base/Types/STRING_LITERAL.h"
#include "base/Types/BIGINT.h"
#include "base/Types/bool.h"
#include "base/Types/MACADDR.h"

// ***** Constructors *****
// FUNC_DEF(</macAddr/>, </(addr, STRING_LITERAL)/>, </macAddr/>)

// ***** Operators *****
// OP_DEF(==, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)
// OP_DEF(!=, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)
// OP_DEF(<, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)
// OP_DEF(<=, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)
// OP_DEF(>, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)
// OP_DEF(>=, </(mac1, macAddr), (mac2, macAddr)/>, </bool/>)

// ***** Functions *****
// FUNC_DEF(</Hash/>, </(val, macAddr)/>, </BIGINT/>)

#endif // _MACADDR_FUNCS_H_
