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
#ifndef _COMM_RECEIVER_H_
#define _COMM_RECEIVER_H_

#include <iostream>

#include "EventGenerator.h"
#include "EventGeneratorImp.h"
#include "Swap.h"


/**
	IP address of a host given by the actual address and the port.
*/
class HostAddressPort {
public:
	//IPv4 address in readable form
	string address;
	//port
	int port;

	//constructors
	HostAddressPort() {
		address = "";
		port = -1;
	}

	HostAddressPort(string _address, int _port) {
		address = _address;
		port = _port;
	}

	// swap function so we can use swapping paradigm
	void swap(HostAddressPort& other){
		SWAP(address, other.address);
		SWAP(port, other.port);
	}

	//equality check method
	bool IsEqual(const HostAddressPort& other) {
		if ((address == other.address) && (port == other.port))
			return true;
		return false;
	}
};


/**
	Event generator for receiving messages from a remote host.
	The listening socket file descriptor and the address of the remote host are
	specified at construction.
	When a message is received, it is routed to the correct event processor as
	identified by the service in the message header.
*/
class CommReceiverImp : public EventGeneratorImp {
private:
	//listening socket file descriptor
	int socketFd;

	//source address
	HostAddressPort source;

public:
	//constructor & destructor
	CommReceiverImp(const int _socketFd, HostAddressPort _source);
	virtual ~CommReceiverImp();

	//method invoked for each received message
	virtual int ProduceMessage(void);

	//close the socket
	int Close();
};

class CommReceiver : public EventGenerator {
public:
	//empty constructor needed for initialization
	CommReceiver() {}
	virtual ~CommReceiver() {}

	//constructor that builds the implementation
	CommReceiver(const int _socketFd, HostAddressPort _source) {
		evGen = new CommReceiverImp(_socketFd, _source);
	}

	//close the socket
	int Close() {
		CommReceiverImp* receiver = dynamic_cast<CommReceiverImp*>(evGen);
		return receiver->Close();
	}
};


#endif // _COMM_RECEIVER_H_
