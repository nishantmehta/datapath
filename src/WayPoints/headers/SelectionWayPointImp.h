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

#include "WayPointImp.h"

// this class is super-simple, since selection can use the default implementation of almost everything
class SelectionWayPointImp : public WayPointImp {

public:

	// const and destr
	SelectionWayPointImp ();
	virtual ~SelectionWayPointImp ();

	// this is the only method that the Selection actually needs to over-ride
	void ProcessHoppingDataMsg (HoppingDataMsg &data);
};
