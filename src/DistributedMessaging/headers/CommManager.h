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
#ifndef _COMM_MANAGER_H_
#define _COMM_MANAGER_H_

#include <string>
#include <pthread.h>

#include "EventProcessor.h"
#include "ProxyEventProcessor.h"
#include "Swap.h"
#include "TwoWayList.h"
#include "InefficientMap.h"
#include "CommListener.h"
#include "CommReceiver.h"
#include "CommSender.h"


using namespace std;


/**
	IP address of a host given by the actual address and the port.
*/
class HostAddressService {
public:
	//IPv4 address in readable form
	string address;
	//port
	off_t service;

	//constructors
	HostAddressService() {
		address = "";
		service = -1;
	}

	HostAddressService(string _address, off_t _service) {
		address = _address;
		service = _service;
	}

	// swap function so we can use swapping paradigm
	void swap(HostAddressService& other){
		SWAP(address, other.address);
		SWAP(service, other.service);
	}

	//equality check method
	bool IsEqual(const HostAddressService& other) {
		if ((address == other.address) && (service == other.service))
			return true;
		return false;
	}
};


/**
	Container for all the data structures used for the communication management.
	Data structures:
	- communication listener
	- (serviceId --> event processor) map; what service an event processor offers
	- (HostAddressPort --> communication receiver) map; what receivers are
		connected
	- (HostAddress --> communication sender) map; what is the communication sender
		to a given host
	- (HostAddressService --> proxy event processor) map

	A CommSender object is created for each remote host the current host is
	communicating with (indexed on the address of the remote host).

	A ProxyEventProcessor is created for each (address, service) pair corresponding
	to a remote host. All proxy event processor for the same remote host but for
	different services use the same common CommSender object to send their messages.
	Thus, there is a single CommSender for a remote host that relays messages for
	multiple ProxyEventProcessor objects for different services.
*/
class CommManager {
private:
	//singleton instance
	static CommManager singleton;

	//block the copy constructor
	CommManager(CommManager&);

	//clean the expired data structures for dead senders and receivers
	int CleanDead();

protected:
	//these are the members of the class used for management

	typedef Keyify<string> CString;
	typedef Keyify<int> CInt;
	typedef Keyify<off_t> COff_t;

	//the access is protected by a mutex
	pthread_mutex_t mutex;

	//communication listener
	CommListener listener;

	//receiving event processors, i.e. the event processors that do the
	//processing of the message
	typedef InefficientMap <COff_t, EventProcessor> ProcessorMap;
	ProcessorMap processors;

	//communication receiver event generators
	typedef InefficientMap <HostAddressPort, CommReceiver> ReceiverMap;
	ReceiverMap receivers;
	ReceiverMap deadReceivers;

	//socket file descriptors for communication receivers
	typedef InefficientMap <HostAddressPort, CInt> ReceiverSocketMap;
	ReceiverSocketMap receiversFd;

	//communication sender event processors
	typedef InefficientMap <CString, CommSender> SenderMap;
	SenderMap senders;
	SenderMap deadSenders;

	//communication senders proxy event processors
	//the only entity used to send remote messages
	typedef InefficientMap <HostAddressService, ProxyEventProcessor> ProxySenderMap;
	ProxySenderMap proxySenders;

public:
	//default constructor; initializes the manager
	CommManager(void);

	//function to get access to the singleton instance
	static CommManager& GetManager(void);

	//destructor
	virtual ~CommManager(void);

	//interface methods
	//start the communication manager
	int Start(void);

	//stop the communication manager
	int Stop(void);

	//register communication receiver
	int RegisterReceiver(const int _socketFd, const HostAddressPort _remoteHost);

	//eliminate a stopped communication receiver
	int EliminateReceiver(const HostAddressPort _remoteHost);

	//find the event processor that is responsible for a service
	int GetEventProcessor(const off_t _service, EventProcessor& _processor);

	//find the proxy event processor in charge of sending messages to address for service
	int FindRemoteEventProcessor(const string& _address, const off_t _service, ProxyEventProcessor& _where);

	//register who as the event processor for service
	int RegisterAsRemoteEventProcessor(EventProcessor& _who, const off_t _service);

	//unregister the event processor for service
	int UnregisterRemoteEventProcessor(const off_t _service);

	//stop communication listener (it already stopped by itself)
	int StopListener();

	//eliminate comm sender that does not function normally anymore
	//in addition, all the proxy event processors that use the sender are eliminated
	int EliminateSender(const string& _remoteMachine);
};

//inline methods
inline CommManager& CommManager::GetManager(void) {
	return singleton;
}

#endif // _COMM_MANAGER_H_
