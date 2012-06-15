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
#include "ConfigData.h"
#include "ID.h"
#include "Waypoint.h"

int main(void){
	SlotContainer slots;
	SlotID slot(1);
	slots.Insert(slot);
	WayPointID jWP("JoinWP");
	int lhs=10;

	JoinConfigData jConf(jWP, lhs, slots);
	Data data;
	data.swap(jConf);

	ConfigData cData;
	cData.swap(data);

	FATALIF (cData.Type() != JoinConfigData::type, "Should have been JoinConfigData");

	ExecEngineData exec;

	return 0;
}
