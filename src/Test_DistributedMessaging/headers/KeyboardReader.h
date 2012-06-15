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
#ifndef _KEYBOARD_READER_H_
#define _KEYBOARD_READER_H_

#include <iostream>

#include "Messages.h"
#include "EventGenerator.h"
#include "EventGeneratorImp.h"
#include "TestConf.h"
#include "CommunicationFramework.h"
#include "ProxyEventProcessor.h"

using namespace std;


/**
	Simple event generator that reads numbers from the input and sends them to a
	remote host over the network.
	A communication framework is assumed to have already been started.
*/
class KeyboardReaderImp : public EventGeneratorImp {
private:
	string machineName;

public:
	KeyboardReaderImp(const char* _machineName)
		: machineName(_machineName) {
		cout << "Insert numbers. Type 0 to finish." << endl;
	}

	virtual ~KeyboardReaderImp() {}

	virtual int ProduceMessage(void) {
		//read numbers from the standard input
		int myInt;
		cin >> myInt;
		cout << endl;

		//find the proxy sender corresponding to the remote machine
		ProxyEventProcessor proxySender;
		if (FindRemoteEventProcessor(machineName, SERVICE_ADD, proxySender) == 0) {
			//send the message to the identified proxy (and from there to the remote host)
			AddMessage_Factory(proxySender, myInt);
		}
		else {
			cout << "ERROR cannot send to " << machineName.c_str() << "." << endl;
		}

		return 0;
	}
};

class KeyboardReader : public EventGenerator {
public:
	KeyboardReader(const char* _machineName) {
		evGen = new KeyboardReaderImp(_machineName);
	}
	virtual ~KeyboardReader() {}
};


#endif // _KEYBOARD_READER_H_
