dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
include(GLA-templates.m4)

m4_define(</INNER_GLA/>, </SumGLA/>)dnl
m4_define(</GLA_NAME/>, </GBy_MAC_Sum/>)dnl
m4_define(</SumGLA_INPUT/>, </(x, DOUBLE)/>)dnl
m4_define(</SumGLA_OUTPUT/>, </(_sum, DOUBLE)/>)dnl
m4_define(</GBY_ATTS/>,</(mac, MACADDR)/>)

#ifndef _GBy_MAC_Sum_H_
#define _GBy_MAC_Sum_H_

#include "DataTypes.h"
#include <iostream>

using namespace std;

#include "SumGLA.h"

dnl
m4_include(GroupByGLA.h.m4)dnl

#endif // 


