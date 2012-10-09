#ifndef _IPV4ADDR_FUNCS_H_
#define _IPV4ADDR_FUNCS_H_

#include "base/Types/IPV4ADDR.h"
#include "base/Types/bool.h"
#include "base/Types/INT.h"
#include "base/Types/BIGINT.h"
#include "base/Types/STRING_LITERAL.h"

// ***** Constructors *****/
// FUNC_DEF(</IPv4/>, </(c1, INT), (c2, INT), (c3, INT), (c4, INT)/>, </IPv4/>)
// FUNC_DEF(</IPv4/>, </(addr, STRING_LITERAL)/>, </IPv4/>)

// ***** Operators *****/
// OP_DEF(<, </(d1, IPv4), (d2, IPv4)/>, </bool/>)
// OP_DEF(<=, </(d1, IPv4), (d2, IPv4)/>, </bool/>)
// OP_DEF(>, </(d1, IPv4), (d2, IPv4)/>, </bool/>)
// OP_DEF(>=, </(d1, IPv4), (d2, IPv4)/>, </bool/>)
// OP_DEF(==, </(d1, IPv4), (d2, IPv4)/>, </bool/>)
// OP_DEF(!=, </(d1, IPv4), (d2, IPv4)/>, </bool/>)

// ***** Functions *****/
// FUNC_DEF(</Domain/>, </(x, IPv4)/>, </IPv4/>)
// FUNC_DEF(</Hash/>, </(x, IPv4)/>, </BIGINT/>)

// FUNC_DEF(</Between/>, </(addr, IPv4), (lower, IPv4), (upper, IPv4)/>, </bool/>)
inline
bool Between( const IPv4 & addr, const IPv4 & lower, const IPv4 & upper ) {
    return IPv4::Between(addr, lower, upper);
}

#endif // _IPVRADDR_FUNCS_H_
