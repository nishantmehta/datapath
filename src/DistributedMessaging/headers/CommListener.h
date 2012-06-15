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
#ifndef _COMM_LISTENER_H_
#define _COMM_LISTENER_H_

#include <string>

#include "EventGenerator.h"
#include "EventGeneratorImp.h"

using namespace std;


/**
	Class responsible for accepting connections by the current machine
	specified by the (name:port) pair. Upon receiving a connection request from
	a different machine, the communication manager is invoked to register a
	communication receiver.
*/
class CommListenerImp : public EventGeneratorImp {
protected:
	//the name of the machine
	string machineName;

	//the port on which to listen for connections
	int port;

	//listening socket file descriptor
	int socketFd;

public:
	//constructor & destructor
	CommListenerImp(string _machineName, int _port);
	virtual ~CommListenerImp();

	//method inherited from the base class to generate messages for communication manager
	virtual int ProduceMessage(void);
};

class CommListener : public EventGenerator {
public:
	//empty constructor needed for initialization
	CommListener() {}
	virtual ~CommListener() {}

	//constructor with implementation provided
	CommListener(string _machineName, int _port) {
		evGen = new CommListenerImp(_machineName, _port);
	}
};


#endif // _COMM_LISTENER_H_
