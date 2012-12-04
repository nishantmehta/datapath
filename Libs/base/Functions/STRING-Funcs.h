//
//  Copyright 2012 Christopher Dudley
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef _BASE_STRING_FUNCS_H_
#define _BASE_STRING_FUNCS_H_

#include <string.h>

#include "base/Types/STRING.h"
#include "base/Types/BIGINT.h"
#include "base/Types/INT.h"
#include "base/Types/STRING_LITERAL.h"
#include "base/Types/bool.h"

///// Definitions of functions already defined in STRING.h /////

// ** Constructors **
// FUNC_DEF(</STRING/>, </(str, STRING_LITERAL)/>, </STRING/>)

// FUNC_DEF(</Hash/>, </(str, STRING)/>, </BIGINT/>)

// OP_DEF(==, </(str1, STRING), (str2, STRING)/>, </bool/>)
// OP_DEF(!=, </(str1, STRING), (str2, STRING)/>, </bool/>)
// OP_DEF(>, </(str1, STRING), (str2, STRING)/>, </bool/>)
// OP_DEF(>=, </(str1, STRING), (str2, STRING)/>, </bool/>)
// OP_DEF(<, </(str1, STRING), (str2, STRING)/>, </bool/>)
// OP_DEF(<=, </(str1, STRING), (str2, STRING)/>, </bool/>)

///// New functions and operators /////

// FUNC_DEF(</ToStringLit/>, </(str, STRING)/>, </STRING_LITERAL/>)
inline
STRING_LITERAL ToStringLit( const STRING& str ) {
    return str.ToString();
}

// FUNC_DEF(</Length/>, </(str, STRING)/>, </INT/>)
inline
INT Length( const STRING& str ) {
    return str.Length();
}

#endif // _BASE_STRING_FUNCS_H_
