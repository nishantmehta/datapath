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
#include <string>
#include <pthread.h>
#include <unistd.h>

#include "EventProcessor.h"
#include "ProxyEventProcessor.h"
#include "CommManager.h"
#include "CommListener.h"
#include "CommReceiver.h"
#include "CommSender.h"
#include "Message.h"
#include "RemoteMacros.h"
#include "CommConfig.h"
#include "Errors.h"

#include "InefficientMap.h"
#include "TwoWayList.h"
#include "InefficientMap.cc"
#include "TwoWayList.cc"


//default constructor
CommManager::CommManager() {
	//initialize members
	pthread_mutex_init(&mutex, NULL);

	//get the name of the host
	string machineName;
	IN_PLACE_STRING_CONSTRUCTOR(machineName, 100, buffer);
	gethostname(buffer, 100);

	//initialize the listener
	CommListener _listener(machineName, LISTEN_PORT);
	listener.swap(_listener);
}

//destructor
CommManager::~CommManager() {
//	WARNING("Destructor Begin.");
	// stop will take care of most stuff
	Stop();

	// destroy the listener
	CommListener _listener;
	listener.swap(_listener);

	// destroy the mutex
	pthread_mutex_destroy(&mutex);

//	WARNING("Destructor End.");
}


//start the communication manager
int CommManager::Start(void) {
	cout << "Start communication manager." << endl;

	//start the communication listener
	pthread_mutex_lock(&mutex);
	listener.Run();
	pthread_mutex_unlock(&mutex);

	return 0;
}


//stop the communication manager
int CommManager::Stop(void) {
//	WARNING("Stop Begin.");

	//clean the dead
	CleanDead();

	pthread_mutex_lock(&mutex);

	//senders and receivers are special; they need to be killed
	senders.MoveToStart();
	while (!senders.AtEnd()){
		CommSender& _sender = senders.CurrentData();
		KillEvProc(_sender);

		senders.Advance();
	}

	receivers.MoveToStart();
	while (!receivers.AtEnd()){
		CommReceiver& _receiver = receivers.CurrentData();
		_receiver.Kill();

		receivers.Advance();
	}

	processors.Clear();
	proxySenders.Clear();
	senders.Clear();
	receivers.Clear();

	//stop the listener
	listener.Kill();

	pthread_mutex_unlock(&mutex);

	cout << "Stop communication manager." << endl;

//	WARNING("Stop End.");

	return 0;
}


//register communication receiver
int CommManager::RegisterReceiver(const int socketFd, const HostAddressPort source) {
//	WARNING("RegisterReceiver Begin.");

	//clean the dead
	CleanDead();

	HostAddressPort _source1(source.address, source.port);
	HostAddressPort _source2(source.address, source.port);
	CInt _socketFd(socketFd);

	// bookkeeping
	pthread_mutex_lock(&mutex);
	//check if there already exists a receiver from the HostAddressPort
	if (receiversFd.IsThere(_source1)) {
		//there is already a similar receiver in
		//ignore the request and return
		//close the socket
		close(socketFd);

		pthread_mutex_unlock(&mutex);

//		WARNING("RegisterReceiver End.");

		return -1;
	}

	// create the new receiver
	CommReceiver _receiver(socketFd, source);
	// start the new receiver
	_receiver.Run();

	receiversFd.Insert(_source1, _socketFd);
	receivers.Insert(_source2, _receiver);
	pthread_mutex_unlock(&mutex);

//	WARNING("RegisterReceiver End.");

	return 0;
}


//eliminate a stopped communication receiver
int CommManager::EliminateReceiver(const HostAddressPort _remoteHost) {
	HostAddressPort key(_remoteHost);
	HostAddressPort _source1;
	HostAddressPort _source2;
	CInt _socketFd;
	CommReceiver _receiver;

	//clean the dead
	CleanDead();

//	WARNING("EliminateReceiver Begin.");

	pthread_mutex_lock(&mutex);
	receiversFd.Remove(key, _source1, _socketFd);
	receivers.Remove(key, _source2, _receiver);
	pthread_mutex_unlock(&mutex);

	// we have to make sure it is dead
	WARNING("Receiver [%s : %d] killed.", _source2.address.c_str(), _source2.port);

	//close the socket corresponding to the receiver
	_receiver.Close();

	pthread_mutex_lock(&mutex);
	deadReceivers.Insert(_source2, _receiver);
	pthread_mutex_unlock(&mutex);

//	WARNING("EliminateReceiver End.");

	return 0;
}


//find the event processor that is responsible for a service
int CommManager::GetEventProcessor(off_t _service, EventProcessor& _processor) {
	COff_t service(_service);

//	WARNING("GetEventProcessor Begin.");

	//access to the data structure is guarded
	pthread_mutex_lock(&mutex);
	int isIn = processors.IsThere(service);
	if (isIn) {
		//if the proxy event processor already exists, return it and exit
		_processor.copy(processors.Find(service));
		pthread_mutex_unlock(&mutex);

//		WARNING("GetEventProcessor End.");

		return 0;
	}

	//the processor for the asked service does not exist
	pthread_mutex_unlock(&mutex);

	WARNING("There is no event processor registered for service %ld!", (unsigned long)_service);

	return -1;
}


