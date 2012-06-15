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

#ifndef _GROUPBY_AVERAGE_H_
#define _GROUPBY_AVERAGE_H_

#include "DataTypes.h"
#include <iostream>

using namespace std;

#include "AverageGLA.h"

m4_define(</INNER_GLA/>, </AverageGLA/>)dnl
m4_define(</GLA_NAME/>, </GBy_Average/>)dnl
m4_define(</AverageGLA_INPUT/>, </(x, DOUBLE)/>)dnl
m4_define(</AverageGLA_OUTPUT/>, </(_count, BIGINT), (_sum, DOUBLE), (avg, DOUBLE)/>)dnl
m4_define(</GBY_ATTS/>,</(ip, IPV4ADDR), (i, INT)/>)
dnl
m4_include(GroupByGLA.h.m4)dnl

#endif // _GROUPBY_AVERAGE_H_


