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
// for Emacs -*- c++ -*_

#ifndef _WAYPOINTCG_H_
#define _WAYPOINTCG_H_

#include <iostream>
#include <fstream>
#include <dlfcn.h>

#include "SymbolicWaypointConfig.h"
#include "WayPointConfigureData.h"

using namespace std;

/** Base class for the Code Generator hierarchy
	* This class only specifies the interface all the Code Generation
	* classes need to provide and allows access to the Factory function.

	* When a new type of waypoint is developed, a new class should be
	* derived from this class and an entry should be added in
	* CGFactory.cc to deal with the new function.

	* The CGFactory.cc should be the only cc file that needs to be
	* compiled so it is preferable to have all the code writen for the
	* Code Generator objects in the same header file.

	* User code will only call the Factory function and never the Code
	* Generator objects themselves.

	* This object is not meant to be stored into TwoWayLists and thus it
	* does not implement the swapping paradigm. The idea is to store
	* these objects into a list of pointers to the base type temporarily
	* while the real configuration objects are build.
**/

class WayPointCG {
private:
	WayPointCG();
	WayPointCG(const WayPointCG&);
	WayPointCG& operator =(const WayPointCG&);

protected:
	SymbolicWaypointConfig myConf;
	string wpname;

public:
	// A constructor from SymbolicWaypointConfig object should be provided
	// It should look like this
	// This constructor has to be called instead of the default constructor
	// by the derived classes
	WayPointCG(SymbolicWaypointConfig& config);

	// This function writes the call to the M4 module in Main.m4 (stream)
	// The constructor already got access to all the info needed
	virtual void GenerateM4Code(ofstream& where)=0;

	// This method extracts the code and asembles it into the WayPointConfig
	// object that will be sent to the Execution Engine
	// The handle is the module returned by dlopen. It will be used to close
	// the module when safe to do so
	// The result is placed in the where parameter (by swapping)
	virtual void ExtractCode(void* handle, WayPointConfigureData& where)=0;

	// the destuctor so we get virtual tables
	virtual ~WayPointCG(void){ }
};

// Declaration of the Factory function
// Always return an object or FAILS if error encountered
WayPointCG *CGFactory(SymbolicWaypointConfig &config);


#endif // _WAYPOINTCG_H_
