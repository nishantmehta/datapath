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
#ifndef _REMOTE_MESSAGE_H_
#define _REMOTE_MESSAGE_H_

#include "Message.h"
#include "Archive.h"
#include "Swap.h"

#include <string>

//remote message type definition
#define REMOTE_MESSAGE_TYPE 1


using namespace std;


/**
	Header for all the network messages ever passed between two hosts.
*/
struct NetworkMessageHeader {
 // the type of the message as returned by Type()
	off_t type;

 // the service the message is addressed to
	off_t service;

	// the size of the message
	size_t size;

	// swap function so we can use swapping paradigm
	void swap(NetworkMessageHeader& other){
		SWAP(type, other.type);
		SWAP(service, other.service);
		SWAP(size, other.size);
	}
};


/**
	Base RemoteMessage class for all the remote messages that can be
	sent across the network. This class is derived from the base class Message
	because these messages need to be also recognized by EventProcessor.
*/
class RemoteMessage : public Message {
protected:
	//boolean indicator on how to treat the RemoteMessage:
	// true : as a specific RemoteMessage as given by its type
	// false : as a RemoteMessage in order to be uniformly treated by all
	//EventProcessor classes
	bool typeIndicator;

	// the service the message is destined to
	off_t service;

public:
	// constructor doing nothing
	RemoteMessage() : typeIndicator(true) {}
	RemoteMessage(bool _typeIndicator) : typeIndicator(_typeIndicator) {}

	virtual ~RemoteMessage() {}

	// Every new message type should redefine this
	static const off_t type = REMOTE_MESSAGE_TYPE;

	virtual off_t Type(void){ return REMOTE_MESSAGE_TYPE; }
	virtual const char* TypeName(void){ return "RemoteMessage"; }

	ARCHIVER_ACCESS_DECLARATION;

	ARCHIVER(ar){
		off_t myType = Type();
		ar & myType;
	}

	//methods to set/reset the typeIndicator
	// Type() will return the true type
	void SetRemote() { typeIndicator = true; }
	// Type() will return the RemoteMessage type
	void ResetRemote() { typeIndicator = false; }

	// get/set the service
	off_t GetService(void){ return service; }
	void SetService(off_t _service){  service = _service; }
};

ARCHIVER_ABSTRACT_CLASS(RemoteMessage);
ARCHIVER_CLASS_EXPORT(RemoteMessage);


/**
	Container class for all the messages that are recieved.

	This message is used as a container for the serialized message.
	The content is used to unpack the actual message.

	This message takes over the receiving buffer.
	The reciver should not reuse the buffer.
	The buffer is dealocated at the end of the process.
*/
class RemoteMessageContainer : public Message {
private:
	// the name of the machine from where the message is comming from
	string machine;

	// the read buffer from the socket (just the content)
	string buffer;

	//message header
	NetworkMessageHeader header;

public:
	// constructor from binary data from the network
	RemoteMessageContainer(string& _machine, string& _buffer, NetworkMessageHeader& _header /** swapped */):
		machine(_machine), buffer(_buffer) {
		header.swap(_header);
	}

	virtual ~RemoteMessageContainer(void){}

	virtual off_t Type(void){ off_t rez = header.type; return rez; }

	// this interface is only for extracting the real message from the container
	// this function gives up the string after this call
	string& GetSerializedContent(void){ return buffer; }

	// this message gives the name of machine from where the message is
	// comming from
	string& GetMachineName(void){ return machine; }

	// get the service the message is destined to
	off_t GetService(void){ off_t rez = header.service; return rez; }

	friend void RemoteMessageContainer_Factory(EventProcessor& dest, void* buffer,
		NetworkMessageHeader& header);
};

inline void RemoteMessageContainer_Factory(EventProcessor& dest, string& machine,
	string& buffer, NetworkMessageHeader& header){
	Message* msg = new RemoteMessageContainer(machine, buffer, header);
	dest.ProcessMessage(*msg);
}


#endif // _REMOTE_MESSAGE_H_
