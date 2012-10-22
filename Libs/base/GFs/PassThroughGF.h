//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
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

#ifndef _PASS_THROUGH_GF_
#define _PASS_THROUGH_GF_

#include "base/Types/bool.h"

/*  Meta-information
 *  GF_DESC
 *      NAME(</PassThroughGF/>)
 *      INPUTS(</(val, bool)/>)
 *  END_DESC
 *
 *  Just a simple pass-through to allow you to mix GFs and standard
 *  expressions in a single waypoint using meta-GFs.
 */

class PassThroughGF {

public:

    bool Filter( const bool& val ) const {
        return val;
    }
};

#endif // _PASS_THROUGH_GF_