//find the proxy event processor in charge of sending messages to address for service
int CommManager::FindRemoteEventProcessor(const string& _address, const off_t _service,
	ProxyEventProcessor& _where) {
//	WARNING("FindRemoteEventProcessor Begin.");

	//clean the dead
	CleanDead();

	HostAddressService remoteHost(_address, _service);

	//access to the data structure is guarded
	pthread_mutex_lock(&mutex);
	int isIn = proxySenders.IsThere(remoteHost);
	if (isIn) {
		//if the proxy event processor already exists, return it and exit
		_where.copy(proxySenders.Find(remoteHost));
		pthread_mutex_unlock(&mutex);

//		WARNING("FindRemoteEventProcessor End.");

		return 0;
	}

	//this is the first time the proxy is called, thus we need to create it
	//there are two cases:
	//	- a CommSender already exists for the given address --> do not create it
	//	- create a new CommSender since it does not exist
	CString address(_address);

	isIn = senders.IsThere(address);
	if (isIn) {
		//the comm sender is already part of the communication framework
		CommSender& commSender = senders.Find(address);
		ProxyEventProcessor newProxySender(commSender.GetEvProc(), _service);
		_where.copy(newProxySender);

		proxySenders.Insert(remoteHost, newProxySender);
		pthread_mutex_unlock(&mutex);

//		WARNING("FindRemoteEventProcessor End.");

		return 0;
	}

	//create a new comm sender, start it, and add it to the communication framework
	CommSender commSender(_address);
	if (!commSender.IsActive()) {
		pthread_mutex_unlock(&mutex);

//		WARNING("FindRemoteEventProcessor End.");

		return -1;
	}

	commSender.ForkAndSpin();

	ProxyEventProcessor newProxySender(commSender.GetEvProc(), _service);
	_where.copy(newProxySender);

	senders.Insert(address, commSender);
	proxySenders.Insert(remoteHost, newProxySender);

	pthread_mutex_unlock(&mutex);

//	WARNING("FindRemoteEventProcessor End.");

	return 0;
}


//register who as the event processor for service
int CommManager::RegisterAsRemoteEventProcessor(EventProcessor& _who, const off_t _service) {
	COff_t service(_service);

//	WARNING("RegisterAsRemoteEventProcessor Begin.");

	//access to the data structure is guarded
	pthread_mutex_lock(&mutex);
	int isIn = processors.IsThere(service);
	if (isIn) {
		pthread_mutex_unlock(&mutex);

		WARNING("There already exists an event processor registered for service %lld!", _service);
		return -1;
	}

	//the processor for the asked service does not exist, add it
	EventProcessor addMe;
	addMe.copy(_who);
	processors.Insert(service, addMe);

	pthread_mutex_unlock(&mutex);

//	WARNING("RegisterAsRemoteEventProcessor End.");

	return 0;
}


//unregister the event processor for service
int CommManager::UnregisterRemoteEventProcessor(const off_t _service) {
	COff_t key(_service);
	COff_t _key;
	EventProcessor _data;

//	WARNING("UnregisterRemoteEventProcessor Begin.");

	pthread_mutex_lock(&mutex);
	processors.Remove(key, _key, _data);
	pthread_mutex_unlock(&mutex);

//	WARNING("UnregisterRemoteEventProcessor End.");

	return 0;
}


//stop communication listener (it already stopped by itself)
int CommManager::StopListener() {
//	WARNING("StopListener Begin.");
//	WARNING("StopListener End.");

	return 0;
}


//eliminate comm sender that does not function normally anymore
//in addition, all the proxy event processors that use the sender are eliminated
int CommManager::EliminateSender(const string& _remoteMachine) {
	CString key(_remoteMachine);
	CString _source;
	CommSender _sender;

	//clean the dead
	CleanDead();

//	WARNING("EliminateSender Begin.");

	pthread_mutex_lock(&mutex);
	//first eliminate the proxy senders that use the sender
	proxySenders.MoveToStart();
	while (!proxySenders.AtEnd())
	{
		HostAddressService& psKey = proxySenders.CurrentKey();
		if (psKey.address == _remoteMachine) {
			//eliminate the proxy event processor
			HostAddressService key1(psKey);
			HostAddressService _source1;
			ProxyEventProcessor _proxy;

			proxySenders.Remove(key1, _source1, _proxy);
		}
		else {
			proxySenders.Advance();
		}
	}

	senders.Remove(key, _source, _sender);

	pthread_mutex_unlock(&mutex);

	//close the socket corresponding to the sender
	_sender.Close();

	pthread_mutex_lock(&mutex);
	//then eliminate the sender in order to allow its correct delete
	deadSenders.Insert(_source, _sender);
	pthread_mutex_unlock(&mutex);

//	WARNING("EliminateSender End.");

	return 0;
}

//clean the expired data structures for dead senders and receivers
int CommManager::CleanDead() {
	pthread_mutex_lock(&mutex);

	//kill the dead senders
	deadSenders.MoveToStart();
	while (!deadSenders.AtEnd()){
		CommSender& _sender = deadSenders.CurrentData();
		KillEvProc(_sender);

		deadSenders.Advance();
	}

	deadSenders.Clear();
	deadReceivers.Clear();

	pthread_mutex_unlock(&mutex);

	return 0;
}
