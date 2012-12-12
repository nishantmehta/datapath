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

#ifndef _BULK_LOADER_INTERNAL_H_
#define _BULK_LOADER_INTERNAL_H_

#include "ContainerTypes.h"

/* Data types used by Bulk Loader to do its internal work. Should be
of no interest to the rest of the system */

M4_CREATE_DATA_TYPE(TextLoaderDS, Data,
</(stream, FILE*), (file, string)/>,<//>)

// container for the above
typedef TwoWayList<TextLoaderDS> TextLoaderDSContainer;

#endif // _BULK_LOADER_INTERNAL_H_
