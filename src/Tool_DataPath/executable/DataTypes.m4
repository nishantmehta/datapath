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
m4_divert(-1)
dnl # this file contains info on datatypes

m4_define(DT_TYPE_INT, DT_SIMPLE)
m4_define(DT_TYPE_BIGINT, DT_SIMPLE)
m4_define(DT_TYPE_IDENTIFIER, DT_SIMPLE)
m4_define(DT_TYPE_FLOAT, DT_SIMPLE)
m4_define(DT_TYPE_DOUBLE, DT_SIMPLE)
m4_define(DT_TYPE_DATE, DT_SIMPLE)
m4_define(DT_TYPE_Bitstring, DT_SIMPLE)
m4_define(DT_TYPE_VARCHAR, DT_COMPLEX)
m4_define(DT_TYPE_BOOL, DT_SIMPLE)
m4_define(DT_TYPE_IPV4ADDR, DT_SIMPLE)
m4_define(DT_TYPE_MACADDR, DT_SIMPLE)


m4_define(DT_SIZE_INT, 4)
m4_define(DT_SIZE_BOOL, 4)
m4_define(DT_SIZE_BIGINT, 8)
m4_define(DT_SIZE_IDENTIFIER, 8)
m4_define(DT_SIZE_FLOAT, 4)
m4_define(DT_SIZE_DOUBLE, 8)
m4_define(DT_SIZE_DATE, 4)
m4_define(DT_SIZE_Bitstring, 8)
m4_define(DT_SIZE_IPV4ADDR, 4)
m4_define(DT_SIZE_MACADDR, 8)

m4_define(GLA_REZTYPE_CountGLA, single)
m4_define(GLA_REZTYPE_SumGLA, single)
m4_define(GLA_REZTYPE_AverageGLA, single)
m4_define(GLA_REZTYPE_RiskGLA, single)
m4_define(GLA_REZTYPE_THistogramGLA, single)

m4_define(GLA_REZTYPE_GBy_IP_Sum, multi)
m4_define(GLA_REZTYPE_TopK_IP_Sum, multi)
m4_define(GLA_REZTYPE_GBy_MAC_Sum, multi)
m4_define(GLA_REZTYPE_TopK_MAC_Sum, multi)
m4_define(GLA_REZTYPE_GBy_IP_MAC_Sum, multi)
m4_define(GLA_REZTYPE_TopK_IP_MAC_Sum, multi)
m4_define(GLA_REZTYPE_GBy_MAC_THist, fragment)
m4_define(GLA_REZTYPE_GBy_INT_Count, multi)

m4_define(GLA_REZTYPE_MinProbGLA, multi)


dnl # m4_define(GLA_CHUNKBOUNDARY_AverageGLA)

m4_define(GLA_REZTYPE_RandAvgGLA, fragment)
m4_define(GLA_REZTYPE_HurricaneGLA, fragment)
m4_define(GLA_REZTYPE_HurricaneNormalGLA, fragment)
m4_define(GLA_REZTYPE_HurricaneZPolyGLA, fragment)
m4_define(GLA_CHUNKBOUNDARY_HurricaneZPolyGLA)
m4_define(GLA_CHUNKBOUNDARY_HurricaneNormalGLA)


m4_define(GLA_CTSTATE_KMeans, KMeansCTState)

m4_define(GLA_REZTYPE_DST_orders_all, fragment)
m4_divert(0)dnl
