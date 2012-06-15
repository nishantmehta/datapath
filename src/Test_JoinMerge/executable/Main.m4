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
dnl Code to generate the functions for the three waypoints join,selection,aggregate
dnl
include(Modules.m4)
M4_CODE_GENERATION_PREAMBLE

dnl M4_JOINMERGE_MODULE(WPJoin,</(LFirst,1),(LSecond,0),(LThird,0)/>,</(RFirst,1),(RSecond,2),(RThird,3)/>)
dnl arguments are:
dnl WPName, ListAttQrysLHSAccessed, ListAttQrysRHSAccessed, 
dnl ListAttQrysLHSCopied, ListAttQrysRHSCopied, QueryClassInfo
dnl ListAtts looks like </(att,querySetSer),../>
dnl QueryClassInfo looks like
dnl (QueryClass,ListHashAtts,ListJoinAttsPairs)
dnl QueryClass is the serialized QueryIDSet of the queries makin up the class
dnl LIstHashAtts: </ att1,att2, ../>
dnl ListJoinAttsPairs </ (lAtt,rAtt), .. />
M4_JOINMERGE_MODULE(WPJoin,dnl
</(LFirst,7),(LSecond,7),(LThird,7)/>,dnl ListAttQrysLHSAccessed
</(RFirst,7),(RSecond,7),(RThird,7)/>,dnl ListAttQrysRHSAccessed
</(LFirst,7),(LSecond,7),(LThird,7)/>,dnl ListAttQrysLHSCopied
</(RFirst,7),(RSecond,7),(RThird,7)/>,dnl ListAttQrysRHSCopied
</(7,((LSecond,RSecond),(LThird,RThird))),(7,((LSecond,RSecond)))/>)dnl list of queryIdSet to comparision attributes pair
