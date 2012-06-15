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
include(DataFunctions.m4)

dnl # M4_INCLUDE_DATA_DEFS(ConfigDataBase.h.m4)
dnl embeded def since not working

#include "Data.h"
#include "ID.h"

// the base class of the hierarchy
M4_CREATE_BASE_DATA_TYPE(ConfigData, Data, 
  </(waypoint, WayPointID)/>, <//>)



#ifndef CONFIG_DATA_H_
#define CONFIG_DATA_H_

// #include "ConfigDataBase.h"

M4_CREATE_BASE_DATA_TYPE(ConfigData2, ConfigData, 
  </(active, bool)/>, <//>)

 M4_CREATE_DATA_TYPE(JoinConfigData, ConfigData,
 	</ ( lhs, int) />,
   </(slots, SlotContainer)/>)

 M4_CREATE_DATA_TYPE(Selection, ConfigData,
 	<//>,
   <//>)


#endif // CONFIG_DATA_H_
